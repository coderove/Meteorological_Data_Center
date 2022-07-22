/*
* Copyright (C) 2022. All rights reserved.
* File name     : execsql.cpp
* Version       : v1.0
* Author        : starrove 
* Date          : 2022/7/17 20:29.
* Description   : 
**************************************************/

#include "_public.h"
#include "_mysql.h"

CLogFile logfile; //日志
connection conn;  //数据库连接对象
CPActive PActive;//进程心跳
//程序推出信号2、15的处理函数
void EXIT(int sig);

//帮助函数
void _help();

int main(int argc, char *argv[]) {

    //函数说明
    if (argc != 5) {
        _help();
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    //CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //打开日志
    if (!logfile.Open(argv[4])) {
        printf("[main]:打开日志文件[%s]失败！\n", argv[4]);
        return -1;
    }

    //PActive.AddPInfo(10,"execsql");   // 进程的心跳，10秒足够。

    //连接数据库 不启用事务
    if (conn.connecttodb(argv[2], argv[3]) != 0) {
        logfile.Write("main:连接数据库[%s]失败！---> [ %s ]\n", argv[2], conn.m_cda.message);
        return -1;
    }
    logfile.Write("main:连接数据库[%s]成功\n", argv[2]);

    CFile File;

    // 打开SQL文件。
    if (!File.Open(argv[1], "r")) {
        logfile.Write("File.Open(%s) failed.\n", argv[1]);
        EXIT(-1);
    }

    char strsql[1001];    // 存放从SQL文件中读取的SQL语句。

    while (true) {
        memset(strsql, 0, sizeof(strsql));

        // 从SQL文件中读取以分号结束的一行。
        if (!File.FFGETS(strsql, 1000, ";")) break;

        // 如果第一个字符是#，注释，不执行。
        if (strsql[0] == '#') continue;

        // 删除掉SQL语句最后的分号。
        char *pp = strstr(strsql, ";");
        if (pp == nullptr) continue;
        pp[0] = 0;

        logfile.Write("%s\n", strsql);

        int iret = conn.execute(strsql);  // 执行SQL语句。

        // 把SQL语句执行结果写日志。
        if (iret == 0) logfile.Write("exec ok[rpc=%d]\n", conn.m_cda.rpc);
        else logfile.Write("exec failed(%s).\n", conn.m_cda.message);

        PActive.UptATime();   // 进程的心跳。
    }

    logfile.WriteEx("\n");
    return 0;
}

//程序推出信号2、15的处理函数
void EXIT(int sig) {
    logfile.Write("程序退出，signal=[%d]\n\n", sig);
    conn.disconnect();  //断开数据库的连接
    exit(0);
}

void _help() {
    printf("'\n----------------------------------------------------------------------------------------\n");
    printf("[Using  ] :./execsql sqlfile connstr charset logfile\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("[Example] :./procctl 120 ./execsql /project/idc/sql/cleardata.sql \"127.0.0.1,root,123456,mdcsql,3306\" utf8 /project/log/idc/execsql.log\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("参数说明:\n\n");
    printf("sqlfile   :sql脚本文件名,每条sql语句可以多行书写,分号表示一条sql语句的结束,不支持注释。\n");
    printf("connstr   :数据库连接参数:ip,username,password,dbname,port\n");
    printf("charset   :数据库的字符集。\n");
    printf("logfile   :本程序运行的日志文件名。\n");
    printf("程序每120秒运行一次,由procctl调度。\n");
    printf("这是一个工具程序,用于执行一个sql脚本文件。\n");
    printf("'----------------------------------------------------------------------------------------\n\n");

}
/*
* Copyright (C) 2022. All rights reserved.
* File name     : standmindtodb.cpp
* Version       : v1.0
* Author        : starrove 
* Date          : 2022/7/17 16:14.
* Description   : 全国站点气象数据库操作 用于把全国站点分钟观测数据入库到数据库表中，支持xml和csv两种文件格式。
**************************************************/


#include "idcapp.h"


CLogFile logfile;  //日志
connection conn;   //数据库连接对象
CPActive PActive;  //进程心跳

//业务处理主函数
bool _standmindtodb(char *pathname, char *connstr, char *charset);

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
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //打开日志
    if (!logfile.Open(argv[4])) {
        printf("[main]:打开日志文件[%s]失败！\n", argv[4]);
        return -1;
    }

    PActive.AddPInfo(10,"standmindtodb");   // 进程的心跳，10秒足够。

    //业务处理主函数
    _standmindtodb(argv[1], argv[2], argv[3]);
    return 0;
}

//业务处理主函数
bool _standmindtodb(char *pathname, char *connstr, char *charset) {
    //sql语句
    sqlstatement stmt;

    int totalcount=0;  //文件的总记录数
    int insertcount=0; //文件成功插入的记录数
    CTimer Timer;      //计时器，记录每个数据文件的处理耗时
    bool bisxml=false;  // 文件格式，true-xml；false-csv。

    CZHOBTMIND ZHOBTMIND(&conn,&logfile);

    //打开目录
    CDir Dir;
    if (!Dir.OpenDir(pathname, "*.xml")) {
        logfile.Write("Dir.OpenDir(%s) 失败\n", pathname);
        return false;
    }

    CFile File;
    while (true) {
        //读取目录 得到一个数据文件名。
        if (!Dir.ReadDir()) { break; }
        if (MatchStr(Dir.m_FullFileName, "*.xml,*.csv")) bisxml=true;
        else bisxml=false;

        //连接数据库
        if (conn.m_state == 0) {
            if (conn.connecttodb(connstr, charset) != 0) {
                logfile.Write("main:连接数据库[%s]失败！---> [ %s ]\n", connstr, conn.m_cda.message);
                return -1;
            }
            logfile.Write("main:连接数据库[%s]成功\n", connstr);
        }

        //logfile.Write("filename[%s] \n", Dir.m_FullFileName);

        totalcount=insertcount=0;
        //打开文件
        if (!File.Open(Dir.m_FullFileName, "r")) {
            logfile.Write("File.Open[%s] 失败\n", Dir.m_FullFileName);
            return false;
        }
        char strBuffer[1024];    //存放文件中的读取的每一行

        while (true) {
            if (bisxml)
            {
                if (!File.FFGETS(strBuffer, 1023, "<endl/>")) break;
            }
            else
            {
                if (!File.Fgets(strBuffer, 1023, true)) break;
                if (strstr(strBuffer,"站点")!=nullptr) continue;   // 把csv文件中的第一行扔掉。
            }

            //logfile.Write("strBuffer - > [%s] \n", strBuffer);
            //处理文件中的每一行
            totalcount++;

            ZHOBTMIND.SplitBuffer(strBuffer,bisxml);

            if (ZHOBTMIND.InsertTable()) insertcount++;

        }
        //删除文件，提交事物。
        File.CloseAndRemove();
        //提交事物
        conn.commit();
        //处理情况
        logfile.Write("已处理文件 [%s] totalcount=[%d] insertcount=[%d] 耗时[%.2f]秒。\n",Dir.m_FullFileName,totalcount,insertcount,Timer.Elapsed());

    }
    return true;
}

//程序推出信号2、15的处理函数
void EXIT(int sig) {
    logfile.Write("程序退出，signal=[%d]\n\n", sig);
    conn.disconnect();  //断开数据库的连接
    exit(0);
}

void _help() {
    printf("'\n----------------------------------------------------------------------------------------\n");
    printf("[Using  ]  :./standmindtodb pathname connstr charset logfile\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("[Example]  :./procctl 10 ./standmindtodb /idcdata/surfdata \"127.0.0.1,root,123456,mdcsql,3306\" utf8 /project/log/idc/standmindtodb.log\n\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("参数文件:");
    printf("pathname   :全国站点分钟观测数据文件存放的目录。\n");
    printf("connstr    :数据库连接参数：ip,username,password,dbname,port\n");
    printf("charset    :数据库的字符集。\n");
    printf("logfile    :本程序运行的日志文件名。\n");
    printf("程序每10秒运行一次，由procctl调度。\n\n\n");
    printf("本程序用于把全国站点分钟观测数据保存到数据库的T_ZHOBTMIND表中，数据只插入，不更新。\n");
    printf("'----------------------------------------------------------------------------------------\n\n");

}
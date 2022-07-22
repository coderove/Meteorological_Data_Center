/*
* Copyright (C) 2022. All rights reserved.
* File name     : standtcodetodb.cpp
* Version       : v1.0
* Author        : starrove 
* Date          : 2022/7/17 14:23.
* Description   : 全国站点参数数据库操作
**************************************************/

#include "_public.h"
#include "_mysql.h"

struct st_stcode {
    char prov_name[31];    //省
    char stand_id[11];     //观测站id
    char stand_name[31];   //观测站名
    char lat[11];          //纬度    用字符串来表示数据库的空值！！！！
    char lon[11];          //经度
    char height[11];       //海拔
};

//存放全国气象数据的容器
vector<struct st_stcode> vstcode;

//数据存放到站点观测数据容器中
bool LoadSTcode(const char *inifile);

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
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //打开日志
    if (!logfile.Open(argv[4])) {
        printf("[main]:打开日志文件[%s]失败！\n", argv[4]);
        return -1;
    }

    PActive.AddPInfo(10,"standtcodetodb");   // 进程的心跳，10秒足够。

    //加载全国气象站点到vstcode容器
    if (!LoadSTcode(argv[1])) {
        logfile.Write("main:加载站点观测数据[%s]失败！\n", argv[1]);
        return -1;
    }
    logfile.Write("main:加载站点观测数据[%s]成功！,气象站点数[%d]\n", argv[1], vstcode.size());

    //连接数据库
    if (conn.connecttodb(argv[2], argv[3]) != 0) {
        logfile.Write("main:连接数据库[%s]失败！---> [ %s ]\n", argv[2], conn.m_cda.message);
        return -1;
    }
    logfile.Write("main:连接数据库[%s]成功\n", argv[2]);

    struct st_stcode stcode;
    //准备插入表的sql
    sqlstatement stmtins(&conn);
    stmtins.prepare(
            "insert into CODE_MDCTCODE(stand_id,stand_name,prov_name,lat,lon,height,upttime) values(:1,:2,:3,:4*100,"
            ":5*100,:6*10,now())");
    stmtins.bindin(1, stcode.stand_id, 10);
    stmtins.bindin(2, stcode.stand_name, 30);
    stmtins.bindin(3, stcode.prov_name, 30);
    stmtins.bindin(4, stcode.lat, 10);
    stmtins.bindin(5, stcode.lon, 10);
    stmtins.bindin(6, stcode.height, 10);

    //准备更新表的sql
    sqlstatement stmtupdate(&conn);
    stmtupdate.prepare(
            "update CODE_MDCTCODE set stand_name=:1,prov_name=:2,lat=:3*100,lon=:4*100,height=:5*10,upttime=now() where stand_id=:6");
    stmtupdate.bindin(1, stcode.stand_name, 30);
    stmtupdate.bindin(2, stcode.prov_name, 30);
    stmtupdate.bindin(3, stcode.lat, 10);
    stmtupdate.bindin(4, stcode.lon, 10);
    stmtupdate.bindin(5, stcode.height, 10);
    stmtupdate.bindin(6, stcode.stand_id, 10);

    //操作计数器
    int inscount=0,uptcount=0;
    //时间计数器
    CTimer ctimer;

    //遍历vstcode容器
    for (auto & i : vstcode) {
        //从容器取出一条记录到结构体stcode中。
        memcpy(&stcode, &i, sizeof(struct st_stcode));
        //执行插入sql
        if (stmtins.execute() != 0) {
            if (stmtins.m_cda.rc == 1062) {
                //记录已存在，就更新sql
                if (stmtupdate.execute() != 0) {
                    logfile.Write("main:更新数据库失败！---> [ %s ] [ %s ]\n", stmtupdate.m_sql, stmtupdate.m_cda.message);
                    return -1;
                }else{
                    uptcount++;
                }
            } else {
                logfile.Write("main:插入数据库失败！---> [ %s ]  [ %s ]\n", stmtins.m_sql, stmtins.m_cda.message);
                return -1;
            }
        }else{
            inscount++;
        }
    }

    // 把总记录数、插入记录数、更新记录数、消耗时长记录日志。
    logfile.Write("总记录数=[%d] 插入=[%d] 更新=[%d] 耗时=[%.2f]秒 \n",vstcode.size(),inscount,uptcount,ctimer.Elapsed());


    //提交事物
    conn.commit();
    return 0;
}

//数据存放到站点观测数据容器中
bool LoadSTcode(const char *inifile) {
    //1.打开文件
    CFile File;
    if (!File.Open(inifile, "r")) {
        logfile.Write("LoadSTcode:打开文件[%s]失败！\n", inifile);
        return false;
    }
    char strBuffer[301];
    CCmdStr CmdStr;
    struct st_stcode stcode;
    while (true) {
        //2.从站点参数文件中读取每一行，读取完 退出
        if (!File.Fgets(strBuffer, 300, true)) { break; }
        //3.把读取到的数据进行拆分
        CmdStr.SplitToCmd(strBuffer, ",", true);
        if (CmdStr.CmdCount() != 6) { continue; }                      //排除第一行
        //4.数据项保存到结构体
        CmdStr.GetValue(0, stcode.prov_name, 30);         //省
        CmdStr.GetValue(1, stcode.stand_id, 10);          //站号
        CmdStr.GetValue(2, stcode.stand_name, 30);        //站名
        CmdStr.GetValue(3, stcode.lat, 10);                //纬度
        CmdStr.GetValue(4, stcode.lon, 10);                //经度
        CmdStr.GetValue(5, stcode.height, 10);             //海拔高度
        //5.结构体存放到容器
        vstcode.push_back(stcode);
    }
    //6.关闭文件 析构函数自动关闭
//    for (auto it: vstcode) {
//        logfile.Write("prov_name = %s,stand_id=%s,stand_name=%s,lat=%.2f,lon=%.2f,"
//                      "height=.2f%\n", it.prov_name, it.stand_id, it.stand_name, it.lat, it.lon, it.height);
//    }
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
    printf("[Using  ] :./standtcodetodb inifile connstr charset logfile\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("[Example] :./procctl 120 ./standtcodetodb /project/idc/ini/stcode.ini \"127.0.0.1,root,123456,mdcsql,3306\" utf8 /project/log/idc/standtcodetodb.log\n");
    printf("'----------------------------------------------------------------------------------------\n");
    printf("参数说明:\n\n");
    printf("inifile   :站点参数文件名（全路径）。\n");
    printf("connstr   :数据库连接参数：ip,username,password,dbname,port\n");
    printf("charset   :数据库的字符集。\n");
    printf("logfile   :本程序运行的日志文件名。\n");
    printf("程序每120秒运行一次，由procctl调度。\n");
    printf("本程序用于把全国站点参数数据保存到数据库表中，如果站点不存在则插入，站点已存在则更新。\n");
    printf("'----------------------------------------------------------------------------------------\n\n");

}
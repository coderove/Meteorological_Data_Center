/*
* Copyright (C) 2022. All rights reserved.
* File name     : deletetable.cpp
* Version       : v1.0
* Author        : starrove 
* Date          : 2022/7/20 13:37.
* Description   : 本程序是数据中心的公共功能模块，用于定时清理表中的数据。
**************************************************/


#include "_public.h"
#include "_mysql.h"

struct st_arg {
    char connstr[101];     // 数据库的连接参数。
    char tname[31];        // 待清理的表名。
    char keycol[31];       // 待清理的表的唯一键字段名。
    char where[1001];      // 待清理的数据需要满足的条件。
    char starttime[31];    // 程序运行的时间区间。
    int timeout;          // 本程序运行时的超时时间。
    char pname[51];        // 本程序运行时的程序名。
} starg;

// 显示程序的帮助
void _help(char *argv[]);

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

CLogFile logfile;

// 判断当前时间是否在程序运行的时间区间内。
bool instarttime();

connection conn1;     // 用于执行查询SQL语句的数据库连接。
connection conn2;     // 用于执行删除SQL语句的数据库连接。

// 业务处理主函数。
bool _deletetable();

void EXIT(int sig);

CPActive PActive;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        _help(argv);
        return -1;
    }

    // 关闭全部的信号和输入输出
    // 处理程序退出的信号
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    if (!logfile.Open(argv[1], "a+")) {
        printf("打开日志文件失败（%s）。\n", argv[1]);
        return -1;
    }

    // 把xml解析到参数starg结构中
    if (!_xmltoarg(argv[2])) return -1;

    // 判断当前时间是否在程序运行的时间区间内。
    if (!instarttime()) return 0;

    PActive.AddPInfo(starg.timeout, starg.pname);
    // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
    // PActive.AddPInfo(starg.timeout*100,starg.pname);

    if (conn1.connecttodb(starg.connstr, nullptr) != 0) {
        logfile.Write("连接数据库(%s) 失败. -> %s\n", starg.connstr, conn1.m_cda.message);
        EXIT(-1);
    }

    if (conn2.connecttodb(starg.connstr, nullptr, 1) != 0)  // 开启自动提交。
    {
        logfile.Write("连接数据库(%s) 失败. -> %s\n", starg.connstr, conn2.m_cda.message);
        EXIT(-1);
    }

    // logfile.Write("connect database(%s) ok.\n",starg.connstr);

    // 业务处理主函数。
    _deletetable();
}


// 业务处理主函数。
bool _deletetable() {
    CTimer Timer;

    char tmpvalue[51];    // 存放从表提取待删除记录的唯一键的值。

    // 从表提取待删除记录的唯一键。
    sqlstatement stmtsel(&conn1);
    stmtsel.prepare("select %s from %s %s", starg.keycol, starg.tname, starg.where);
    stmtsel.bindout(1, tmpvalue, 50);

    // 拼接绑定删除SQL语句where 唯一键 in (...)的字符串。
    char bindstr[2001];
    char strtemp[11];

    memset(bindstr, 0, sizeof(bindstr));

    for (int i = 0; i < MAXPARAMS; i++) {
        memset(strtemp, 0, sizeof(strtemp));
        sprintf(strtemp, ":%lu,", i + 1);
        strcat(bindstr, strtemp);
    }

    bindstr[strlen(bindstr) - 1] = 0;    // 最后一个逗号是多余的。

    char keyvalues[MAXPARAMS][51];   // 存放唯一键字段的值。

    // 准备删除数据的SQL，一次删除MAXPARAMS条记录。
    sqlstatement stmtdel(&conn2);
    stmtdel.prepare("delete from %s where %s in (%s)", starg.tname, starg.keycol, bindstr);
    for (int i = 0; i < MAXPARAMS; i++) { stmtdel.bindin(i + 1, keyvalues[i], 50); }

    int ccount = 0;
    memset(keyvalues, 0, sizeof(keyvalues));

    if (stmtsel.execute() != 0) {
        logfile.Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
        return false;
    }

    while (true) {
        memset(tmpvalue, 0, sizeof(tmpvalue));

        // 获取结果集。
        if (stmtsel.next() != 0) break;

        strcpy(keyvalues[ccount], tmpvalue);
        ccount++;

        // 每MAXPARAMS条记录执行一次删除语句。
        if (ccount == MAXPARAMS) {
            if (stmtdel.execute() != 0) {
                logfile.Write("stmtdel.execute() failed.\n%s\n%s\n", stmtdel.m_sql, stmtdel.m_cda.message);
                return false;
            }

            ccount = 0;
            memset(keyvalues, 0, sizeof(keyvalues));

            PActive.UptATime();
        }
    }

    // 如果不足MAXPARAMS条记录，再执行一次删除。
    if (ccount > 0) {
        if (stmtdel.execute() != 0) {
            logfile.Write("stmtdel.execute() failed.\n%s\n%s\n", stmtdel.m_sql, stmtdel.m_cda.message);
            return false;
        }
    }

    if (stmtsel.m_cda.rpc > 0)
        logfile.Write("delete from %s %d rows in %.02fsec.\n", starg.tname, stmtsel.m_cda.rpc, Timer.Elapsed());

    return true;
}

// 判断当前时间是否在程序运行的时间区间内。
bool instarttime() {
    // 程序运行的时间区间，例如02,13表示：如果程序启动时，踏中02时和13时则运行，其它时间不运行。
    if (strlen(starg.starttime) != 0) {
        char strHH24[3];
        memset(strHH24, 0, sizeof(strHH24));
        LocalTime(strHH24, "hh24");  // 只获取当前时间中的小时。
        if (strstr(starg.starttime, strHH24) == nullptr) return false;
    }

    return true;
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer) {
    memset(&starg, 0, sizeof(struct st_arg));

    GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr, 100);
    if (strlen(starg.connstr) == 0) {
        logfile.Write("connstr is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "tname", starg.tname, 30);
    if (strlen(starg.tname) == 0) {
        logfile.Write("tname is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "keycol", starg.keycol, 30);
    if (strlen(starg.keycol) == 0) {
        logfile.Write("keycol is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "where", starg.where, 1000);
    if (strlen(starg.where) == 0) {
        logfile.Write("where is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "starttime", starg.starttime, 30);

    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
    if (starg.timeout == 0) {
        logfile.Write("timeout is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
    if (strlen(starg.pname) == 0) {
        logfile.Write("pname is null.\n");
        return false;
    }

    return true;
}

void EXIT(int sig) {
    logfile.Write("程序退出，signal=[%d]\n\n", sig);
    conn1.disconnect();
    conn2.disconnect();
    exit(0);
}

// 函数的说明
void _help(char *argv[]) {
    printf("\n----------------------------------------------------------------------------------------\n");
    printf("[Using ] :./deletetable logfilename xmlbuffer\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[Sample] :./procctl 3600 ./deletetable /project/log/idc/deletetable_MDCMIND1.log \"<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><tname>CODE_MDCMIND1</tname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-120,now())</where><starttime>01,02,03,04,05,13</starttime><timeout>120</timeout><pname>deletetable_MDCMIND1</pname>\"\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("参数说明:\n\n");
    printf("logfilename :本程序运行的日志文件。\n");
    printf("xmlbuffer   :本程序运行的参数，用xml表示，具体如下：\n");
    printf("connstr     :数据库的连接参数，格式：ip,username,password,dbname,port。\n");
    printf("tname       :待清理数据表的表名。\n");
    printf("keycol      :待清理数据表的唯一键字段名。\n");
    printf("starttime   :程序运行的时间区间，例如02,13表示：如果程序运行时，踏中02时和13时则运行，其它时间\n"\
                                "            不运行。如果starttime为空，本参数将失效，只要本程序启动就会执行数据迁移，为了减少\n"\
                                "            对数据库的压力，数据迁移一般在数据库最闲的时候时进行。\n");
    printf("where       :待清理的数据需要满足的条件，即SQL语句中的where部分。\n");
    printf("timeout     :本程序的超时时间，单位：秒，建议设置120以上。\n");
    printf("pname       :进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n");
    printf("本程序是数据中心的公共功能模块，用于定时清理表中的数据。\n");
    printf("----------------------------------------------------------------------------------------\n\n");
}
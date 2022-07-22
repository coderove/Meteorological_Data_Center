/*
* Copyright (C) 2022. All rights reserved.
* File name     : dminingmysql.cpp
* Version       : v1.0
* Author        : starrove 
* Date          : 2022/7/18 8:34.
* Description   : 数据中心公共功能模块，用于从mysql数据库源表抽取数据，生成xml文件。
**************************************************/



#include "_public.h"
#include "_mysql.h"


// 程序运行参数的结构体。
// 程序运行参数的结构体。
struct st_arg {
    char connstr[101];     // 数据库的连接参数。
    char charset[51];      // 数据库的字符集。
    char selectsql[1024];  // 从数据源数据库抽取数据的SQL语句。
    char fieldstr[501];    // 抽取数据的SQL语句输出结果集字段名，字段名之间用逗号分隔。
    char fieldlen[501];    // 抽取数据的SQL语句输出结果集字段的长度，用逗号分隔。
    char bfilename[31];    // 输出xml文件的前缀。
    char efilename[31];    // 输出xml文件的后缀。
    char outpath[301];     // 输出xml文件存放的目录。
    int maxcount;          // 输出xml文件最大记录数，0表示无限制。
    char starttime[52];    // 程序运行的时间区间
    char incfield[31];     // 递增字段名。
    char incfilename[301]; // 已抽取数据的递增字段最大值存放的文件。
    char connstr1[101];    // 已抽取数据的递增字段最大值存放的数据库的连接参数。
    int timeout;           // 进程心跳的超时时间。
    char pname[51];        // 进程名，建议用"dminingmysql_后缀"的方式。
} starg;

#define MAXFIELDCOUNT  100    // 结果集字段的最大数。
//#define MAXFIELDLEN    500  // 结果集字段值的最大长度。
int MAXFIELDLEN=-1;           // 结果集字段值的最大长度，存放fieldlen数组中元素的最大值。

char strfieldname[MAXFIELDCOUNT][31];    // 结果集字段名数组，从starg.fieldstr解析得到。
int  ifieldlen[MAXFIELDCOUNT];           // 结果集字段的长度数组，从starg.fieldlen解析得到。
int  ifieldcount;                        // strfieldname和ifieldlen数组中有效字段的个数。
int  incfieldpos=-1;                     // 递增字段在结果集数组中的位置。

connection conn,conn1;

CLogFile logfile;

// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer);

// 判断当前时间是否在程序运行的时间区间内。
bool instarttime();

// 数据抽取的主函数。
bool _dminingmysql();

CPActive PActive;  // 进程心跳。

char strxmlfilename[301]; // xml文件名。
void crtxmlfilename();    // 生成xml文件名。

long imaxincvalue;       // 自增字段的最大值。
bool readincfield();     // 从数据库表中或starg.incfilename文件中获取已抽取数据的最大id。
bool writeincfield();    // 把已抽取数据的最大id写入数据库表或starg.incfilename文件。

// 程序退出和信号2、15的处理函数。
void EXIT(int sig);

void _help();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        _help();
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    //CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //打开日志文件
    if (!logfile.Open(argv[1])) {
        printf("[ftpputfiles->main] :打开日志文件[%s]失败！\n", argv[1]);
        return -1;
    }
    //解析xml，得到运行参数
    if (!_xmltoarg(argv[2])) { return -1; }

    // 判断当前时间是否在程序运行的时间区间内。
    if (!instarttime()) return 0;

    PActive.AddPInfo(starg.timeout,starg.pname);  // 把进程的心跳信息写入共享内存。
    // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
    // PActive.AddPInfo(5000,starg.pname);

    // 连接数据源的数据库。
    if (conn.connecttodb(starg.connstr,starg.charset)!=0)
    {
        logfile.Write("连接 [数据源] 数据库[%s] 失败 --> [%s]\n",starg.connstr,conn.m_cda.message); return -1;
    }
    logfile.Write("连接 [数据源] 数据库[%s] 成功.\n",starg.connstr);

    // 连接本地的数据库，用于存放已抽取数据的自增字段的最大值。
    if (strlen(starg.connstr1)!=0)
    {
        if (conn1.connecttodb(starg.connstr1,starg.charset)!=0)
        {
            logfile.Write("连接 [本地] 数据库[%s] 失败 --> [%s]\n",starg.connstr1,conn1.m_cda.message); return -1;
        }
        logfile.Write("连接 [本地] 数据库[%s] 成功.\n",starg.connstr1);
    }

    //进入ftp文件目录
    _dminingmysql();

}

// 数据抽取的主函数。
bool _dminingmysql()
{
    // 从数据库表中或starg.incfilename文件中获取已抽取数据的最大id。
    readincfield();

    sqlstatement stmt(&conn);
    stmt.prepare(starg.selectsql);
    char strfieldvalue[ifieldcount][MAXFIELDLEN+1];  // 抽取数据的SQL执行后，存放结果集字段值的数组。
    for (int ii=1;ii<=ifieldcount;ii++)
    {
        stmt.bindout(ii,strfieldvalue[ii-1],ifieldlen[ii-1]);
    }

    // 如果是增量抽取，绑定输入参数（已抽取数据的最大id）。
    if (strlen(starg.incfield)!=0) stmt.bindin(1,&imaxincvalue);

    if (stmt.execute()!=0)
    {
        logfile.Write("stmt.execute() 失败 --> [%s]  [%s]\n",stmt.m_sql,stmt.m_cda.message); return false;
    }

    PActive.UptATime();

    CFile File;   // 用于操作xml文件。

    while (true)
    {
        memset(strfieldvalue,0,sizeof(strfieldvalue));

        if (stmt.next()!=0) break;

        if (!File.IsOpened())
        {
            crtxmlfilename();   // 生成xml文件名。

            if (!File.OpenForRename(strxmlfilename, "w+"))
            {
                logfile.Write("File.OpenForRename(%s) 失败.\n",strxmlfilename); return false;
            }

            File.Fprintf("<data>\n");
        }

        for (int ii=1;ii<=ifieldcount;ii++)
            File.Fprintf("<%s>%s</%s>",strfieldname[ii-1],strfieldvalue[ii-1],strfieldname[ii-1]);

        File.Fprintf("<endl/>\n");

        // 如果记录数达到starg.maxcount行就切换一个xml文件。
        if ( (starg.maxcount>0) && (stmt.m_cda.rpc%starg.maxcount==0) )
        {
            File.Fprintf("</data>\n");

            if (!File.CloseAndRename())
            {
                logfile.Write("File.CloseAndRename(%s) 失败.\n",strxmlfilename); return false;
            }

            logfile.Write("生成文件[%s(%d)]。\n",strxmlfilename,starg.maxcount);

            PActive.UptATime();
        }

        // 更新自增字段的最大值。
        if ( (strlen(starg.incfield)!=0) && (imaxincvalue<atol(strfieldvalue[incfieldpos])) )
            imaxincvalue=atol(strfieldvalue[incfieldpos]);
    }

    if (File.IsOpened())
    {
        File.Fprintf("</data>\n");

        if (!File.CloseAndRename())
        {
            logfile.Write("File.CloseAndRename(%s) 失败.\n",strxmlfilename); return false;
        }

        if (starg.maxcount==0)
            logfile.Write("生成文件 [%s(%d)]。\n",strxmlfilename,stmt.m_cda.rpc);
        else
            logfile.Write("生成文件 [%s(%d)]。\n",strxmlfilename,stmt.m_cda.rpc%starg.maxcount);
    }

    // 把已抽取数据的最大id写入数据库表或starg.incfilename文件。
    if (stmt.m_cda.rpc>0) writeincfield();

    return true;
}

// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer) {
    memset(&starg, 0, sizeof(struct st_arg));

    GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr, 100);
    if (strlen(starg.connstr) == 0) {
        logfile.Write("connstr is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "charset", starg.charset, 50);
    if (strlen(starg.charset) == 0) {
        logfile.Write("charset is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "selectsql", starg.selectsql, 1000);
    if (strlen(starg.selectsql) == 0) {
        logfile.Write("selectsql is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "fieldstr", starg.fieldstr, 500);
    if (strlen(starg.fieldstr) == 0) {
        logfile.Write("fieldstr is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "fieldlen", starg.fieldlen, 500);
    if (strlen(starg.fieldlen) == 0) {
        logfile.Write("fieldlen is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "bfilename", starg.bfilename, 30);
    if (strlen(starg.bfilename) == 0) {
        logfile.Write("bfilename is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "efilename", starg.efilename, 30);
    if (strlen(starg.efilename) == 0) {
        logfile.Write("efilename is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "outpath", starg.outpath, 300);
    if (strlen(starg.outpath) == 0) {
        logfile.Write("outpath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "starttime", starg.starttime, 50);  // 可选参数。

    GetXMLBuffer(strxmlbuffer, "incfield", starg.incfield, 30);  // 可选参数。

    GetXMLBuffer(strxmlbuffer, "incfilename", starg.incfilename, 300);  // 可选参数。

    GetXMLBuffer(strxmlbuffer, "maxcount", &starg.maxcount);  // 可选参数。

    GetXMLBuffer(strxmlbuffer, "connstr1", starg.connstr1, 100);  // 可选参数。

    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);   // 进程心跳的超时时间。
    if (starg.timeout == 0) {
        logfile.Write("timeout is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);     // 进程名。
    if (strlen(starg.pname) == 0) {
        logfile.Write("pname is null.\n");
        return false;
    }

    // 1、把starg.fieldlen解析到ifieldlen数组中；
    CCmdStr CmdStr;

    CmdStr.SplitToCmd(starg.fieldlen, ",");

    // 判断字段数是否超出MAXFIELDCOUNT的限制。
    if (CmdStr.CmdCount() > MAXFIELDCOUNT) {
        logfile.Write("fieldlen的字段数太多，超出了最大限制[%d]。\n", MAXFIELDCOUNT);
        return false;
    }

    for (int i = 0; i < CmdStr.CmdCount(); i++) {
        CmdStr.GetValue(i, &ifieldlen[i]);
        // if (ifieldlen[ii]>MAXFIELDLEN) ifieldlen[ii]=MAXFIELDLEN;   // 字段的长度不能超过MAXFIELDLEN。
        if (ifieldlen[i] > MAXFIELDLEN) MAXFIELDLEN = ifieldlen[i];   // 得到字段长度的最大值。
    }

    ifieldcount = CmdStr.CmdCount();

    // 2、把starg.fieldstr解析到strfieldname数组中；
    CmdStr.SplitToCmd(starg.fieldstr, ",");

    // 判断字段数是否超出MAXFIELDCOUNT的限制。
    if (CmdStr.CmdCount() > MAXFIELDCOUNT) {
        logfile.Write("fieldstr的字段数太多，超出了最大限制[%d]。\n", MAXFIELDCOUNT);
        return false;
    }

    for (int i = 0; i < CmdStr.CmdCount(); i++) {
        CmdStr.GetValue(i, strfieldname[i], 30);
    }

    // 判断strfieldname和ifieldlen两个数组中的字段是否一致。
    if (ifieldcount != CmdStr.CmdCount()) {
        logfile.Write("[fieldstr]和[fieldlen]的元素数量不一致。\n");
        return false;
    }

    // 3、获取自增字段在结果集中的位置。
    if (strlen(starg.incfield) != 0) {
        for (int i = 0; i < ifieldcount; i++) {
            if (strcmp(starg.incfield, strfieldname[i]) == 0) {
                incfieldpos = i;
                break;
            }
        }

        if (incfieldpos == -1) {
            logfile.Write("递增字段名[%s]不在列表[%s]中。\n", starg.incfield, starg.fieldstr);
            return false;
        }

        if ((strlen(starg.incfilename) == 0) && (strlen(starg.connstr1) == 0)) {
            logfile.Write("[incfilename]和[connstr1]参数必须二选一。\n");
            return false;
        }
    }

    return true;
}

// 判断当前时间是否在程序运行的时间区间内。
bool instarttime()
{
    // 程序运行的时间区间，例如02,13表示：如果程序启动时，踏中02时和13时则运行，其它时间不运行。
    if (strlen(starg.starttime)!=0)
    {
        char strHH24[3];
        memset(strHH24,0,sizeof(strHH24));
        LocalTime(strHH24,"hh24");  // 只获取当前时间中的小时。
        if (strstr(starg.starttime,strHH24)==nullptr) return false;
    }

    return true;
}
// 生成xml文件名。
void crtxmlfilename(){
    // xml全路径文件名=start.outpath+starg.bfilename+当前时间+starg.efilename+序号.xml
    char strLocalTime[21];
    memset(strLocalTime,0,sizeof(strLocalTime));
    LocalTime(strLocalTime,"yyyymmddhh24miss");

    static int iseq=1;
    SNPRINTF(strxmlfilename,300,sizeof(strxmlfilename),"%s/%s_%s_%s_%d.xml",starg.outpath,starg.bfilename,strLocalTime,starg.efilename,iseq++);
}

// 从数据库表中或starg.incfilename文件中获取已抽取数据的最大id。
bool readincfield()
{
    imaxincvalue=0;    // 自增字段的最大值。

    // 如果starg.incfield参数为空，表示不是增量抽取。
    if (strlen(starg.incfield)==0) return true;

    if (strlen(starg.connstr1)!=0)
    {
        // 从数据库表中加载自增字段的最大值。
        // create table CODE_MAXINCVALUE(pname varchar(50),maxincvalue numeric(15),primary key(pname));
        sqlstatement stmt(&conn1);
        stmt.prepare("select maxincvalue from CODE_MAXINCVALUE where pname=:1");
        stmt.bindin(1,starg.pname,50);
        stmt.bindout(1,&imaxincvalue);
        stmt.execute();
        stmt.next();
    }
    else
    {
        // 从文件中加载自增字段的最大值。
        CFile File;

        // 如果打开starg.incfilename文件失败，表示是第一次运行程序，也不必返回失败。
        // 也可能是文件丢了，那也没办法，只能重新抽取。
        if (!File.Open(starg.incfilename, "r")) return true;

        // 从文件中读取已抽取数据的最大id。
        char strtemp[31];
        File.FFGETS(strtemp,30);
        imaxincvalue=atol(strtemp);
    }

    logfile.Write("上次已抽取数据的位置 [%s=%ld]。\n",starg.incfield,imaxincvalue);

    return true;
}

// 把已抽取数据的最大id写入starg.incfilename文件。
bool writeincfield()
{
    // 如果starg.incfield参数为空，表示不是增量抽取。
    if (strlen(starg.incfield)==0) return true;

    if (strlen(starg.connstr1)!=0)
    {
        // 把自增字段的最大值写入数据库的表。
        // create table CODE_MAXINCVALUE(pname varchar(50),maxincvalue numeric(15),primary key(pname));
        sqlstatement stmt(&conn1);
        stmt.prepare("update CODE_MAXINCVALUE set maxincvalue=:1 where pname=:2");
        if (stmt.m_cda.rc==1146)
        {
            // 如果表不存在，就创建表。
            conn1.execute("create table CODE_MAXINCVALUE(pname varchar(50),maxincvalue numeric(15),primary key(pname))");
            conn1.execute("insert into CODE_MAXINCVALUE values('%s',%ld)",starg.pname,imaxincvalue);
            conn1.commit();
            return true;
        }
        stmt.bindin(1,&imaxincvalue);
        stmt.bindin(2,starg.pname,50);
        if (stmt.execute()!=0)
        {
            logfile.Write("stmt.execute() 失败.---> [%s]  [%s]\n",stmt.m_sql,stmt.m_cda.message); return false;
        }
        if (stmt.m_cda.rpc==0)
        {
            conn1.execute("insert into CODE_MAXINCVALUE values('%s',%ld)",starg.pname,imaxincvalue);
        }
        conn1.commit();
    }
    else
    {
        // 把自增字段的最大值写入文件。
        CFile File;

        if (!File.Open(starg.incfilename, "w+"))
        {
            logfile.Write("File.Open(%s) 失败.\n",starg.incfilename); return false;
        }

        // 把已抽取数据的最大id写入文件。
        File.Fprintf("%ld",imaxincvalue);

        File.Close();
    }

    return true;
}

//程序推出信号2、15的处理函数
void EXIT(int sig) {
    logfile.Write("程序退出，signal=[%d]\n\n", sig);
    exit(0);
}

//帮助函数
void _help() {
    printf("\n----------------------------------------------------------------------------------------\n");
    printf("[Using ]    :./dminingmysql logfilename xmlbuffer\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[Sample]    :./procctl 3600 ./dminingmysql /project/log/idc/dminingmysql_CODE_MDCTCODE.log \"<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><charset>utf8</charset><selectsql>select stand_id,stand_name,prov_name,lat,lon,height from CODE_MDCTCODE</selectsql><fieldstr>stand_id,stand_name,prov_name,lat,lon,height</fieldstr><fieldlen>10,30,30,10,10,10</fieldlen><bfilename>MDCTCODE</bfilename><efilename>MDCCZ</efilename><outpath>/idcdata/dmindata</outpath><timeout>30</timeout><pname>dminingmysql_CODE_MDCTCODE</pname>\"\n");
    printf("             ./procctl  30  ./dminingmysql /project/log/idc/dminingmysql_CODE_MDCMIND.log   \"<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><charset>utf8</charset><selectsql>select stand_id,date_format(ddatetime,'%%%%Y-%%%%m-%%%%d %%%%H:%%%%i:%%%%s'),t,p,u,wd,wf,r,vis,keyid from CODE_MDCMIND where keyid>:1 and ddatetime>timestampadd(minute,-120,now())</selectsql><fieldstr>stand_id,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>10,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>MDCMIND</bfilename><efilename>MDCCZ</efilename><outpath>/idcdata/dmindata</outpath><starttime></starttime><incfield>keyid</incfield><incfilename>/idcdata/dmining/dminingmysql_CODE_MDCMIND_MDCMIND.list</incfilename><timeout>30</timeout><pname>dminingmysql_MDCMIND_MDCMIND</pname><maxcount>1000</maxcount><connstr1>127.0.0.1,root,123456,mdcsql,3306</connstr1>\"\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("参数说明:\n\n");
    printf("logfilename :本程序运行的日志文件。\n");
    printf("xmlbuffer   :本程序运行的参数，用xml表示，具体如下：\n");
    printf("connstr     :数据库的连接参数，格式：ip,username,password,dbname,port。\n");
    printf("charset     :数据库的字符集，这个参数要与数据源数据库保持一致，否则会出现中文乱码的情况。\n");
    printf("selectsql   :从数据源数据库抽取数据的SQL语句，注意：时间函数的百分号%需要四个，显示出来才有两个，被prepare之后将剩一个。\n");
    printf("fieldstr    :抽取数据的SQL语句输出结果集字段名，中间用逗号分隔，将作为xml文件的字段名。\n");
    printf("fieldlen    :抽取数据的SQL语句输出结果集字段的长度，中间用逗号分隔。fieldstr与fieldlen的字段必须一一对应。\n");
    printf("bfilename   :输出xml文件的前缀。\n");
    printf("efilename   :输出xml文件的后缀。\n");
    printf("outpath     :输出xml文件存放的目录。\n");
    printf("maxcount    :输出xml文件的最大记录数，缺省是0，表示无限制，如果本参数取值为0，注意适当加大timeout的取值，防止程序超时。\n");
    printf("starttime   :程序运行的时间区间，例如02,13表示：如果程序启动时，踏中02时和13时则运行，其它时间不运行。"\
                                 "如果starttime为空，那么starttime参数将失效，只要本程序启动就会执行数据抽取，为了减少数据源"\
                                 "的压力，从数据库抽取数据的时候，一般在对方数据库最闲的时候时进行。\n");
    printf("incfield    :递增字段名，它必须是fieldstr中的字段名，并且只能是整型，一般为自增字段。"\
                                 "如果incfield为空，表示不采用增量抽取方案。");
    printf("incfilename :已抽取数据的递增字段最大值存放的文件，如果该文件丢失，将重新抽取全部的数据。\n");
    printf("connstr1    :已抽取数据的递增字段最大值存放的数据库的连接参数。connstr1和incfilename二选一，connstr1优先。");
    printf("timeout     :本程序的超时时间，单位：秒。\n");
    printf("pname       :进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n");
    printf("本程序是数据中心的公共功能模块，用于从MySQL数据库源表抽取数据，生成xml文件。\n");
    printf("----------------------------------------------------------------------------------------\n\n");

}


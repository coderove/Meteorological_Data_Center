/**
* @file ftpgetfiles.cpp
* @brief 文件下载模块
* @author coderove
* @date 2022-7-13
* @copyright Copyright (c) 2022年 coderove All rights reserved.
*/

#ifndef PROJECT_FTPGETFILES_H
#define PROJECT_FTPGETFILES_H

#include "_public.h"
#include "_ftp.h"

Cftp ftp;         //ftp操作
CLogFile logfile; //日志

// 程序运行参数的结构体。
struct st_arg {
    char host[31];           // 远程服务端的IP和端口。
    int mode;                // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
    char username[31];       // 远程服务端ftp的用户名。
    char password[31];       // 远程服务端ftp的密码。
    char remotepath[301];    // 远程服务端存放文件的目录。
    char localpath[301];     // 本地文件存放的目录。
    char matchname[101];     // 待下载文件匹配的规则。
    char listfilename[301];  //下载前列出服务器文件名的文件
    int ptype;               // 下载后服务端文件的处理方式：1-什么也不做；2-删除；3-备份。
    char remotepathbak[301]; // 下载后服务端文件的备份目录。
    char okfilename[301];    // 已下载成功文件名清单。
    bool checkmtime;         // 是否需要检查服务端文件的时间，true-需要，false-不需要，缺省为false。
    int timeout;             // 进程心跳的超时时间。
    char pname[51];          // 进程名，建议用"ftpgetfiles_后缀"的方式。
} starg;

struct st_fileinfo {
    char filename[301];    //文件名
    char mtime[21];        //文件时间
};

vector<struct st_fileinfo> vlistfile_1;    // 已下载成功文件名的容器，从okfilename中加载。
vector<struct st_fileinfo> vlistfile_2;    // 下载前列出服务端文件名的容器，从nlist文件中加载。
vector<struct st_fileinfo> vlistfile_3;    // 本次不需要下载的文件的容器。
vector<struct st_fileinfo> vlistfile_4;    // 本次需要下载的文件的容器。

//1.加载okfilensme文件中的内容到容器vlistfile_1
bool LoadOkFile();

//2.比较vlistfile_2和vlistfile_1，等到vlistfile_3和vlistfile_4
bool CompVector();

//3.把容器vlistfile_3中的内容写入到okfilename文件覆盖之前的旧文件okfilename
bool WriteToOkFile();

//如果ptype ==1 ,把下载成功的文件记录最加到okfilename中
bool AppendToOkFile(struct st_fileinfo *stFileinfo);


//把ftp.nlist()方法获取到的list文件加载到容器vfilelist中
bool LoadListFile();

//程序推出信号2、15的处理函数
void EXIT(int sig);

//帮助函数
void _help();

//解析xml到结构体starg中
bool _xmltoarg(char *strxmlbuffer);

// 下载文件功能的主函数。
bool _ftpgetfiles();

//进程心跳
CPActive PActive;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        _help();
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //打开日志文件
    if (!logfile.Open(argv[1])) {
        printf("[ftpgetfiles->main] :打开日志文件[%s]失败！\n", argv[1]);
        return -1;
    }
    //解析xml，得到运行参数
    if (!_xmltoarg(argv[2])) { return -1; }

    //将进程心跳信息写入共享内存
    PActive.AddPInfo(starg.timeout,starg.pname);

    //登录ftp
    if (!ftp.login(starg.host, starg.username, starg.password, starg.mode)) {
        logfile.Write("[ftpgetfiles->main] ftp.login(%s,%s,%s) 失败。\n", starg.host, starg.username, starg.password);
        return -1;
    }
    logfile.Write("[ftpgetfiles->main] ftp.login 成功。 \n");

    //进入ftp文件目录
    _ftpgetfiles();


}

// 下载文件功能的主函数。
bool _ftpgetfiles() {
    //进入ftp文件目录
    if (!ftp.chdir(starg.remotepath)) {
        logfile.Write("[ftpgetfiles->_ftpgetfiles] ftp.chdir(%s) 失败。\n", starg.remotepath);
        return false;
    }
    //调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件
    if (!ftp.nlist(".", starg.listfilename)) {
        logfile.Write("[ftpgetfiles->_ftpgetfiles] ftp.nlist(%s) 失败。\n", starg.listfilename);
        return false;
    }

    PActive.UptATime();  //更新进程心跳

    //把ftp.nlist()方法获取到的list文件加载到容器vlistfile_2中
    if (!LoadListFile()) {
        logfile.Write("[ftpgetfiles->_ftpgetfiles] LoadListFile 失败 。\n");
        return false;
    }

    PActive.UptATime();  //更新进程心跳

    //处理容器数据
    if (starg.ptype == 1) {
        //1.加载okfilensme文件中的内容到容器vlistfile_1
        LoadOkFile();
        //2.比较vlistfile_2和vlistfile_1，等到vlistfile_3和vlistfile_4
        CompVector();
        //3.把容器vlistfile_3中的内容写入到okfilename文件覆盖之前的旧文件okfilename
        WriteToOkFile();
        //4.把vlistfile_4中的内容复制到vlistfile_2中
        vlistfile_2.clear();
        vlistfile_2.swap(vlistfile_4);
    }

    PActive.UptATime();  //更新进程心跳

    //遍历容器vlistfile_2  //调用ftp.get()从服务器下载文件
    char strremotefilename[301];  //服务器文件名
    char strlocalfilename[301];  //本地文件名
    for (int i = 0; i < vlistfile_2.size(); ++i) {
        SNPRINTF(strremotefilename, sizeof(strremotefilename), 300, "%s/%s", starg.remotepath, vlistfile_2[i].filename);
        SNPRINTF(strlocalfilename, sizeof(strlocalfilename), 300, "%s/%s", starg.localpath, vlistfile_2[i].filename);
        logfile.Write("[ftpgetfiles->_ftpgetfiles] get file [%s] ...\n", vlistfile_2[i].filename);
        if (!ftp.get(strremotefilename, strlocalfilename)) {
            logfile.Write("[ftpgetfiles->_ftpgetfiles] get file [%s] 失败\n", vlistfile_2[i].filename);
            return false;
        }
        logfile.Write("[ftpgetfiles->_ftpgetfiles] get file [%s] 成功\n", vlistfile_2[i].filename);

        PActive.UptATime();  //更新进程心跳

        //如果ptype ==1 ,把下载成功的文件记录最加到okfilename中
        if (starg.ptype == 1) {
            AppendToOkFile(&vlistfile_2[i]);
        }

        //删除文件
        if (starg.ptype == 2) {
            if (!ftp.ftpdelete(strremotefilename)) {
                logfile.Write("[ftpgetfiles->_ftpgetfiles] ftp.ftpdelete(%s) 失败 ---- 可能的原因:权限不足或文件不存在\n",
                              strremotefilename);
                return false;
            }
        }
        //转存到备份目录
        if (starg.ptype == 3) {
            char strremotefilenamebak[301];  //备份文件名
            SNPRINTF(strremotefilenamebak, sizeof(strremotefilenamebak), 300, "%s/%s", starg.remotepathbak,
                     vlistfile_2[i].filename);
            if (!ftp.ftprename(strremotefilename, strremotefilenamebak)) {
                logfile.Write("[ftpgetfiles->_ftpgetfiles] ftp.ftprename(%s,%s) 失败 ---- 可能的原因:权限不足或备份目录不存在（请手动创建）\n",
                              strremotefilename,
                              strremotefilenamebak);
                return false;
            }
        }
    }
    return true;
}

//1.加载okfilensme文件中的内容到容器vlistfile_1
bool LoadOkFile() {
    CFile File;
    vlistfile_1.clear();
    //注意，如果程序第一次下载，okfileanme是不存在的，打开文件失败也返回true
    if (!File.Open(starg.okfilename, "r")) { return true; }
    struct st_fileinfo stfileinfo;
    char strbuffer[501];
    while (true) {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

        if (!File.Fgets(stfileinfo.filename, 300, true)) { break; }

        GetXMLBuffer(strbuffer, "filename", stfileinfo.filename);
        GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime);

        vlistfile_1.push_back(stfileinfo);
    }
    return true;
}

//2.比较vlistfile_2和vlistfile_1，等到vlistfile_3和vlistfile_4
bool CompVector() {
    vlistfile_3.clear();
    vlistfile_4.clear();

    //遍历vlistfile_2
    int i, j;
    for (i = 0; i < vlistfile_2.size(); ++i) {
        //在vlistfile_1中查找vlistfile_2[i]的文件
        for (j = 0; j < vlistfile_1.size(); ++j) {
            //找到了放入vlistfile_3
            if ((strcmp(vlistfile_2[i].filename, vlistfile_1[j].filename) == 0) &&
                (strcmp(vlistfile_2[i].mtime, vlistfile_1[j].mtime) == 0)) {
                vlistfile_3.push_back(vlistfile_2[i]);
                break;
            }
        }
        //没找到放入vlistfile_4
        if (j == vlistfile_1.size()) { vlistfile_4.push_back(vlistfile_2[i]); }
    }

    return true;
}

//3.把容器vlistfile_3中的内容写入到okfilename文件覆盖之前的旧文件okfilename
bool WriteToOkFile() {
    CFile File;
    if (!File.Open(starg.okfilename, "w")) {
        logfile.Write("[ftpgetfiles->WriteToOkFile] File.Open(%s)\n", starg.okfilename);
        return false;
    }
    for (auto &i: vlistfile_3) {
        File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", i.filename, i.mtime);
    }
    return true;
}

//如果ptype ==1 ,把下载成功的文件记录最加到okfilename中
bool AppendToOkFile(struct st_fileinfo *stFileinfo) {
    CFile File;
    if (!File.Open(starg.okfilename, "a")) {
        logfile.Write("[ftpgetfiles->WriteToOkFile] File.Open(%s)\n", starg.okfilename);
        return false;
    }
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stFileinfo->filename, stFileinfo->mtime);
    return true;
}

//把ftp.nlist()方法获取到的list文件加载到容器vlistfile_2中
bool LoadListFile() {
    CFile File;
    vlistfile_2.clear();
    if (!File.Open(starg.listfilename, "r")) {
        logfile.Write("[ftpgetfiles->file.open](%s) 失败！\n", starg.listfilename);
        return false;
    }
    struct st_fileinfo stfileinfo;
    while (true) {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

        if (!File.Fgets(stfileinfo.filename, 300, true)) { break; }

        if (!MatchStr(stfileinfo.filename, starg.matchname)) { continue; }

        if ((starg.ptype == 1) && starg.checkmtime) {
            //获取ftp服务器的文件时间
            if (!ftp.mtime(stfileinfo.filename)) {
                logfile.Write("[ftpgetfiles->file.mtime](%s) 失败！\n", stfileinfo.filename);
                return false;
            }
            strcpy(stfileinfo.mtime, ftp.m_mtime);
        }
        vlistfile_2.push_back(stfileinfo);
    }
    //for (auto &i: vlistfile_2) { logfile.Write("filename[%s]\n", i.filename); }
    return true;
}

//解析xml到结构体starg中
bool _xmltoarg(char *strxmlbuffer) {
    memset(&starg, 0, sizeof(struct st_arg));
    GetXMLBuffer(strxmlbuffer, "host", starg.host, 30); // 远程服务端的IP和端口。
    if (strlen(starg.host) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] host is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "mode", &starg.mode);  // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
    if (starg.mode != 2) { starg.mode = 1; }

    GetXMLBuffer(strxmlbuffer, "username", starg.username, 30);  // 远程服务端ftp的用户名。
    if (strlen(starg.username) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] username is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "password", starg.password, 30); // 远程服务端ftp的密码。
    if (strlen(starg.password) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] password is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "remotepath", starg.remotepath, 300); // 远程服务端存放文件的目录。
    if (strlen(starg.remotepath) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] remotepath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "localpath", starg.localpath, 300);  // 本地文件存放的目录。
    if (strlen(starg.localpath) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] localpath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname, 100);  // 待下载文件匹配的规则。
    if (strlen(starg.matchname) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] matchname is null.\n");
        return false;
    }
    GetXMLBuffer(strxmlbuffer, "listfilename", starg.listfilename, 100);  // 待下载文件匹配的规则。
    if (strlen(starg.listfilename) == 0) {
        logfile.Write("[ftpgetfiles->_xmltoarg] listfilename is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);// 下载后服务端文件的处理方式：1-什么也不做；2-删除；3-备份。
    if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3)) {
        logfile.Write("[ftpgetfiles->_xmltoarg] ptype is error.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak, 300); // 下载后服务端文件的备份目录。
    if ((starg.ptype == 3) && (strlen(starg.remotepathbak) == 0)) {
        logfile.Write("[ftpgetfiles->_xmltoarg] remotepathbak is null.\n");
        return false;
    }
    GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename, 300); // 已下载成功文件名清单。
    if ((starg.ptype == 1) && (strlen(starg.okfilename) == 0)) {
        logfile.Write("[ftpgetfiles->_xmltoarg] okfilename is null.\n");
        return false;
    }
    // 是否需要检查服务端文件的时间，true-需要，false-不需要，此参数只有当ptype=1时才有效，缺省为false。
    GetXMLBuffer(strxmlbuffer, "checkmtime", &starg.checkmtime);

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

    return true;
}


//程序推出信号2、15的处理函数
void EXIT(int sig) {
    printf("程序退出，signal=[%d]\n\n", sig);
    exit(0);
}

//帮助函数
void _help() {
    printf("\n----------------------------------------------------------------------------------------\n");
    printf("[Using ] :./ftpgetfiles logfilename xmlbuffer\n");

    printf("----------------------------------------------------------------------------------------\n");

    printf("[Sample] :./procctl 30 ./ftpgetfiles /project/log/idc/ftpgetfiles_surfdata.log \"<host>192.168.18.133:21</host>"
           "<mode>1</mode><username>root</username><password>000000</password><localpath>/idcdata/surfdata"
           "</localpath><remotepath>/project/tempdata/idc/surfdata</remotepath><matchname>CODEROVE_ZH*.XML,CODEROVE_ZH*.CSV</matchname>"
           "<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><remotepathbak>/project/tempdata/idc/surfdatabak</remotepathbak>"
           "<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n");

    printf("----------------------------------------------------------------------------------------\n");
    printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
    printf("logfilename是本程序运行的日志文件   xmlbuffer为文件下载的参数\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("xmlbuffer参数说明:\n\n");
    printf("<host>127.0.0.1:21</host>                                                :远程服务器的IP和端口。\n");
    printf("<mode>1</mode>                                                           :传输模式,1-被动模式,2-主动模式,缺省采用被动模式。\n");
    printf("<username>root</username>                                                :远程服务器ftp的用户名。\n");
    printf("<password>000000</password>                                              :远程服务器ftp的密码。\n");
    printf("<remotepath>/tempdata/idc/surfdata</remotepath>                          :远程服务器存放文件的目录。\n");
    printf("<localpath>/idcdata/surfdata</localpath>                                 :本地文件存放的目录。\n");
    printf("<matchname>CODEROVE_ZH*.XML,CODEROVE_ZH*.CSV</matchname>                 :待下载文件匹配的规则。不匹配的文件不会被下载,本字段尽可能设置精确,不建议用*匹配全部的文件。\n");
    printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename>  :下载前列出服务端文件名的文件。\n");
    printf("<ptype>1</ptype>                                                         :文件下载成功后,远程服务端文件的处理方式: 1-nothing；2-delete；3-copy，如果为3，还要指定备份的目录。\n");
    printf("<remotepathbak>/project/tempdata/idc/surfdatabak</remotepathbak>         :文件下载成功后,服务端文件的备份目录,此参数只有当ptype=3时才有效。\n");
    printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename>       :已下载成功文件名清单,此参数只有当ptype=1时才有效。\n");
    printf("<checkmtime>true</checkmtime>                                            :是否需要检查服务端文件的时间,true-需要,false-不需要，此参数只有当ptype=1时才有效,缺省为false。\n");
    printf("<timeout>80</timeout>                                                    :下载文件超时时间,单位:秒,视文件大小和网络带宽而定。\n");
    printf("<pname>ftpgetfiles_surfdata</pname>                                      :进程名,尽可能采用易懂的、与其它进程不同的名称,方便故障排查。\n");
    printf("----------------------------------------------------------------------------------------\n\n");
}

#endif //PROJECT_FTPGETFILES_H

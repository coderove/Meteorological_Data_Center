/**
* @file gzipfiles.cpp
* @brief
* @author coderove
* @date 2022-7-13
* @copyright Copyright (c) 2022年 coderove All rights reserved.
*/
#ifndef _CODE_GZIPFILES
#define _CODE_GZIPFILES

#include "_public.h"

//程序推出信号2、15的处理函数
void EXIT(int sig);

int main(int argc, char *argv[]) {
    //程序的说明
    if (argc != 4) {
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("[Using  ] :   ./gzipfiles pathname matchstr timeout\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("[Example] :(1)./gzipfiles /project/log/idc \"*.log.20*\" 0.02\n");
        printf("           (2)./gzipfiles /project/tempdata/idc/surfdata \"*.xml,*.json\" 0.01\n");
        printf("           (3)./procctl 300 ./gzipfiles /project/log/idc \"*.log.20*\" 0.02\n");
        printf("           (4)./procctl 300 ./gzipfiles /project/tempdata/idc/surfdata \"*.xml,*.json\" 0.01\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("参数说明:\n\n");
        printf("pathname  :需要压缩文件的目录及子目录。\n");
        printf("matchstr  :文件匹配。\n");
        printf("timeout   :历史时间点。\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("这是一个工具程序,用于压缩历史的数据文件或日志文件。\n");
        printf("本程序把pathname目录及子目录中timeout天之前的匹配matchstr文件全部压缩,timeout可以是小数。\n");
        printf("本程序不写日志文件,也不会在控制台输出任何信息。\n");
        printf("本程序调用/usr/bin/gzip命令压缩文件。\n");
        printf("----------------------------------------------------------------------------------------\n\n");

        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    CloseIOAndSignal(true);
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    //获取文件超时的时间点
    char strTimeOut[21];
    LocalTime(strTimeOut, "yyyy-mm-dd hh24:mi:ss", 0 - (int) (atof(argv[3]) * 24 * 60 * 60));

    CDir Dir;
    // 打开目录，CDir.OpenDir()
    if (!Dir.OpenDir(argv[1], argv[2], 10000, true)) {
        printf("[gzipfiles]:打开文件(%s)失败\n", argv[1]);
        return -1;
    }
    char strCmd[1024]; // 存放gzip压缩文件的命令。

    //遍历目录中的文件名
    while (true) {
        // 得到一个文件的信息，CDir.ReadDir()
        if (!Dir.ReadDir()) break;
        // 与超时的时间点比较，如果更早，就需要压缩  若已经压缩就忽略
        if ((strcmp(Dir.m_ModifyTime, strTimeOut) < 0) && !MatchStr(Dir.m_FileName, "*.gz")) {
            // 压缩文件，调用操作系统的gzip命令。
            SNPRINTF(strCmd, sizeof(strCmd), 1000, "/usr/bin/gzip -f %s 1>/dev/null 2>/dev/null", Dir.m_FullFileName);
            if (system(strCmd) == 0) {
                printf("gzip [%s] 成功.\n", Dir.m_FullFileName);
            } else {
                printf("gzip [%s] 失败.\n", Dir.m_FullFileName);
            }
        }
        return 0;
    }
}

//程序推出信号2、15的处理函数
void EXIT(int sig) {
    printf("程序退出，signal=[%d]\n\n", sig);
    exit(0);
}

#endif
/**
* @file procctl.cpp
* @brief 服务调度程序
* @author coderove
* @date 2022-7-12
* @copyright Copyright (c) 2022年 coderove All rights reserved.
*/


#ifndef _CODE_PROCCTL
#define _CODE_PROCCTL

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("[Using  ] :./procctl timetvl program argv ...\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("[Example] :./procctl 60  ./crtsurfdata  /project/idc/ini/stcode.ini /project/tempdata/idc/surfdata  /project/log/idc/crtsurfdata.log csv,xml,json\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("参数说明:\n\n");
        printf("timetvl   :运行周期,单位:秒。被调度的程序运行结束后,在timetvl秒后会被procctl重新启动。\n");
        printf("program   :被调度的程序名,必须使用全路径。\n");
        printf("argvs     :被调度的程序的参数。\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("本程序是服务程序的调度程序,周期性启动服务程序或shell脚本。\n");
        printf("注意△:本程序不会被kill杀死,但可以用kill -9强行杀死。\n");
        printf("----------------------------------------------------------------------------------------\n\n");
        return -1;
    }

    // 关闭信号和IO，本程序不希望被打扰。
    for (int i = 0; i < 64; i++) {
        signal(i, SIG_IGN);
        close(i);
    }

    // 生成子进程，父进程退出，让程序运行在后台，由系统1号进程托管。
    if (fork() != 0) exit(0);

    // 启用SIGCHLD信号，让父进程可以wait子进程退出的状态。
    signal(SIGCHLD, SIG_DFL);

    char *pargv[argc];
    for (int i = 2; i < argc; i++) { pargv[i - 2] = argv[i]; }

    pargv[argc - 2] = nullptr;

    while (true) {
        if (fork() == 0) {
            execv(argv[2], pargv);
            exit(0);  //失败时退出，防止无限制fork，系统死机
        } else {
            int status;
            wait(&status);
            sleep(atoi(argv[1]));
        }
    }
}

#endif
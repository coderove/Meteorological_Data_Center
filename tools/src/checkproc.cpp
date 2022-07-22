/**
 * @file checkproc.cpp
 * @brief 守护程序
 * @author coderove
 * @date 2022-7-13
 * @copyright Copyright (c) 2022年 coderove All rights reserved.
 */

#ifndef _CODE_CHECKOROC
#define _CODE_CHECKOROC

#include "_public.h"

CLogFile logfile; //日志

int main(int argc, char *argv[]) {
    //程序说明
    if (argc != 2) {
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("[Using  ]  :./checkproc logfilename\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("[Example]  :./project/bin/procctl 10 ./bin/checkproc /project/log/proc/checkproc.log\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("参数说明:\n\n");
        printf("logfilename: 日志文件名\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("本程序用于检查后台服务程序是否超时，如果已超时，立即终止。\n");
        printf("注意:\n");
        printf("              1.本程序由procctl启动，运行周期建议为10s。\n");
        printf("              2.为了避免被普通用户误杀，本程序应由root用户启动 \n");
        printf("              3.本程序只能用 kill - 9 强制终止\n");
        printf("----------------------------------------------------------------------------------------\n\n");

        return 0;
    }
    //忽略全部信号，使程序不被干扰
    CloseIOAndSignal(true);
    //打开日志
    if (!logfile.Open(argv[3])) {
        printf("[checkproc]:打开日志文件[%s]失败！\n", argv[3]);
        return -1;
    }
    //创建/获取共享内存
    int shmid = 0;
    //键值：SHMKEY，大小:MAXNUMP
    if ((shmid = shmget((key_t) SHMKEYP, MAXNUMP * sizeof(struct st_procinfo), 0666 | IPC_CREAT)) == -1) {
        logfile.Write("创建/获取共享内存(%x)失败。\n", SHMKEYP);
        return false;
    }
    //连接共享内存到当前进程的地址空间
    struct st_procinfo *shm = (struct st_procinfo *) shmat(shmid, 0, 0);
    //遍历共享内存中全部的进程心跳记录
    for (int i = 0; i < MAXNUMP; ++i) {
        //如果记录pid==0，表示空记录，continue
        if (shm[i].pid == 0) { continue; }
        //如果记录pid！=0，表示服务程序心跳记录本地
//        logfile.Write("pos=%d,pid=%d,pname=%s,timeout=%d,atime=%d\n", i, shm[i].pid, shm[i].pname, shm[i].timeout,
//                      shm[i].atime);
        //向进程发送信号0，判断是否还处在，如果不存在，从共享内存删除记录，continue
        int pret = kill(shm[i].pid, 0);
        if (pret == -1) {
            logfile.Write("进程pid=[%d](%s)已经不存在。\n", shm[i].pid, shm[i].pname);
            memset(shm + i, 0, sizeof(struct st_procinfo));//从共享内存删除记录，
            continue;
        }
        time_t now = time(0); //当前时间
        //如果进程未超时，continue
        if (now - shm[i].atime < shm[i].timeout) { continue; }
        //如果已经超时
        logfile.Write("进程pid=[%d](%s)已超时。\n", shm[i].pid, shm[i].pname);
        //发送信号15，尝试正常终止进程。
        kill(shm[i].pid, 15);
        //间隔1s判断是否存在，一共5s
        for (int j = 0; j < 5; ++j) {
            sleep(1);
            pret = kill(shm[i].pid, 0); //向进程发送信号0，判断是否还处在
            if (pret == -1) { break; }
        }
        //如果进程仍处在，就发送信号9，强制终止。
        if (pret == -1) {
            logfile.Write("进程pid=[%d](%s)已正常终止。\n", shm[i].pid, shm[i].pname);
        } else {
            kill(shm[i].pid, 9);//强制终止。
            logfile.Write("进程pid=[%d](%s)已强制终止。\n", shm[i].pid, shm[i].pname);
        }
        //从共享内存中删除已超时的进程心跳记录。
        memset(shm + i, 0, sizeof(st_procinfo));
    }
    //把共享内存从当前进程中分离
    shmdt(shm);
    return 0;
}
#endif
/**
 * @file crtsurfdata.cpp
 * @author coderove
 * @brief 生成全国气象站点观测的分钟数据
 * @version 0.1
 * @date 2022-07-12
 *
 * @copyright Copyright (src) 2022
 *
 */
#ifndef _CODE_CRTSURFDATA
#define _CODE_CRTSURFDATA

#include "_public.h"


CFile File;       //文件操作
CLogFile logfile; //日志
CPActive PActive;//进程心跳
char strddatetime[21]; //获取当前时间作为观测时间
/* 省   站号  站名 纬度   经度  海拔高度
 * 安徽,58015,砀山,34.27,116.2,44.2
 * */
struct st_stcode {
    char prov_name[31];  //省
    char stand_id[11];   //观测站id
    char stand_name[31]; //观测站名
    double lat;          //纬度
    double lon;          //经度
    double height;       //海拔
};
//全国气象站点分钟观测数据结构
struct st_surfdata {
    char stand_id[11];      //观测站id
    char ddatetime[21];     //数据时间 格式：yyyymmddhh24miss
    int t;                    //气温：单位，0.1摄氏度
    int p;                    //气压：0.1百帕
    int u;                    //相对湿度，0-100
    int wd;                    //风向，0-360
    int wf;                    //风速：单位0.1m/s
    int r;                    //降雨量：0.1mm
    int vis;                //能见度：0.1m
};

//存放全国气象数据的容器
vector<struct st_stcode> vstcode;

//存放全国气象站点分钟观测数据的容器
vector<struct st_surfdata> vsurfdata;

//数据存放到站点观测数据容器中
bool LoadSTcode(const char *inifile);

//模拟生成全国气象站点每分钟观测数据，存放在vsurfdata
void createSurfData();

//把容器vsurfdata的全国气象站点的分钟观测数据写入wenjain
bool CreatSurfFile(const char *outpath, const char *datafmt);

//程序推出信号2、15的处理函数
void EXIT(int sig);

//帮助函数
void _help();

int main(int argc, char *argv[]) {
    // 1. 参数文件 inifile  2.  测试生成路径 outpath 3.日志 logfile
    if ((argc != 5) && (argc != 6)) {
        _help();
        return -1;
    }

    //忽略全部信号，使程序不被干扰 kill+ 进程号可正常终止，当请不要 kill -9 强制终止
    CloseIOAndSignal(true);
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    if (!logfile.Open(argv[3])) {
        printf("[main]:打开日志文件[%s]失败！\n", argv[3]);
        return -1;
    }

    logfile.Write("程序[%s] 开始运行\n", argv[0]);

    PActive.AddPInfo(20, "surfdata"); //进程心跳
    //数据存放到站点观测数据容器中
    if (!LoadSTcode(argv[1])) {
        logfile.Write("main:加载站点观测数据[%s]失败！\n", argv[1]);
        return -1;
    }

    memset(strddatetime, 0, sizeof(strddatetime));
    if (argc == 5) {
        LocalTime(strddatetime, "yyyymmddhh24miss");
    } else {
        STRCPY(strddatetime, sizeof(strddatetime), argv[5]);
    };


    //模拟生成全国气象站点每分钟观测数据，存放在vsurfdata
    createSurfData();

    //把容器vsurfdata的全国气象站点的分钟观测数据写入wenjain
    if (strstr(argv[4], "xml") != nullptr) { CreatSurfFile(argv[2], "xml"); }
    if (strstr(argv[4], "json") != nullptr) { CreatSurfFile(argv[2], "json"); }
    if (strstr(argv[4], "csv") != nullptr) { CreatSurfFile(argv[2], "csv"); }


    logfile.Write("程序[%s] 运行结束\n", argv[0]);
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
        CmdStr.GetValue(0, stcode.prov_name, 30);   //省
        CmdStr.GetValue(1, stcode.stand_id, 10);    //站号
        CmdStr.GetValue(2, stcode.stand_name, 30);  //站名
        CmdStr.GetValue(3, &stcode.lat);                //纬度
        CmdStr.GetValue(4, &stcode.lon);                //经度
        CmdStr.GetValue(5, &stcode.height);             //海拔高度
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

//模拟生成全国气象站点每分钟观测数据，存放在vsurfdata
void createSurfData() {
    //随机数种子
    srand(time(0));

    struct st_surfdata stsurfdata;
    //遍历气象站点的vstcode容器
    for (auto it: vstcode) {
        memset(&stsurfdata, 0, sizeof(struct st_surfdata));
        //用随机种子填充分钟观测数据结构体
        strncpy(stsurfdata.stand_id, it.stand_id, 10);    //观测站id
        strncpy(stsurfdata.ddatetime, strddatetime, 14);  //数据时间 格式：yyyymmddhh24miss
        stsurfdata.t = rand() % 351;                                    //气温：单位，0.1摄氏度
        stsurfdata.p = rand() % 265 + 10000;                            //气压：0.1百帕
        stsurfdata.u = rand() % 100 + 1;                                //相对湿度，0-100
        stsurfdata.wd = rand() % 360;                                   //风向，0-360
        stsurfdata.wf = rand() % 150;                                   //风速：单位0.1m/s
        stsurfdata.r = rand() % 16;                                     //降雨量：0.1mm
        stsurfdata.vis = rand() % 5001 + 100000;                        //能见度：0.1m
        //观测数据放入vsurfdata容器
        vsurfdata.push_back(stsurfdata);
    }
}

//把容器vsurfdata的全国气象站点的分钟观测数据写入wenjain
bool CreatSurfFile(const char *outpath, const char *datafmt) {
    //拼接生成的文件名 ，例如 /tmp/surfdata/CODEROVE_ZH_202207121615_568.csv
    char strFileName[301];
    snprintf(strFileName, (size_t) 300, "%s/CODEROVE_ZH_%s_%d.%s", outpath, strddatetime, getpid(), datafmt);

    //打开文件
    if (!File.OpenForRename(strFileName, "w")) {
        logfile.Write("CreatSurfFile:文件[%s]打开失败 --权限不足/磁盘空间不足", strFileName);
        return false;
    }

    //写入第一行的标题
    if (strcmp(datafmt, "csv") == 0) { File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n"); }
    if (strcmp(datafmt, "xml") == 0) { File.Fprintf("<data>\n"); }
    if (strcmp(datafmt, "json") == 0) { File.Fprintf("{\"data\":[\n"); }
    //遍历存放观测数据的vsurfdata容器
    for (int i = 0; i < vsurfdata.size(); ++i) {
        //写入记录
        if (strcmp(datafmt, "csv") == 0) {
            File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n", vsurfdata[i].stand_id, vsurfdata[i].ddatetime,
                         vsurfdata[i].t / 10.0, vsurfdata[i].p / 10.0,
                         vsurfdata[i].u, vsurfdata[i].wd,
                         vsurfdata[i].wf / 10.0, vsurfdata[i].r / 10.0, vsurfdata[i].vis / 10.0);
        }
        if (strcmp(datafmt, "xml") == 0) {
            File.Fprintf(
                    "<stand_id>%s</stand_id><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",
                    vsurfdata[i].stand_id, vsurfdata[i].ddatetime, vsurfdata[i].t / 10.0, vsurfdata[i].p / 10.0,
                    vsurfdata[i].u, vsurfdata[i].wd,
                    vsurfdata[i].wf / 10.0, vsurfdata[i].r / 10.0, vsurfdata[i].vis / 10.0);
        }
        if (strcmp(datafmt, "json") == 0) {
            File.Fprintf(
                    "{\"stand_id\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\",\"u\":\"%d\",\"wd\":\"%d\","
                    "\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",
                    vsurfdata[i].stand_id, vsurfdata[i].ddatetime, vsurfdata[i].t / 10.0, vsurfdata[i].p / 10.0,
                    vsurfdata[i].u, vsurfdata[i].wd,
                    vsurfdata[i].wf / 10.0, vsurfdata[i].r / 10.0, vsurfdata[i].vis / 10.0);
            if (i >= vsurfdata.size() - 1) {
                File.Fprintf("\n");
            } else {
                File.Fprintf(",\n");
            }
        }

    }
    if (strcmp(datafmt, "xml") == 0) { File.Fprintf("</data>\n"); }
    if (strcmp(datafmt, "json") == 0) { File.Fprintf("]}\n"); }

    //关闭文件
    File.CloseAndRename();
    UTime(strFileName, strddatetime); //修改文件的时间属性
    logfile.Write("数据文件[%s]生成成功  记录时间[%s]  记录数量[%d] \n", strFileName, strddatetime, vsurfdata.size());
    return true;
}

//程序推出信号2、15的处理函数
void EXIT(int sig) {
    logfile.Write("程序退出，signal=[%d]\n\n", sig);
    exit(0);
}

void _help(){
    printf("'\n----------------------------------------------------------------------------------------\n");
    printf("[Using  ]   :    ./crtsurfdata inifile outpath logfile datafmt [datetime]\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[Example]   :(1) ./crtsurfdata /project/idc/ini/stcode.ini /project/tempdata/idc/surfdata  /project/log/idc/crtsurfdata.log xml,json,csv\n");
    printf("            :(2) ./crtsurfdata /project/idc/ini/stcode.ini /project/tempdata/idc/surfdata  /project/log/idc/crtsurfdata.log xml,json,csv 20220712213506\n");
    printf("            :(3) ./procctl 60  ./crtsurfdata  /project/idc/ini/stcode.ini /project/tempdata/idc/surfdata  /project/log/idc/crtsurfdata.log csv,xml,json\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("参数说明:\n\n");
    printf("crtsurfdata :可执行文件\n");
    printf("inifile     :全国气象站点参数文件。\n");
    printf("outpath     :全国气象站点数据文件存放目录。\n");
    printf("logfile     :程序运行日志。\n");
    printf("datafmt     :生成数据的格式,支持xml、json、csv三种格式,中间用逗号分割。\n");
    printf("datetime    :可选参数,生指定时间的数据和文件。\n");
    printf("----------------------------------------------------------------------------------------\n\n");
}

#endif
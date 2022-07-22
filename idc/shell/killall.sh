####################################################################
# 停止数据中心后台服务程序的脚本。
####################################################################

killall -9 procctl
killall gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles fileserver tcpputfiles tcpgetfiles standtcodetodb
killall standmindtodb execsql dminingmysql xmltodb syncupdate syncincrement deletetable migratetable

sleep 3
killall  -9 gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles fileserver tcpputfiles tcpgetfiles standtcodetodb
killall  -9 standmindtodb execsql dminingmysql xmltodb syncupdate syncincrement deletetable migratetable

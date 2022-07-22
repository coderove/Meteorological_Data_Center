# 开发框架头文件路径。
PUBINCL = -I/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
PUBCPP = /project/public/_public.cpp


# mysql头文件存放路径
MYSQLINCL = -I/usr/include/mysql  -I/project/public/db/mysql

# mysql库文件保存路径
MYSQLLIB = -L/usr/lib64/mysql

# mysql链接库
MYSQLLIBS = -lmysqlclient

# 开发框架mysql的cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
MYSQLCPP = /project/public/db/mysql/_mysql.cpp

#####################################
# oracle头文件路径
# ORAINCL = -I$(ORACLE_HOME)/rdbms/public -I/project/public/db/oracle

# oracle库文件路径
# ORALIB =  -L$(ORACLE_HOME)/lib -L.

# oracle的oci库
# ORALIBS = -lclntsh

# 开发框架oracle的cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
# ORACPP = /project/public/db/oracle/_ooci.cpp
#####################################



#可执行文件
BINARY = /project/bin/

# 编译参数。
CFLAGS = -g


all: procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles  fileserver tcpgetfiles
all: execsql dminingmysql xmltodb syncupdate syncincrement deletetable migratetable
# all:xmltodb_oracle deletetable_oracle migratetable_oracle dminingoracle syncupdate_oracle syncincrement_oracle syncincrementex_oracle

procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl $(BINARY)

checkproc:checkproc.cpp
	g++ $(CFLAGS) -o checkproc checkproc.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp checkproc $(BINARY)

gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) -o gzipfiles gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles $(BINARY)

deletefiles:deletefiles.cpp
	g++ $(CFLAGS) -o deletefiles deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles $(BINARY)


ftpgetfiles:ftpgetfiles.cpp
	g++ $(CFLAGS) -o ftpgetfiles ftpgetfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp ftpgetfiles $(BINARY)

ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAGS) -o ftpputfiles ftpputfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp ftpputfiles $(BINARY)

tcpputfiles:tcpputfiles.cpp
	g++ $(CFLAGS) -o tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP)  -lm -lc
	cp tcpputfiles $(BINARY)


fileserver:fileserver.cpp
	g++ $(CFLAGS) -o fileserver fileserver.cpp $(PUBINCL) $(PUBCPP)  -lm -lc
	cp fileserver $(BINARY)

tcpgetfiles:tcpgetfiles.cpp
	g++ $(CFLAGS) -o tcpgetfiles tcpgetfiles.cpp $(PUBINCL) $(PUBCPP)  -lm -lc
	cp tcpgetfiles $(BINARY)

execsql:execsql.cpp
	g++ $(CFLAGS) -o execsql execsql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp execsql $(BINARY)

dminingmysql:dminingmysql.cpp
	g++ $(CFLAGS) -o dminingmysql dminingmysql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp dminingmysql $(BINARY)

xmltodb:xmltodb.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o xmltodb xmltodb.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp xmltodb  $(BINARY)


syncupdate:syncupdate.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o syncupdate syncupdate.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncupdate  $(BINARY)

syncincrement:syncincrement.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o syncincrement syncincrement.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncincrement $(BINARY)

deletetable:deletetable.cpp
	g++ $(CFLAGS) -o deletetable deletetable.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp deletetable $(BINARY)

migratetable:migratetable.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o migratetable migratetable.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp migratetable $(BINARY)

######################### ORACLE数据库操作程序编译 #########################
#xmltodb_oracle:xmltodb_oracle.cpp _tools_oracle.h _tools_oracle.cpp
#	g++ $(CFLAGS) -o xmltodb_oracle xmltodb_oracle.cpp _tools_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp xmltodb_oracle $(BINARY)
#
#deletetable_oracle:deletetable_oracle.cpp
#	g++ $(CFLAGS) -o deletetable_oracle deletetable_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp deletetable_oracle $(BINARY)
#
#dminingoracle:dminingoracle.cpp
#	g++ $(CFLAGS) -o dminingoracle dminingoracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp dminingoracle $(BINARY)
#
#migratetable_oracle:migratetable_oracle.cpp _tools_oracle.h _tools_oracle.cpp
#	g++ $(CFLAGS) -o migratetable_oracle migratetable_oracle.cpp _tools_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp migratetable_oracle $(BINARY)
#
#syncupdate_oracle:syncupdate_oracle.cpp _tools_oracle.h _tools_oracle.cpp
#	g++ $(CFLAGS) -o syncupdate_oracle syncupdate_oracle.cpp _tools_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp syncupdate_oracle $(BINARY)
#
#syncincrement_oracle:syncincrement_oracle.cpp _tools_oracle.h _tools_oracle.cpp
#	g++ $(CFLAGS) -o syncincrement_oracle syncincrement_oracle.cpp _tools_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp syncincrement_oracle $(BINARY)
#
#syncincrementex_oracle:syncincrementex_oracle.cpp _tools_oracle.h _tools_oracle.cpp
#	g++ $(CFLAGS) -o syncincrementex_oracle syncincrementex_oracle.cpp _tools_oracle.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) -lm -lc
#	cp syncincrementex_oracle $(BINARY)

clean:
	rm -f procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles  fileserver tcpgetfiles
	rm -f execsql dminingmysql xmltodb syncupdate syncincrement deletetable migratetable
	# rm -f xmltodb_oracle deletetable_oracle migratetable_oracle dminingoracle syncupdate_oracle syncincrement_oracle syncincrementex_oracle

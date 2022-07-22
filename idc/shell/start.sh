####################################################################
 # 启动数据中心后台服务程序的脚本
####################################################################

# 检查服务程序是否超时，配置在/etc/rc.local中由root用户执行。
/project/bin/./procctl 30 /project/bin/./checkproc

# 压缩数据中心后台服务程序的备份日志。
/project/bin/./procctl 300 /project/bin/./gzipfiles /project/log/idc "*.log.20*" 0.02

# 生成用于测试的全国气象站点观测的分钟数据。
/project/bin/./procctl 60 /project/bin/./crtsurfdata /project/idc/ini/stcode.ini /project/tempdata/idc/surfdata  /project/log/idc/crtsurfdata.log csv,xml,json

# 清理原始的全国气象站点观测的分钟数据目录/tempdata/idc/surfdata中的历史数据文件。
/project/bin/./procctl 300 /project/bin/./deletefiles /project/tempdata/idc/surfdata "*" 0.02

###################################################################################

#下载全国气象站点观测的分钟数据的xml文件
/project/bin/./procctl 30 /project/bin/./ftpgetfiles /project/log/idc/ftpgetfiles_surfdata.log "
<host>127.0.0.1:21</host><mode>1</mode><username>root</username><password>000000</password>
<localpath>/idcdata/surfdata</localpath><remotepath>/project/tempdata/idc/surfdata</remotepath>
<matchname>CODEROVE_ZH*.XML,CODEROVE_ZH*.CSV</matchname>
<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype>
<remotepathbak>/project/tempdata/idc/surfdatabak</remotepathbak>
<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime>
<timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"

#上传全国气象站点观测的分钟数据的xml文件
/project/bin/./procctl 30 /project/bin/./ftpputfiles /project/log/idc/ftpputfiles_surfdata.log "
<host>127.0.0.1:21</host><mode>1</mode><username>root</username><password>000000</password>
<localpath>/project/tempdata/idc/surfdata</localpath><remotepath>/project/tempdata/ftpputtests/</remotepath>
<matchname>CODEROVE_ZH*.JSON</matchname><ptype>1</ptype>
<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout>
<pname>ftpputfiles_surfdata</pname>"


# 清理全国气象站点观测的分钟数据目录/tempdata/idc/surfdata中的历史数据文件。
/project/bin/./procctl 300 /project/bin/./deletefiles /idcdata/surfdata "*" 0.02
# 清理全国气象站点观测的分钟数据目录/tempdata/idc/surfdata中的历史数据文件。
/project/bin/./procctl 300 /project/bin/./deletefiles /project/tempdata/ftpputtests "*" 0.02

###################################################################################
# 文件传输的服务器程序
 /project/bin/./procctl 10  /project/bin/./fileserver 5005 /project/log/idc/fileserver.log

#把目录/project/tempdata/ftpputtests/的文件上传到/project/tempdata/tcpputtests/
 /project/bin/./procctl 20  /project/bin/./tcpputfiles /project/log/idc/tcpputfiles_surfdata.log "
 <ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><clientpath>/project/tempdata/ftpputtests/</clientpath>
 <andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><srvpath>/project/tempdata/tcpputtests/</srvpath>
 <timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>"

#把目录/project/tempdata/tcpputtests/的文件下载到/project/tempdata/tcpgettests/
 /project/bin/./procctl 20 /project/bin/./tcpgetfiles /project/log/idc/tcpgetfiles_surfdata.log "
 <ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><srvpath>/project/tempdata/tcpputtests/</srvpath>
 <andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname>
 <clientpath>/project/tempdata/tcpgettests/</clientpath><timetvl>10</timetvl><timeout>50</timeout>
 <pname>tcpgetfiles_surfdata</pname>"

# 清理全国气象站点观测的分钟数据目录/project/tempdata/tcpgettests/中的历史数据文件。
/project/bin/./procctl 300 /project/bin/./deletefiles /project/tempdata/ftpputtests "*" 0.02

# 把全国站点参数数据保存到数据库表中，如果站点不存在则插入，站点已存在则更新。
/project/bin/./procctl 120 /project/bin/./standtcodetodb /project/idc/ini/stcode.ini "127.0.0.1,root,123456,mdcsql,3306" utf8 /project/log/idc/obtcodetodb.log

#把全国站点分钟观测数据保存到数据库CODE_MDCMIND表中，数据只插入，不更新
/project/bin/./procctl 120 /project/bin/./standmindtodb /idcdata/surfdata "127.0.0.1,root,123456,mdcsql,3306" utf8 /project/log/idc/standmindtodb.log

#清理CODE_MDCMIND表中120分之前的数据，保证有效的磁盘空间
/project/bin/./procctl 120 /project/bin/./execsql /project/idc/sql/cleardata.sql "127.0.0.1,root,123456,mdcsql,3306" utf8 /project/log/idc/execsql.log

#################################
#每隔1h把CODE_MDCCODE中全部的数据抽取出来
/project/bin/./procctl 3600 /project/bin/./dminingmysql /project/log/idc/dminingmysql_CODE_MDCTCODE.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><charset>utf8</charset><selectsql>select stand_id,stand_name,prov_name,lat,lon,height from CODE_MDCTCODE</selectsql><fieldstr>stand_id,stand_name,prov_name,lat,lon,height</fieldstr><fieldlen>10,30,30,10,10,10</fieldlen><bfilename>MDCTCODE</bfilename><efilename>MDCCZ</efilename><outpath>/idcdata/dmindata</outpath><timeout>30</timeout><pname>dminingmysql_CODE_MDCTCODE</pname>"
#每30秒从CODE_MDCMIND表中增量抽取
/project/bin/./procctl  30  /project/bin/./dminingmysql /project/log/idc/dminingmysql_CODE_MDCMIND.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><charset>utf8</charset><selectsql>select stand_id,date_format(ddatetime,'%%Y-%%m-%%d %%H:%%i:%%s'),t,p,u,wd,wf,r,vis,keyid from CODE_MDCMIND where keyid>:1 and ddatetime>timestampadd(minute,-120,now())</selectsql><fieldstr>stand_id,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>10,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>MDCMIND</bfilename><efilename>MDCCZ</efilename><outpath>/idcdata/dmindata</outpath><starttime></starttime><incfield>keyid</incfield><timeout>30</timeout><pname>dminingmysql_MDCMIND_MDCMIND</pname><maxcount>1000</maxcount><connstr1>127.0.0.1,root,123456,mdcsql,3306</connstr1>"
#清理/idcdata/dmindata历史数据
/project/bin/./procctl 300 /project/bin/./deletefiles /idcdata/dmindata "*" 0.02
#################################
#把目录/idcdata/dmindata的文件上传到/idcdata/xmltodb/vip1 交由xmltodb入库
/project/bin/./procctl 20  /project/bin/./tcpputfiles /project/log/idc/tcpputfiles_dmindata.log "<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><clientpath>/idcdata/dmindata/</clientpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><srvpath>/idcdata/xmltodb/vip1/</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>"

#把/idcdata/xmltodb/vip1目录xml文件入库
/project/bin/./procctl 10 /project/bin/./xmltodb /project/log/idc/xmltodb_vip1.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><charset>utf8</charset><inifilename>/project/idc/ini/xmltodb.xml</inifilename><xmlpath>/idcdata/xmltodb/vip1</xmlpath><xmlpathbak>/idcdata/xmltodb/vip1bak</xmlpathbak><xmlpatherr>/idcdata/xmltodb/vip1err</xmlpatherr><timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_vip1</pname>"
#清理/idcdata/xmltodb/vip1bak历史数据
/project/bin/./procctl 300 /project/bin/./deletefiles /idcdata/xmltodb/vip1bak "*" 0.02
#清理/idcdata/xmltodb/vip1err历史数据
/project/bin/./procctl 300 /project/bin/./deletefiles /idcdata/xmltodb/vip1err "*" 0.02

#############################
# 采用全表刷新同步的方法，把表CODE_MDCTCODE1中的数据同步到表CODE_MDCTCODE2
/project/bin/./procctl 10 /project/bin/./syncupdate /project/log/idc/syncupdate_MDCTCODE2.log "<localconnstr>127.0.0.1,root,123456,mdcsql,3306</localconnstr><charset>utf8</charset><fedtname>LK_MDCTCODE1</fedtname><localtname>CODE_MDCTCODE2</localtname><remotecols>stand_id,stand_name,prov_name,lat,lon,height/10,upttime,keyid</remotecols><localcols>stand_id,stand_name,prov_name,lat,lon,height,upttime,keyid</localcols><synctype>1</synctype><timeout>50</timeout><pname>syncupdate_MDCTCODE2</pname>"

# 采用分批同步的方法，把表CODE_MDCTCODE1中stand_id like '54%'的记录同步到表CODE_MDCTCODE3
/project/bin/./procctl 10 /project/bin/./syncupdate /project/log/idc/syncupdate_MDCTCODE3.log "<localconnstr>127.0.0.1,root,123456,mdcsql,3306</localconnstr><charset>utf8</charset><fedtname>LK_MDCTCODE1</fedtname><localtname>CODE_MDCTCODE3</localtname><remotecols>stand_id,stand_name,prov_name,lat,lon,height/10,upttime,keyid</remotecols><localcols>stand_id,stand_name,prov_name,lat,lon,height,upttime,keyid</localcols><where>where stand_id like '54%%'</where><synctype>2</synctype><remoteconnstr>127.0.0.1,root,123456,mdcsql,3306</remoteconnstr><remotetname>CODE_MDCTCODE1</remotetname><remotekeycol>stand_id</remotekeycol><localkeycol>stand_name</localkeycol><maxcount>10</maxcount><timeout>50</timeout><pname>syncupdate_MDCTCODE3</pname>"


# 采用增量同步的方法，把表CODE_MDCMIND1中全部的记录同步到表CODE_MDCMIND2
/project/bin/./procctl 10 /project/bin/./syncincrement /project/log/idc/syncincrement_MDCMIND2.log "<localconnstr>127.0.0.1,root,123456,mdcsql,3306</localconnstr><remoteconnstr>127.0.0.1,root,123456,mdcsql,3306</remoteconnstr><charset>utf8</charset><remotetname>CODE_MDCMIND1</remotetname><fedtname>LK_MDCMIND1</fedtname><localtname>CODE_MDCMIND2</localtname><remotecols>stand_id,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stand_id,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</localcols><remotekeycol>keyid</remotekeycol><localkeycol>keyid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_MDCMIND2</pname>"

# 采用增量同步的方法，把表CODE_MDCMIND1中stand_id like '54%'的记录同步到表CODE_MDCMIND3
/project/bin/./procctl 10 /project/bin/./syncincrement /project/log/idc/syncincrement_MDCMIND3.log "<localconnstr>127.0.0.1,root,123456,mdcsql,3306</localconnstr><remoteconnstr>127.0.0.1,root,123456,mdcsql,3306</remoteconnstr><charset>utf8</charset><remotetname>CODE_MDCMIND1</remotetname><fedtname>LK_MDCMIND1</fedtname><localtname>CODE_MDCMIND3</localtname><remotecols>stand_id,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stand_id,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</localcols><where>and stand_id like '54%%'</where><remotekeycol>keyid</remotekeycol><localkeycol>keyid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_MDCMIND3</pname>"

###############################################
# 把CODE_MDCMIND表中120分钟之前的数据迁移到CODE_MDCMIND_HIS表。
/project/bin/./procctl  3600 /project/bin/./migratetable /project/log/idc/migratetable_MDCMIND.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><srctname>CODE_MDCMIND</srctname><dsttname>CODE_MDCMIND_HIS</dsttname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-65,now())</where><maxcount>300</maxcount><timeout>120</timeout><pname>migratetable_ZHOBTMIND</pname>"

# 清理CODE_MDCMIND1表中120分钟之前的数据。
/project/bin/./procctl  3600 /project/bin/./deletetable /project/log/idc/deletetable_MDCMIND1.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><tname>CODE_MDCMIND1</tname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-65,now())</where><timeout>120</timeout><pname>deletetable_MDCMIND1</pname>"

# 清理CODE_MDCMIND2表中120分钟之前的数据。
/project/bin/./procctl  3600 /project/bin/./deletetable /project/log/idc/deletetable_MDCMIND2.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><tname>CODE_MDCMIND2</tname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-65,now())</where><timeout>120</timeout><pname>deletetable_MDCMIND2</pname>"

# 清理CODE_MDCMIND3表中120分钟之前的数据。
/project/bin/./procctl  3600 /project/bin/./deletetable /project/log/idc/deletetable_MDCMIND3.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><tname>CODE_MDCMIND3</tname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-65,now())</where><timeout>120</timeout><pname>deletetable_MDCMIND3</pname>"

# 清理CODE_MDCMIND_HIS表中240分钟之前的数据。
/project/bin/./procctl  3600 /project/bin/./deletetable /project/log/idc/deletetable_MDCMIND_HIS.log "<connstr>127.0.0.1,root,123456,mdcsql,3306</connstr><tname>CODE_MDCMIND_HIS</tname><keycol>keyid</keycol><where>where ddatetime<timestampadd(minute,-240,now())</where><timeout>120</timeout><pname>deletetable_MDCMIND_HIS</pname>"

#####
#####oracle 数据生成脚本 未写
#####



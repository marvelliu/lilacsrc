#!/bin/sh
# $Id: build-RH9.sh 10007 2007-06-24 10:48:13Z kxn $
#
# 快速编译脚本 for Fedora Core 3/4/5
#
# 修改这里的参数定制您的站点
# BBSHOME 表示您的 BBS 所在的目录
BBSHOME=/usr/local/bbs

# BBSSITE 表示您采用的站点定制文件
BBSSITE=fb2k-v2

# WWWROOT 表示您的 WWW 所在的目录
# 注意：这个目录不是 httpd.conf 里面的 DocumentRoot 参数。
WWWROOT=/var/www

# 以下的代码请不要修改，除非您能确定自己在干什么
if [ -f Makefile ]; then
  make distclean
fi

if [ -d CVS ]; then
  cvs up -d
  ./autogen.sh
fi

if [ -d bbs2www ]; then
  WWWCONFIG="--with-www=$WWWROOT --with-php=/usr/include/php"
else
  WWWCONFIG=--disable-www
fi

if [ -d sshbbsd ]; then
  if [ -d CVS ]; then
    cd sshbbsd
    ./autogen.sh
    cd ..
  fi
  SSHCONFIG=--enable-ssh
else
  SSHCONFIG=--disable-ssh
fi

if [ -d innbbsd ]; then
  INNCONFIG=--enable-innbbsd
else
  INNCONFIG=--disable-innbbsd
fi

./configure --prefix=$BBSHOME --enable-site=$BBSSITE \
         $WWWCONFIG \
         $SSHCONFIG \
         $INNCONFIG \
         --with-mysql

make

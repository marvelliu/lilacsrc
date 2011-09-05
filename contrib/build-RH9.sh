#!/bin/sh
# $Id: build-RH9.sh 10007 2007-06-24 10:48:13Z kxn $
#
# ���ٱ���ű� for Fedora Core 3/4/5
#
# �޸�����Ĳ�����������վ��
# BBSHOME ��ʾ���� BBS ���ڵ�Ŀ¼
BBSHOME=/usr/local/bbs

# BBSSITE ��ʾ�����õ�վ�㶨���ļ�
BBSSITE=fb2k-v2

# WWWROOT ��ʾ���� WWW ���ڵ�Ŀ¼
# ע�⣺���Ŀ¼���� httpd.conf ����� DocumentRoot ������
WWWROOT=/var/www

# ���µĴ����벻Ҫ�޸ģ���������ȷ���Լ��ڸ�ʲô
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

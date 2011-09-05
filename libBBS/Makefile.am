lib_LTLIBRARIES = libBBS.la
libBBS_la_SOURCES = record.c log.c ucache.c stuff.c md5.c crypt.c pass.c bcache.c utmp.c \
	lvars.c $(top_builddir)/libBBS/site.c boards.c bbs_sendmail.c libmsg.c article.c convcode.c \
	sysconf.c libann.c filter.c mgrep.c $(top_builddir)/libBBS/default.c output.c libtmpl.c libpc.c \
	xml.c super_filter.c
AM_CFLAGS = -I$(top_srcdir)/src
CLEANFILES=@EXTRA_CLEAN@
DISTCLEANFILES=Makefile.in

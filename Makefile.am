EXTRA_DIST = lib/* README INSTALL \
             site/fb2k-v2.c site/fb2k-v2.h site/default.c site/default.h \
             bbshome innbbsd config.icc config.lib doc/* makedist.sh \
             contrib/crontab.SMTH contrib/build-RH9.sh bbs2www.in 
SUBDIRS=libsystem libBBS rzsz src daemon local_utl mail2bbs service @BBSSUBDIRS@
CLEANFILES=@EXTRA_CLEAN@
DISTCLEANFILES=configure aclocal.m4 Makefile.in config.guess config.sub depcomp install-sh missing ylwrap kbs_config.h.in ltmain.sh
AM_MAKEFLAGS=-s 
dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -rf `find $(distdir) -name config.status`
	rm -rf `find $(distdir) -name .deps`

install-home:
	sh makedist.sh ${prefix}

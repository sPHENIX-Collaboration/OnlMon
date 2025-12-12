AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlgl1mon_client.la \
  libonlgl1mon_server.la

libonlgl1mon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonlgl1mon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmonodbc


gl1includedir=$(pkgincludedir)/gl1

gl1include_HEADERS = \
  GL1Mon.h \
  GL1MonDraw.h

libonlgl1mon_server_la_SOURCES = \
  GL1Mon.cc

libonlgl1mon_client_la_SOURCES = \
  GL1MonDraw.cc

#bin_SCRIPTS = \
#  GL1MonSetup.csh \
#  GL1MonSetup.sh


noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlgl1mon_server.la

testexternals_client_LDADD = \
  libonlgl1mon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)

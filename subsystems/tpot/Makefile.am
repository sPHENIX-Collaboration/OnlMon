AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonltpotmon_client.la \
  libonltpotmon_server.la

libonltpotmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonltpotmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


tpotincludedir=$(pkgincludedir)/tpot

tpotinclude_HEADERS = \
  MicromegasDefs.h \
  MicromegasMapping.h \
  TpotMon.h \
  TpotMonDraw.h \
  TpotMonGeometry.h

libonltpotmon_server_la_SOURCES = \
  MicromegasMapping.cc \
  TpotMon.cc \
  TpotMonGeometry.cc

libonltpotmon_client_la_SOURCES = \
  MicromegasMapping.cc \
  TpotMonDraw.cc \
  TpotMonGeometry.cc

bin_SCRIPTS = \
  TpotMonSetup.csh \
  TpotMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client

testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonltpotmon_server.la

testexternals_client_LDADD = \
  libonltpotmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)

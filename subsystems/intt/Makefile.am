AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlinttmon_client.la \
  libonlinttmon_server.la

libonlinttmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver


libonlinttmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient


inttincludedir=$(pkgincludedir)/intt

inttinclude_HEADERS = \
  InttMon.h \
  InttMonDraw.h \
  ServerStats.h \
  BcoDiff.h \
  FphxBco.h \
  History.h \
  Hitmap.h \
  Hitrates.cc \
  ZoomedFphxBco.h \
  SingleInttDrawer.h

libonlinttmon_server_la_SOURCES = \
  InttMon.cc

libonlinttmon_client_la_SOURCES = \
  InttMon.cc \
  InttMonDraw.cc \
  ServerStats.cc \
  BcoDiff.cc \
  FphxBco.cc \
  History.cc \
  Hitmap.cc \
  Hitrates.cc \
  ZoomedFphxBco.cc \
  SingleInttDrawer.cc

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlinttmon_server.la

testexternals_client_LDADD = \
  libonlinttmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)

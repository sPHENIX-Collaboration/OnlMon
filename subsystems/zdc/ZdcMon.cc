// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "ZdcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>

#include <cmath>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

ZdcMon::ZdcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new ZdcMon()
  return;
}

ZdcMon::~ZdcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int ZdcMon::Init()
{
  gRandom->SetSeed(rand());
  // read our calibrations from ZdcMonData.dat
  const char *zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(zdccalib) + "/" + "ZdcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  zdchist1 = new TH1F("zdcmon_hist1", "test 1d histo", 101, 0., 100.);
  zdchist2 = new TH2F("zdcmon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);
  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, zdchist1);  // uses the TH1->GetName() as key
  se->registerHisto(this, zdchist2);
  Reset();
  return 0;
}

int ZdcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int ZdcMon::process_event(Event * /* evt */)
{
  evtcnt++;
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("zdchist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  zdchist1->Fill(gRandom->Gaus(50,10));
  zdchist2->Fill(gRandom->Gaus(50,10), gRandom->Gaus(50,10), 1.);

  return 0;
}

int ZdcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}


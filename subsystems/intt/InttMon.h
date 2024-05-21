#ifndef INTT_MON_H
#define INTT_MON_H

#include <onlmon/OnlMon.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

//#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>

#include <TH1D.h>
#include <TH2D.h>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class InttMon : public OnlMon
{
 public:
  // InttMon.cc
  InttMon(const std::string& name);
  virtual ~InttMon();

  int Init();
  int BeginRun(const int);
  int process_event(Event*);
  int Reset();

  int MiscDebug();

  // InttMon_o_Binning.cc
  // (public so InttMonDraw can use them)
  struct HitData_s
  {
    int fee, chp;
  };
  int static HitBin(HitData_s const&);

  struct BcoData_s
  {
    int fee, bco;
  };
  int static BcoBin(BcoData_s const&);

 private:
  // InttMon.cc
  int DBVarInit();
  int DBVarUpdate();

  // Members
  OnlMonDB* dbvars = nullptr;
  int evtcnt = 0;

  TH1D* EvtHist = nullptr;
  TH1D* HitHist = nullptr;
  TH1D* BcoHist = nullptr;
  //...
};

#endif

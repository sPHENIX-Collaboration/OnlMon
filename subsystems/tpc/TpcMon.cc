// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "TpcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>

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

TpcMon::TpcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new TpcMon()
  return;
}

TpcMon::~TpcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  delete dbvars;
  return;
}

int TpcMon::Init()
{
  // read our calibrations from TpcMonData.dat
  const char *tpccalib = getenv("TPCCALIB");
  if (!tpccalib)
  {
    std::cout << "TPCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(tpccalib) + "/" + "TpcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  tpchist1 = new TH1F("tpcmon_hist1", "test 1d histo", 101, 0., 100.);
  tpchist2 = new TH2F("tpcmon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);

  //TPC GEM Module Displays

  NorthSideADC = new TH2F("NorthSideADC" , "ADC Counts North Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  SouthSideADC = new TH2F("SouthSideADC" , "ADC Counts South Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  //


  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, tpchist1);  // uses the TH1->GetName() as key
  se->registerHisto(this, tpchist2);
  se->registerHisto(this, NorthSideADC);
  se->registerHisto(this, SouthSideADC);
  dbvars = new OnlMonDB(ThisName);  // use monitor name for db table name
  DBVarInit();
  Reset();
  return 0;
}

int TpcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int TpcMon::process_event(Event * /* evt */)
{
  evtcnt++;
  OnlMonServer *se = OnlMonServer::instance();
  // using ONLMONBBCLL1 makes this trigger selection configurable from the outside
  // e.g. if the BBCLL1 has problems or if it changes its name
  if (!se->Trigger("ONLMONBBCLL1"))
  {
    std::ostringstream msg;
    msg << "Processing Event " << evtcnt
        << ", Trigger : 0x" << std::hex << se->Trigger()
        << std::dec;
    // severity levels and id's for message sources can be found in
    // $ONLINE_MAIN/include/msg_profile.h
    // The last argument is a message type. Messages of the same type
    // are throttled together, so distinct messages should get distinct
    // message types
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), TRGMESSAGE);
  }
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("tpchist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  tpchist1->Fill((float) idummy);
  tpchist2->Fill((float) idummy, (float) idummy, 1.);

  //fill the TPC module displays
  float r, theta;

  //dummy data
  float North_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };
  float South_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };

  for(int tpciter = 1; tpciter < 73 ; tpciter++){

    Locate(tpciter, &r, &theta);
    //std::cout << "r is: "<< r <<" theta is: "<< theta <<"\n";

    if(tpciter < 37){ //South side
      SouthSideADC->Fill(theta,r, South_Side_Arr[tpciter-1]); //fill South side with the weight = bin content
    }
    else { //North side
      NorthSideADC->Fill(theta,r,North_Side_Arr[tpciter-37]); //fill North side with the weight = bin content
    }
 
  }
  //


  if (idummy++ > 10)
  {
    if (dbvars)
    {
      dbvars->SetVar("tpcmoncount", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("tpcmondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("tpcmonnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    std::ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
  }
  return 0;
}

void TpcMon::Locate(int id, float *rbin, float *thbin)
{
   float CSIDE_angle_bins[12] = { 0.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 6.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 }; //CCW from x = 0 (RHS horizontal)

   float ASIDE_angle_bins[12] = { 6.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 0.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12  }; //CCW from x = 0 (RHS horizontal)

   int modid3 = id % 3;

   switch(modid3) {
     case 1:
       *rbin = 0.4; //R1
       break;
     case 2:
       *rbin = 0.6; //R2
       break;
     case 0:
       *rbin = 0.8; //R3
       break;
   }

  if( id < 37){
    *thbin = CSIDE_angle_bins[TMath::FloorNint((id-1)/3)];
  }
  else if( id >= 37){
    *thbin = ASIDE_angle_bins[TMath::FloorNint((id-37)/3)];
  }
}


int TpcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int TpcMon::DBVarInit()
{
  // variable names are not case sensitive
  std::string varname;
  varname = "tpcmoncount";
  dbvars->registerVar(varname);
  varname = "tpcmondummy";
  dbvars->registerVar(varname);
  varname = "tpcmonnew";
  dbvars->registerVar(varname);
  if (verbosity > 0)
  {
    dbvars->Print();
  }
  dbvars->DBInit();
  return 0;
}

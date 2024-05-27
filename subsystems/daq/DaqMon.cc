// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "DaqMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/eventReceiverClient.h>
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

DaqMon::DaqMon(const std::string &name)
  : OnlMon(name)
{
  return;
}

DaqMon::~DaqMon()
{
  return;
}

int DaqMon::Init()
{
  gRandom->SetSeed(rand());
  const char *daqcalib = getenv("DAQCALIB");
  if (!daqcalib)
  {
    std::cout << "DAQCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(daqcalib) + "/" + "DaqMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  printf("doing the Init\n");

  h_gl1_clock_diff = new TH2F("h_gl1_clock_diff","", 6, 0,6, 2, -0.5, 1.5);
  h_gl1_clock_diff->GetXaxis()->SetTitleSize(0);
  h_gl1_clock_diff->GetYaxis()->SetNdivisions(202);
  h_gl1_clock_diff->GetXaxis()->SetNdivisions(101);
  h_gl1_clock_diff->GetYaxis()->SetBinLabel(1,"#bf{Unlocked}");
  h_gl1_clock_diff->GetYaxis()->SetBinLabel(2,"#bf{Locked}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(1,"#bf{MBD}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(2,"#bf{EMCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(3,"#bf{IHCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(4,"#bf{OHCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(5,"#bf{sEPD}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(6,"#bf{ZDC}");

  h_fem_match = new TH2F("h_fem_match","", 20, -0.5,19.5, 1, 0.5, 1.5);
  for(int is=0;is<10;is++){
      h_fem_match->GetXaxis()->SetBinLabel(is+1,Form("seb0%d",is));
  }
  for(int is=10;is<19;is++){
      h_fem_match->GetXaxis()->SetBinLabel(is+1,Form("seb%d",is));
  }
  h_fem_match->GetXaxis()->SetBinLabel(20,"seb20");
  
  OnlMonServer *se = OnlMonServer::instance();
  se->registerHisto(this, h_gl1_clock_diff);  
  se->registerHisto(this, h_fem_match); 
  Reset();
  erc = new eventReceiverClient("gl1daq");


  std::string mappingfile = std::string(daqcalib) + "/" + "packetid_seb_mapping.txt";
  loadpacketMapping(mappingfile);

  return 0;
}

int DaqMon::BeginRun(const int /* runno */)
{
  return 0;
}

int DaqMon::process_event(Event *e /* evt */)
{
    if (e->getEvtType() >= 8) 
    {
        return 0;
    }
  
  int evtnr = e->getEvtSequence();
  if (evtnr < 3)
  {
    return 0;
  }

  evtcnt++;
  long long int gl1_clock = 0;
  Event *gl1Event = erc->getEvent(evtnr+1);

  if (!gl1Event)
  {
    return 0;
  }
  Packet *pgl1 = gl1Event->getPacket(14001);
  if (pgl1)
  {
    gl1_clock = pgl1->lValue(0, "BCO");
    delete pgl1;
  }
  delete gl1Event;

  Packet *plist[100];
  int npackets = e->getPacketList(plist,100);
  bool mismatchfem = true;
  int femevtref = 0;
  int femclkref = 0;
  int sebid = 0;
  for (int ipacket = 0; ipacket < npackets; ipacket++) {
      Packet * p = plist[ipacket];
      if (p) {
          int pnum = p->getIdentifier();
          int calomapid = CaloPacketMap(pnum);
          long int packet_clock = p->lValue(0,"CLOCK");
          clockdiff[ipacket] = gl1_clock  - packet_clock;
          int fdiff = (clockdiff[ipacket] != previousdiff[ipacket]) ? 0 : 1;
          previousdiff[ipacket] = clockdiff[ipacket];

          int nADCs = p->iValue(0,"NRMODULES");
          for(int iadc = 0; iadc<nADCs ; iadc++){
              if(ipacket==0 && iadc==0){ femevtref = p->iValue(iadc,"FEMEVTNR"); femclkref = p->iValue(iadc,"FEMCLOCK");}

              if(femevtref !=  p->iValue(iadc,"FEMEVTNR") || fabs(femclkref - p->iValue(0,"FEMCLOCK"))>2)
              {
                  mismatchfem = false;
                  sebid = getmapping(pnum);
              }
          }
          
          if(evtcnt>3) h_gl1_clock_diff->Fill(calomapid,fdiff);
          
      }
      delete p;
  }
  if(mismatchfem == false) h_fem_match->Fill(sebid,1);

  return 0;
}

int DaqMon::Reset()
{
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int DaqMon::CaloPacketMap(int pnum)
{
  int caloid = -1;
  if (pnum >= packet_mbd_low && pnum <= packet_mbd_high)
  {
    caloid = 0;
  }
  else if (pnum >= packet_emcal_low && pnum <= packet_emcal_high)
  {
    caloid = 1;
  }
  else if (pnum >= packet_ihcal_low && pnum <= packet_ihcal_high)
  {
    caloid = 2;
  }
  else if (pnum >= packet_ohcal_low && pnum <= packet_ohcal_high)
  {
    caloid = 3;
  }
  else if (pnum >= packet_sepd_low && pnum <= packet_sepd_high)
  {
    caloid = 4;
  }
  else if (pnum >= packet_zdc)
  {
    caloid = 5;
  }
  return caloid;
}

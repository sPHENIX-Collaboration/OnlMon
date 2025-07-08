// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "BbcMon.h"
#include <mbd/MbdEvent.h>

#include <onlmon/OnlMon.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>
#include <onlmon/RunDBodbc.h>
#include <onlmon/triggerEnum.h>

#include <Event/msg_profile.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>
#include <Event/eventReceiverClient.h>

#include <mbd/MbdGeomV1.h>
#include <mbd/MbdOutV2.h>
#include <mbd/MbdPmtContainerV1.h>
#include <mbd/MbdPmtHit.h>

#include <TF1.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2Poly.h>
#include <TString.h>
#include <TSystem.h>

#include <cmath>
#include <cstdio>  // for printf
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>


/*
enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};
*/

BbcMon::BbcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new BbcMon()
  return;
}

BbcMon::~BbcMon()
{
  delete bevt;
  delete _mbdgeom;
  delete erc;

  return;
}

int BbcMon::Init()
{
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  std::cout << "BbcMon::Init()" << std::endl;

  bevt = new MbdEvent();
  _mbdgeom = new MbdGeomV1();

  // Set trigger bits
  mbdns = (0x1UL << TriggerEnum::MBD_NS2) | (0x1UL << TriggerEnum::MBD_NS1); // mbd wide triggers
  mbdnsvtx10 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX10) | (0x1UL << TriggerEnum::MBD_NS1_ZVRTX10);
  mbdnsvtx30 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX30);
  mbdnsvtx150 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX150);
  mbdtrig = mbdns | mbdnsvtx10 | mbdnsvtx30 | mbdnsvtx150;
  zdcns = (0x1UL << TriggerEnum::ZDC_NS);
  emcal = (0x1UL << TriggerEnum::PHOTON6_MBD_NS2) | (0x1UL << TriggerEnum::PHOTON8_MBD_NS2)
          | (0x1UL << TriggerEnum::PHOTON10_MBD_NS2) | (0x1UL << TriggerEnum::PHOTON12_MBD_NS2);
  hcal = (0x1UL << TriggerEnum::HCAL_SINGLES);
  emcalmbd = emcal | mbdtrig;
  hcalmbd = hcal | mbdtrig;
  trigmask = mbdtrig | zdcns | emcal | hcal;    // any reasonable trigger

  // read settings from BbcMonData.dat
  const char *bbccalib = getenv("BBCCALIB");
  if (!bbccalib)
  {
    std::cout << "BBCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string configfname = std::string(bbccalib) + "/" + "BbcMonData.dat";
  std::ifstream configfile(configfname);
  if ( configfile.is_open() )
  {
    std::cout << "MBD: Reading " << configfname << std::endl;
    std::string label;
    uint64_t trigbit{0};
    while ( configfile >> label >> std::hex >> trigbit >> std::dec )
    {
      if ( label == "#" )
      {
          continue;
      }
      else if ( label == "USEGL1" )
      {
          useGL1 = static_cast<int>(trigbit);
      }
      else if ( label == "TRIGMASK" )
      {
          trigmask = trigbit;
          std::cout << "Overriding with trigmask " << label << " 0x" << std::hex << trigbit << std::dec << std::endl;
      }
      else if ( label[0] != '#' && label[0] != '/' )
      {
        std::cout << "Using trigger " << label << " 0x" << std::hex << trigbit << std::dec << std::endl;
      }
    }

    configfile.close();
  }
  else
  {
    std::cout << "MBD: ERROR, " << configfname << " not found" << std::endl;
    exit(1);
  }
  orig_trigmask = trigmask;

  // get gl1 event receiver
  if ( useGL1==1 )
  {
    erc = new eventReceiverClient("gl1daq");
    rdb = new RunDBodbc;
  }
  else if ( useGL1==2 )
  {
    std::cout << "Connecting to eventserver on localhost" << std::endl;
    erc = new eventReceiverClient("localhost");
    rdb = new RunDBodbc;
  }

  // Book Histograms

  // Trigger Information ----------------------------------------------------
  if ( useGL1 )
  {
    bbc_trigs = new TH1F("bbc_trigs", "Trigger Counts", 64, -0.5, 63.5);
  }
    // initialize auto-update trigger histograms
    for ( int i = 0; i < TriggerEnum::NUM_MBD_TRIGGERS; i++ ){
      std::string name = Form("bbc_zvertex_autoupdate_%i", i);
      bbc_zvertex_autoupdate[i] = new TH1F(name.c_str(), 
        Form("MBD ZVertex Trigger %s", TriggerEnum::MBTriggerNames[i]),
        BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
      bbc_zvertex_autoupdate[i]->Sumw2();
      bbc_zvertex_autoupdate[i]->GetXaxis()->SetTitle("ZVertex [cm]");
      bbc_zvertex_autoupdate[i]->GetYaxis()->SetTitle("Number of Event");
      bbc_zvertex_autoupdate[i]->GetXaxis()->SetTitleSize(0.05);
      bbc_zvertex_autoupdate[i]->GetYaxis()->SetTitleSize(0.05);
      bbc_zvertex_autoupdate[i]->GetXaxis()->SetTitleOffset(0.70);
      bbc_zvertex_autoupdate[i]->GetYaxis()->SetTitleOffset(1.75);
      bbc_zvertex_autoupdate[i]->GetXaxis()->SetLabelSize(0.07);
      bbc_zvertex_autoupdate[i]->GetXaxis()->SetTickSize(0.1);
    }
  // }

  // Nhit Distributions
  bbc_south_nhit = new TH1F("bbc_south_nhit","MBD.S Nhits",64,-0.5,63.5);
  bbc_north_nhit = new TH1F("bbc_north_nhit","MBD.N Nhits",64,-0.5,63.5);
  for (int iarm=0; iarm<2; iarm++)
  {
    TString name = "bbc_nhit_emcal"; name += iarm;
    bbc_nhit_emcal[iarm] = new TH1F(name,"MBD Nhits, EMCAL trig",64,-0.5,63.5);
    name = "bbc_nhit_hcal"; name += iarm;
    bbc_nhit_hcal[iarm] = new TH1F(name,"MBD Nhits, JET trig",64,-0.5,63.5);
    name = "bbc_nhit_emcalmbd"; name += iarm;
    bbc_nhit_emcalmbd[iarm] = new TH1F(name,"MBD Nhits, EMCAL&&MBD trig",64,-0.5,63.5);
    name = "bbc_nhit_hcalmbd"; name += iarm;
    bbc_nhit_hcalmbd[iarm] = new TH1F(name,"MBD Nhits, JET&&MBD trig",64,-0.5,63.5);
  }

  // TDC Distribution ----------------------------------------------------

  bbc_tdc = new TH2F("bbc_tdc", "BBC Raw TDC Distribution",
                     nPMT_BBC, -.5, nPMT_BBC - .5,
                     BbcMonDefs::nBIN_TDC, 0, BbcMonDefs::tdc_max_overflow * BbcMonDefs::TDC_CONVERSION_FACTOR);
  //std::cout << "BBCTDC " << (uint64_t)bbc_tdc << std::endl;

  // TDC Overflow Deviation ----------------------------------------------
  bbc_tdc_overflow = new TH2F("bbc_tdc_overflow", "MBD TDC Overflow Deviation",
                              nPMT_BBC, -.5, nPMT_BBC - .5,
                              int(BbcMonDefs::VIEW_OVERFLOW_MAX - BbcMonDefs::VIEW_OVERFLOW_MIN + 1),
                              BbcMonDefs::VIEW_OVERFLOW_MIN - .5, BbcMonDefs::VIEW_OVERFLOW_MAX + .5);

  std::ostringstream name, title;

  // TDC Overflow Distribution for each PMT ------------------------------
  for (int ipmt = 0; ipmt < nPMT_BBC; ipmt++)
  {
    name << "bbc_tdc_overflow_" << std::setw(3) << std::setfill('0') << ipmt;
    title << "MBD TDC Overflow Deviation of #" << std::setw(3) << std::setfill('0') << ipmt;
    bbc_tdc_overflow_each[ipmt] = new TH1F(name.str().c_str(), title.str().c_str(),
                                           int(BbcMonDefs::VIEW_OVERFLOW_MAX - BbcMonDefs::VIEW_OVERFLOW_MIN + 1),
                                           BbcMonDefs::VIEW_OVERFLOW_MIN, BbcMonDefs::VIEW_OVERFLOW_MAX);
    name.str("");
    name.clear();
    title.str("");
    title.clear();
  }

  // ADC Distribution --------------------------------------------------------

  bbc_adc = new TH2F("bbc_adc", "MBD ADC(Charge) Distribution", nPMT_BBC, -.5, nPMT_BBC - .5, BbcMonDefs::nBIN_ADC, 0, BbcMonDefs::MAX_ADC_MIP);

  bbc_tdc_armhittime = new TH2F("bbc_tdc_armhittime", "Arm-Hit-Time Correlation of North and South MBD",
                                64, BbcMonDefs::min_armhittime, BbcMonDefs::max_armhittime,
                                64, BbcMonDefs::min_armhittime, BbcMonDefs::max_armhittime);
  bbc_tdc_armhittime->GetXaxis()->SetTitle("South [ns]");
  bbc_tdc_armhittime->GetYaxis()->SetTitle("North [ns]");

  // Vertex Distributions --------------------------------------------------------

  bbc_zvertex = new TH1F("bbc_zvertex", "MBD ZVertex, main trigger", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex->Sumw2();
  bbc_zvertex->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_alltrigger = new TH1F("bbc_zvertex_alltrigger", "MBD ZVertex, all triggers",
                                    BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_alltrigger->Sumw2();
  bbc_zvertex_alltrigger->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_alltrigger->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_alltrigger->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_alltrigger->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_alltrigger->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_alltrigger->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_alltrigger->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_alltrigger->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_short = new TH1F("bbc_zvertex_short", "MBD ZVertex (NS, wide), short time scale",
                               BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_short->Sumw2();
  bbc_zvertex_short->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_short->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_short->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_short->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_short->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_short->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_ns = new TH1F("bbc_zvertex_ns", "MBD zvertex_ns, main trigger", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_ns->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_ns->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_ns->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_ns->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_ns->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_ns->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_ns->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_ns->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_10 = new TH1F("bbc_zvertex_10", "MBD ZVertex (|z|<10)",
                             BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_10->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_10->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_10->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_10->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_10->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_10->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_10->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_10->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_30 = new TH1F("bbc_zvertex_30", "MBD ZVertex (|z|<30)",
                            BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_30->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_30->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_30->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_30->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_30->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_30->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_30->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_30->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_60 = new TH1F("bbc_zvertex_60", "MBD ZVertex (|z|<60)",
                            BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_60->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_60->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_60->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_60->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_60->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_60->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_60->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_60->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_10_chk = new TH1F("bbc_zvertex_10_chk", "MBD ZVertex (|z|<10)",
                             BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_10_chk->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_10_chk->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_10_chk->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_10_chk->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_10_chk->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_10_chk->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_10_chk->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_10_chk->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_30_chk = new TH1F("bbc_zvertex_30_chk", "MBD ZVertex (|z|<30)",
                            BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_30_chk->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_30_chk->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_30_chk->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_30_chk->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_30_chk->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_30_chk->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_30_chk->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_30_chk->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_60_chk = new TH1F("bbc_zvertex_60_chk", "MBD ZVertex (|z|<60)",
                            BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_60_chk->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_60_chk->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_60_chk->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_60_chk->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_60_chk->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_60_chk->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_60_chk->GetXaxis()->SetLabelSize(0.05);
  bbc_zvertex_60_chk->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_zdcns = new TH1F("bbc_zvertex_zdcns", "MBD zvertex, ZDCNS trig", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_zdcns->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_zdcns->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_zdcns->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_zdcns->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_zdcns->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_zdcns->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_zdcns->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_zdcns->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_emcal = new TH1F("bbc_zvertex_emcal", "MBD zvertex, PHOTON trig", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_emcal->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_emcal->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_emcal->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_emcal->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_emcal->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_emcal->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_emcal->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_emcal->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_hcal = new TH1F("bbc_zvertex_hcal", "MBD zvertex, JET trig", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_hcal->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_hcal->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_hcal->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_hcal->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_hcal->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_hcal->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_hcal->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_hcal->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_emcalmbd = new TH1F("bbc_zvertex_emcalmbd", "MBD zvertex, PHOTON&&MBD trig", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_emcalmbd->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_emcalmbd->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_emcalmbd->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_emcalmbd->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_emcalmbd->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_emcalmbd->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_emcalmbd->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_emcalmbd->GetXaxis()->SetTickSize(0.1);

  bbc_zvertex_hcalmbd = new TH1F("bbc_zvertex_hcalmbd", "MBD zvertex, JET&&MBD trig", BbcMonDefs::zvtnbin, BbcMonDefs::min_zvertex, BbcMonDefs::max_zvertex);
  bbc_zvertex_hcalmbd->GetXaxis()->SetTitle("zvertex [cm]");
  bbc_zvertex_hcalmbd->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_hcalmbd->GetXaxis()->SetTitleSize(0.05);
  bbc_zvertex_hcalmbd->GetYaxis()->SetTitleSize(0.05);
  bbc_zvertex_hcalmbd->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_hcalmbd->GetYaxis()->SetTitleOffset(1.75);
  bbc_zvertex_hcalmbd->GetXaxis()->SetLabelSize(0.07);
  bbc_zvertex_hcalmbd->GetXaxis()->SetTickSize(0.1);

  f_zvtx = new TF1("f_zvtx", "gaus", -30., 30.);
  bbc_nevent_counter = new TH1F("bbc_nevent_counter",
                                "The nEvent Counter bin1:Total Event bin2:Collision Event bin3:Laser Event",
                                16, 0, 16);

  // bbc_tzero_zvtx = new TH2F("bbc_tzero_zvtx",
  //     "TimeZero vs ZVertex", 100, -200, 200, 110, -11, 11 );
  bbc_tzero_zvtx = new TH2F("bbc_tzero_zvtx", "TimeZero vs ZVertex", 100, -200, 200, 110, -16, 16);
  bbc_tzero_zvtx->SetXTitle("ZVertex [cm]");
  bbc_tzero_zvtx->SetYTitle("TimeZero[ns]");

  bbc_avr_hittime = new TH1F("bbc_avr_hittime", "MBD Average Hittime", 128, 0, 24);
  bbc_avr_hittime->Sumw2();
  bbc_avr_hittime->GetXaxis()->SetTitle("Avr HitTime [ns]");
  bbc_avr_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_avr_hittime->GetXaxis()->SetTitleSize(0.05);
  bbc_avr_hittime->GetYaxis()->SetTitleSize(0.05);
  bbc_avr_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_avr_hittime->GetYaxis()->SetTitleOffset(1.75);

  // should make plots of the raw tdc for time channels
  // bbc_south_hittime = new TH1F("bbc_south_hittime", "BBC South Hittime", 164, -100, 16300);
  // bbc_north_hittime = new TH1F("bbc_north_hittime", "BBC North Hittime", 164, -100, 16300);
  bbc_south_hittime = new TH1F("bbc_south_hittime", "MBD South Hittime", 150, -15, 15);
  bbc_north_hittime = new TH1F("bbc_north_hittime", "MBD North Hittime", 150, -15, 15);

  bbc_south_hittime->GetXaxis()->SetTitle("South HitTime [ns]");
  bbc_south_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_south_hittime->GetXaxis()->SetTitleSize(0.05);
  bbc_south_hittime->GetYaxis()->SetTitleSize(0.05);
  bbc_south_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_south_hittime->GetYaxis()->SetTitleOffset(1.75);

  bbc_north_hittime->GetXaxis()->SetTitle("North HitTime [ns]");
  bbc_north_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_north_hittime->GetXaxis()->SetTitleSize(0.05);
  bbc_north_hittime->GetYaxis()->SetTitleSize(0.05);
  bbc_north_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_north_hittime->GetYaxis()->SetTitleOffset(1.75);

  // bbc_south_chargesum = new TH1F("bbc_south_chargesum", "BBC South ChargeSum [MIP]", 128, 0, BbcMonDefs::MAX_CHARGE_SUM);
  // bbc_north_chargesum = new TH1F("bbc_north_chargesum", "BBC North ChargeSum [MIP]", 128, 0, BbcMonDefs::MAX_CHARGE_SUM);
  bbc_south_chargesum = new TH1F("bbc_south_chargesum", "BBC South ChargeSum [AU]", 128, 0, BbcMonDefs::MAX_CHARGE_SUM);
  bbc_north_chargesum = new TH1F("bbc_north_chargesum", "BBC North ChargeSum [AU]", 128, 0, BbcMonDefs::MAX_CHARGE_SUM);

  bbc_north_chargesum->SetTitle("MBD ChargeSum [MIP]");
  bbc_north_chargesum->GetXaxis()->SetTitle("ChargeSum [MIP]");
  // bbc_north_chargesum->GetXaxis()->SetTitle("North ChargeSum [MIP]");
  bbc_north_chargesum->GetYaxis()->SetTitle("Number of Event");
  bbc_north_chargesum->GetXaxis()->SetTitleSize(0.05);
  bbc_north_chargesum->GetYaxis()->SetTitleSize(0.05);
  bbc_north_chargesum->GetXaxis()->SetTitleOffset(0.70);
  bbc_north_chargesum->GetYaxis()->SetTitleOffset(1.75);

  // bbc_south_chargesum->GetXaxis()->SetTitle("South ChargeSum [MIP]");
  bbc_south_chargesum->GetYaxis()->SetTitle("Number of Event");
  bbc_south_chargesum->GetXaxis()->SetTitleSize(0.05);
  bbc_south_chargesum->GetYaxis()->SetTitleSize(0.05);
  bbc_south_chargesum->GetXaxis()->SetTitleOffset(0.70);
  bbc_south_chargesum->GetYaxis()->SetTitleOffset(1.75);

  // scale down factor for each trigger
  bbc_prescale_hist = new TH1F("bbc_prescale_hist", "prescales", 64, 0, 64);
  bbc_prescale_hist->SetXTitle("trigger");

  // waveforms
  bbc_time_wave = new TH2F("bbc_time_wave", "MBD time waveforms by ch", BbcMonDefs::BBC_MAXSAMPLES, -0.5, BbcMonDefs::BBC_MAXSAMPLES - 0.5, 128, 0, 128);
  bbc_charge_wave = new TH2F("bbc_charge_wave", "MBD charge waveforms by ch", BbcMonDefs::BBC_MAXSAMPLES, -0.5, BbcMonDefs::BBC_MAXSAMPLES - 0.5, 128, 0, 128);

  bbc_time_wave->GetXaxis()->SetTitle("Sample");
  bbc_time_wave->GetYaxis()->SetTitle("Ch");
  bbc_time_wave->GetXaxis()->SetTitleSize(0.05);
  bbc_time_wave->GetYaxis()->SetTitleSize(0.05);
  bbc_time_wave->GetXaxis()->SetTitleOffset(0.70);
  bbc_time_wave->GetYaxis()->SetTitleOffset(0.75);
  bbc_charge_wave->GetXaxis()->SetTitle("Sample");
  bbc_charge_wave->GetYaxis()->SetTitle("Ch");
  bbc_charge_wave->GetXaxis()->SetTitleSize(0.05);
  bbc_charge_wave->GetYaxis()->SetTitleSize(0.05);
  bbc_charge_wave->GetXaxis()->SetTitleOffset(0.70);
  bbc_charge_wave->GetYaxis()->SetTitleOffset(0.75);

  // hitmaps
  bbc_south_hitmap = new TH2Poly();
  bbc_south_hitmap->SetName("bbc_south_hitmap");
  bbc_south_hitmap->SetTitle("MBD South Hitmap");
  bbc_south_hitmap->GetXaxis()->SetTitle("x (cm)");
  bbc_south_hitmap->GetYaxis()->SetTitle("y (cm)");
  bbc_south_hitmap->GetXaxis()->SetTitleSize(0.05);
  bbc_south_hitmap->GetYaxis()->SetTitleSize(0.05);
  bbc_south_hitmap->GetXaxis()->SetTitleOffset(0.70);
  bbc_south_hitmap->GetYaxis()->SetTitleOffset(0.70);
  bbc_south_hitmap->SetMinimum(0.001);

  bbc_north_hitmap = new TH2Poly();
  bbc_north_hitmap->SetName("bbc_north_hitmap");
  bbc_north_hitmap->SetTitle("MBD North Hitmap");
  bbc_north_hitmap->GetXaxis()->SetTitle("x (cm)");
  bbc_north_hitmap->GetYaxis()->SetTitle("y (cm)");
  bbc_north_hitmap->GetXaxis()->SetTitleSize(0.05);
  bbc_north_hitmap->GetYaxis()->SetTitleSize(0.05);
  bbc_north_hitmap->GetXaxis()->SetTitleOffset(0.70);
  bbc_north_hitmap->GetYaxis()->SetTitleOffset(0.70);
  bbc_north_hitmap->SetMinimum(0.001);

  // Get the detector geometry
  Double_t x[6];  // x,y location of the 6 points of the BBC hexagonal PMT's, in cm
  Double_t y[6];
  for (int ipmt = 0; ipmt < 128; ipmt++)
  {
    float xcent = _mbdgeom->get_x(ipmt);
    float ycent = _mbdgeom->get_y(ipmt);
    int arm = _mbdgeom->get_arm(ipmt);
    std::cout << ipmt << "\t" << xcent << "\t" << ycent << std::endl;

    // create hexagon
    x[0] = xcent - 0.8;  // in cm
    y[0] = ycent + 1.4;
    x[1] = xcent + 0.8;
    y[1] = ycent + 1.4;
    x[2] = xcent + 1.6;
    y[2] = ycent;
    x[3] = xcent + 0.8;
    y[3] = ycent - 1.4;
    x[4] = xcent - 0.8;
    y[4] = ycent - 1.4;
    x[5] = xcent - 1.6;
    y[5] = ycent;

    if (arm == 0)
    {
      bbc_south_hitmap->AddBin(6, x, y);
    }
    else if (arm == 1)
    {
      bbc_north_hitmap->AddBin(6, x, y);
    }
  }

  // register histograms with server otherwise client won't get them
  OnlMonServer *se = OnlMonServer::instance();

  // if ( useGL1 )
  // {
    se->registerHisto(this, bbc_trigs);
    for ( int i = 0; i < TriggerEnum::NUM_MBD_TRIGGERS; i++ ){
      se->registerHisto(this, bbc_zvertex_autoupdate[i]);
    }
  // }
  se->registerHisto(this, bbc_south_nhit);
  se->registerHisto(this, bbc_north_nhit);
  for (int iarm=0; iarm<2; iarm++)
  {
    se->registerHisto(this, bbc_nhit_emcal[iarm]);
    se->registerHisto(this, bbc_nhit_hcal[iarm]);
    se->registerHisto(this, bbc_nhit_emcalmbd[iarm]);
    se->registerHisto(this, bbc_nhit_hcalmbd[iarm]);
  }
  se->registerHisto(this, bbc_adc);
  se->registerHisto(this, bbc_tdc);
  se->registerHisto(this, bbc_tdc_overflow);
  for (auto &ipmt : bbc_tdc_overflow_each)
  {
      se->registerHisto(this, ipmt);
  }

  se->registerHisto(this, bbc_tdc_armhittime);
  se->registerHisto(this, bbc_zvertex);
  se->registerHisto(this, bbc_zvertex_alltrigger);
  se->registerHisto(this, bbc_zvertex_ns);
  se->registerHisto(this, bbc_zvertex_10);
  se->registerHisto(this, bbc_zvertex_30);
  se->registerHisto(this, bbc_zvertex_60);
  se->registerHisto(this, bbc_zvertex_10_chk);
  se->registerHisto(this, bbc_zvertex_30_chk);
  se->registerHisto(this, bbc_zvertex_60_chk);
  se->registerHisto(this, bbc_zvertex_zdcns);
  se->registerHisto(this, bbc_zvertex_emcal);
  se->registerHisto(this, bbc_zvertex_hcal);
  se->registerHisto(this, bbc_zvertex_emcalmbd);
  se->registerHisto(this, bbc_zvertex_hcalmbd);
  se->registerHisto(this, bbc_nevent_counter);
  se->registerHisto(this, bbc_tzero_zvtx);
  se->registerHisto(this, bbc_prescale_hist);
  se->registerHisto(this, bbc_avr_hittime);
  se->registerHisto(this, bbc_north_hittime);
  se->registerHisto(this, bbc_south_hittime);
  se->registerHisto(this, bbc_north_chargesum);
  se->registerHisto(this, bbc_south_chargesum);
  se->registerHisto(this, bbc_north_hitmap);
  se->registerHisto(this, bbc_south_hitmap);
  /*
  se->registerHisto(this, bbc_tmax[0]);
  se->registerHisto(this, bbc_tmax[1]);
  */
  se->registerHisto(this, bbc_time_wave);
  se->registerHisto(this, bbc_charge_wave);

  /*
  dbvars = new OnlMonDB(ThisName);  // use monitor name for db table name
  DBVarInit();
  */
  Reset();

  m_mbdout = new MbdOutV2();
  m_mbdpmts = new MbdPmtContainerV1();

  // prep the vtx to MCR info
  char hname[1024];
  gethostname(hname,sizeof(hname)-1);
  sendflagfname = "/home/phnxrc/operations/mbd/mbd2mcr.";
  sendflagfname += hname;
  std::cout << "sendflagfname " << sendflagfname << "\t" << hname << std::endl;
  fillnumber = 0;
  if ( useGL1==1 )
  {
    UpdateSendFlag( 0 );
  }

  gl1badflagfname = "/home/phnxrc/operations/mbd/mbdgl1bypass.";
  gl1badflagfname += hname;
  std::cout << "gl1badflagfname " << gl1badflagfname << "\t" << hname << std::endl;

  return 0;
}

int BbcMon::BeginRun(const int runno)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  std::cout << "BbcMon::BeginRun(), run " << runno << std::endl;
  Reset();
  if ( useGL1 )
  {
    OnlMonServer *se = OnlMonServer::instance();
    se->UseGl1();
  }
  bevt->InitRun();

  // stop sending vtx on a new fill
  int prev_fill = fillnumber;
  int current_fill = GetFillNumber();
  if ( prev_fill==0 || current_fill!=prev_fill )
  {
    std::cout << "MBD: Found new fill " << current_fill << std::endl;
    fillnumber = current_fill;
    if ( useGL1==1 )
    {
      UpdateSendFlag( 0 );
    }
  }

  // get gl1badflag on new run
  GetGL1BadFlag();

  uint64_t trigs_enabled = 0;
  if ( rdb != nullptr )
  {
    std::vector<int> scaledowns;
    rdb->GetScaledowns( scaledowns, runno );
    bbc_prescale_hist->Reset();
    for ( int itrig = 0; itrig < 64; itrig++)
    {
      bbc_prescale_hist->SetBinContent( itrig+1, scaledowns[itrig] );
      std::cout << "scaledowns " << itrig << "\t" << scaledowns[itrig] << std::endl;

      if ( scaledowns[itrig] >= 0 )
      {
        trigs_enabled |= (0x1UL<<itrig);
      }
    }
  }
  std::cout << "trigs_enabled 0x" << std::hex << trigs_enabled << std::dec << std::endl;

  mbdbest = GetMinBiasTrigBit( trigs_enabled );
  mbdwidebest = GetMinBiasWideTrigBit( trigs_enabled );

  if ( mbdbest == std::numeric_limits<uint64_t>::max() )
  {
    std::cout << "Oddball run without a proper MB trigger, using all triggers instead" << std::endl;
    trigmask = std::numeric_limits<uint64_t>::max();
    std::cout << std::hex << "trigmask " << trigmask << std::dec << std::endl;
  }
  else
  {
    trigmask = orig_trigmask;
  }

  return 0;
}

int BbcMon::GetFillNumber()
{
  TString retval = gSystem->GetFromPipe( "/home/phnxrc/mbd/chiu/mbd_operations/httpRequestDemo.py -g ringSpec.blue fillNumberM | awk 'NR==1 {print $3}'" );
  if ( retval.IsDec() )
  {
    int fill = retval.Atoi();
    return fill;
  }

  std::cerr << PHWHERE << "GetFromPipe() failed with retval " << retval << std::endl;
  return 0; // default if we get back a junk value
}

uint64_t BbcMon::GetMinBiasTrigBit(uint64_t trigs_enabled)
{
  // look for MB triggers, and select lowest prescale
  std::vector<int> widebits = {
    TriggerEnum::MBD_NS2_ZVRTX10,
    TriggerEnum::MBD_NS1_ZVRTX10,
    TriggerEnum::MBD_NS2_ZVRTX30,
    TriggerEnum::MBD_NS1,
    TriggerEnum::MBD_NS2,
    TriggerEnum::MBD_NS2_ZVRTX150,
  };
  
  int best_scaledown = 999999999;
  int best_trig = -1;
  for ( int itrig : widebits )
  {
    int scaledown = bbc_prescale_hist->GetBinContent( itrig + 1 );
    std::cout << "BEST_TRIG " << itrig << "\t" << scaledown << std::endl;
    if ( scaledown>=0 && scaledown<best_scaledown )
    {
      best_scaledown = scaledown;
      best_trig = itrig;
    }
  }

  if ( best_trig>=0 )
  {
    std::cout << "BEST MBDTRIG IS " << best_trig << std::endl;
    return ( 0x1UL << best_trig );
  }

  // no MBD only trigger, look for ZDC trigger
  if ( (zdcns&trigs_enabled) == zdcns )
  {
    return zdcns;
  }

  // no match, use any trigger
  return std::numeric_limits<uint64_t>::max();
}

uint64_t BbcMon::GetMinBiasWideTrigBit(uint64_t trigs_enabled)
{
  // look for MB triggers, and select lowest prescale
  std::vector<int> widebits = {
    TriggerEnum::MBD_NS1,
    TriggerEnum::MBD_NS2,
    TriggerEnum::MBD_NS2_ZVRTX150
  };
  
  int best_scaledown = 99999999;
  int best_trig = -1;
  for ( int itrig : widebits )
  {
    int scaledown = bbc_prescale_hist->GetBinContent( itrig + 1 );
    if ( scaledown>=0 && scaledown<best_scaledown )
    {
      best_scaledown = scaledown;
      best_trig = itrig;
    }
  }

  if ( best_trig>=0 )
  {
    std::cout << "BEST WIDE MBDTRIG IS " << best_trig << std::endl;
    return ( 0x1UL << best_trig );
  }

  // no MBD only trigger, look for ZDC trigger
  if ( (zdcns&trigs_enabled) == zdcns )
  {
    std::cout << "BEST WIDE MBDTRIG IS ZDCNS" << std::endl;
    return zdcns;
  }

  // no match, use any trigger
  return std::numeric_limits<uint64_t>::max();
}

int BbcMon::UpdateSendFlag(const int flag)
{
  sendflag = flag;
  std::ofstream sendflagfile( sendflagfname );
  if ( sendflagfile.is_open() )
  {
    sendflagfile << sendflag << std::endl;
  }
  else
  {
    std::cout << "unable to open file " << sendflagfname << std::endl;
    return 0;
  }
  sendflagfile.close();
  return 1;
}

int BbcMon::GetSendFlag()
{
  std::ifstream sendflagfile( sendflagfname );
  if ( sendflagfile.is_open() )
  {
    sendflagfile >> sendflag;
  }
  else
  {
    std::cout << "unable to open file " << sendflagfname << std::endl;
    sendflag = 0;
  }
  sendflagfile.close();

  return sendflag;
}

int BbcMon::UpdateGL1BadFlag(const int flag)
{
  gl1badflag = flag;
  std::ofstream gl1badflagfile( gl1badflagfname );
  if ( gl1badflagfile.is_open() )
  {
    gl1badflagfile << gl1badflag << std::endl;
  }
  else
  {
    std::cout << "unable to open file " << gl1badflagfname << std::endl;
    return 0;
  }
  gl1badflagfile.close();
  std::cout << "YYY setting gl1bad " << gl1badflag << std::endl;
  return 1;
}

int BbcMon::GetGL1BadFlag()
{
  std::ifstream gl1badflagfile( gl1badflagfname );
  if ( gl1badflagfile.is_open() )
  {
    gl1badflagfile >> gl1badflag;
  }
  else
  {
    std::cout << "unable to open file " << gl1badflagfname << std::endl;
    gl1badflag = 0;
  }
  gl1badflagfile.close();
  //std::cout << "XXX gl1badflag " << gl1badflag << std::endl;

  return gl1badflag;
}

int BbcMon::EndRun(const int /* runno */)
{
  // This does nothing for now, but can put summary info here for debugging

  return 0;
}

int BbcMon::process_event(Event *evt)
{
  /*
  if (evt->getEvtType() == 9)  // spin patterns stored in BeginRun
  {
    std::cout << "Found begin run event " << std::endl;
    Packet *pBlueFillNumber = evt->getPacket(14915);
    //pYellFillNumber = evt->getPacket(packet_YELLFILLNUMBER);
    if ( pBlueFillNumber )
    {
      int fillnumberBlue = pBlueFillNumber->iValue(0);
      std::cout << "Blue fill number " << fillnumberBlue << std::endl;
      delete pBlueFillNumber;
    }
  }
  */

  if (evt->getEvtType() != DATAEVENT)
  {
    return 0;
  }

  evtcnt++;
  bbc_nevent_counter->Fill(0);

  [[maybe_unused]] OnlMonServer *se = OnlMonServer::instance();

  Packet *p[2];
  p[0] = evt->getPacket(1001);
  p[1] = evt->getPacket(1002);

  // Check that we have both MBD/BBC packets
  if (!p[0] || !p[1])
  {
    std::ostringstream msg;
    msg << "MBD packet not found";
    se->send_message(this, MSG_SOURCE_BBC, MSG_SEV_WARNING, msg.str(), 1);
    msg.str("");
    bbc_nevent_counter->Fill(3);  // bad event, missing packets

    delete p[0];
    delete p[1];

    return 0;
  }

  // Check that both MBD/BBC packets have good checksums
  if ((p[0]->iValue(0, "EVENCHECKSUMOK") == 0) || (p[0]->iValue(0, "ODDCHECKSUMOK") == 0) ||
      (p[1]->iValue(0, "EVENCHECKSUMOK") == 0) || (p[1]->iValue(0, "ODDCHECKSUMOK") == 0))
  {
    std::ostringstream msg;
    msg << "MBD packets have bad checksum(s)";
    se->send_message(this, MSG_SOURCE_BBC, MSG_SEV_WARNING, msg.str(), 1);
    msg.str("");
    bbc_nevent_counter->Fill(4);  // bad event, missing packets

    delete p[0];
    delete p[1];

    return 0;
  }

  delete p[0];
  delete p[1];

  int f_evt = evt->getEvtSequence();
  if ( Verbosity() && f_evt%1000 == 0 )
  {
    std::cout << "mbd evt " << f_evt << "\t" << useGL1 << std::endl;
  }

  if ( f_evt < skipto )
  {
      if ( (f_evt%10000)==0 )
      {
          std::cout << "skipping " << f_evt << std::endl;
      }
      return 0;
  }

  if ( (f_evt%1000)==0 )
  {
    GetGL1BadFlag();
  }

  // Get Trigger Info
  if ( useGL1 )
  {
    
    triggervec = 0UL;
    triginput = 0UL;
    trigraw = 0UL;
    triglive = 0UL;
    trigscaled = 0UL;
    Event *gl1Event = erc->getEvent( f_evt );
    //std::cout << "gl1event " << (uint64_t)gl1Event << "\t" << f_evt << std::endl;

    if (gl1Event)
    {      
        se->IncrementGl1FoundCounter();
        //std::cout << "Found gl1event " << f_evt << std::endl;
        Packet* p_gl1 = gl1Event->getPacket(14001);
        if (p_gl1)
        {
            //gl1_bco = p_gl1->lValue(0,"BCO");
            triggervec = static_cast<uint64_t>( p_gl1->lValue(0,"TriggerVector") );
            triginput = static_cast<uint64_t>( p_gl1->lValue(0,"TriggerInput") );
            //std::cout << "trig " << std::hex << triggervec << "\t" << triginput << std::dec << std::endl;

            trigraw = static_cast<uint64_t>( p_gl1->lValue(0,"TriggerInput") );
            triglive = static_cast<uint64_t>( p_gl1->lValue(0,"LiveVector") );
            trigscaled = static_cast<uint64_t>( p_gl1->lValue(0,"ScaledVector") );

            triggervec = trigscaled;
            /*
            if( ((triggervec&mbdtrig)==0) && ((triggervec&zdcns)==0) ){
                // if no mbd or zdcns bit is set, then we use the live vector
                // to determine if this is a valid event
                triggervec = triglive;
                std::cout << "I am using the live vector for this event" << std::endl;
            }
            */

            /*
               std::cout << "TRIGS" << std::hex << std::endl;
               std::cout << "TrigInp\t" << std::setw(12) << triginput << std::endl;
               std::cout << "TrigVec\t" << std::setw(12) << triggervec << std::endl;
               std::cout << "RAW\t" << std::setw(12) << trigraw << std::endl;
               std::cout << "LIVE\t" << std::setw(12) << triglive << std::endl;
               std::cout << "SCALED\t" << std::setw(12) << trigscaled << std::endl;
               std::cout << "BUSY\t" << std::setw(12) << busy << std::endl;
               std::cout << std::dec << std::endl;
               */

            for (int itrig = 0; itrig < 64; itrig++ )
            {
                uint64_t trigbit = 0x1UL << itrig;
                if ( (triggervec&trigbit) != 0 )
                {
                    bbc_trigs->Fill( itrig );
                }
            }

            delete p_gl1;
        }
        delete gl1Event;
    }
  }
  else
  {
      // if we don't use GL1, set every trig bit true
      triggervec = std::numeric_limits<uint64_t>::max();
  }

  // calculate BBC
  bevt->Clear();
  bevt->SetRawData(evt,m_mbdpmts);

  if (bevt->calib_is_done() == 0)
  {
      return 0;
  }

  // Skip if this doesn't have a relevant trigger
  // (Can use any trigger for sampmax calib, in principle)
  if ( ((triggervec&trigmask) == 0UL) && (gl1badflag==0) )
  {
      if ( f_evt%1000 == 0 )
      {
          std::cout << "skipping " << f_evt << "\t" << std::hex << triggervec
              << "\t" << trigmask << std::dec << std::endl;
      }
      return 0;
  }

  bevt->Calculate(m_mbdpmts, m_mbdout);

  bbc_nevent_counter->Fill(1);
  double zvtx = m_mbdout->get_zvtx();
  double t0 = m_mbdout->get_t0();
  double qsum[2] = {0, 0};
  qsum[0] = m_mbdout->get_q(0);
  qsum[1] = m_mbdout->get_q(1);
  int south_nhits = m_mbdout->get_npmt(0);
  int north_nhits = m_mbdout->get_npmt(1);

  static int counter = 0;
  evtcnt++;
  if (counter < 10)
  {
      std::cout << "zt\t" << f_evt << "\t" << zvtx << "\t" << t0 << std::endl;
      counter++;
  }

  for ( int i = 0; i < TriggerEnum::NUM_MBD_TRIGGERS; i++ ){
    if ( (triggervec&TriggerEnum::MBTriggers[i]) != 0 )
    {
      bbc_last_update_ticker[i]++;
      if ( bbc_last_update_ticker[i] > zvtx_autoupdate_ticker ){
        for ( int ix = 0; ix < BbcMonDefs::zvtnbin; ix++ )
        {
          bbc_zvertex_autoupdate[i]->SetBinContent( ix+1, 0 ); // zero out the histogram
        }
        bbc_last_update_ticker[i] = 0;
      }
      bbc_zvertex_autoupdate[i]->Fill(zvtx);
    } // end of trigger check
  } // end of loop over triggers

  // vertex and t0
  //std::cout << "mbdns " << std::hex << mbdns << std::dec << std::endl;
  if ( (triggervec&mbdbest)!=0 )
  {
      bbc_nevent_counter->Fill(5);  // num BBCNS triggers

      bbc_zvertex->Fill(zvtx);
      bbc_south_nhit->Fill( south_nhits );
      bbc_north_nhit->Fill( north_nhits );

      if ( triginput&mbdnsvtx10 )
      {
          bbc_zvertex_10_chk->Fill(zvtx);
      }
      if ( triginput&mbdnsvtx30 )
      {
          bbc_zvertex_30_chk->Fill(zvtx);
      }
      if ( triginput&mbdnsvtx150 )
      {
          bbc_zvertex_60_chk->Fill(zvtx);
      }
  } 
  if ( (triggervec&mbdwidebest)!=0 )
  {
      bbc_zvertex_ns->Fill(zvtx);
      bbc_zvertex_short->Fill(zvtx);
  }
  // else if ( (triglive&mbdns)!=0 ) 
  // {
  //     bbc_nevent_counter->Fill(5);  // num BBCNS triggers

  //     bbc_zvertex->Fill(zvtx);
  //     bbc_zvertex_short->Fill(zvtx);
  //     bbc_zvertex_ns->Fill(zvtx);
  //     bbc_south_nhit->Fill( south_nhits );
  //     bbc_north_nhit->Fill( north_nhits );

  //     if ( triginput&mbdnsvtx10 )
  //     {
  //         bbc_zvertex_10_chk->Fill(zvtx);
  //     }
  //     if ( triginput&mbdnsvtx30 )
  //     {
  //         bbc_zvertex_30_chk->Fill(zvtx);
  //     }
  //     if ( triginput&mbdnsvtx150 )
  //     {
  //         bbc_zvertex_60_chk->Fill(zvtx);
  //     }

  // }

  if ( triggervec&mbdnsvtx10 )
  {
      bbc_zvertex_10->Fill(zvtx);
  }
  if ( triggervec&mbdnsvtx30 )
  {
      bbc_zvertex_30->Fill(zvtx);
  }
  if ( triggervec&mbdnsvtx150 )
  {
      bbc_zvertex_60->Fill(zvtx);
  }
  if ( triggervec&zdcns )
  {
      bbc_zvertex_zdcns->Fill(zvtx);
  }
  if ( triggervec&emcal )
  {
      bbc_zvertex_emcal->Fill(zvtx);
      bbc_nhit_emcal[0]->Fill( south_nhits );
      bbc_nhit_emcal[1]->Fill( north_nhits );
  }
  if ( triggervec&hcal )
  {
      bbc_zvertex_hcal->Fill(zvtx);
      bbc_nhit_hcal[0]->Fill( south_nhits );
      bbc_nhit_hcal[1]->Fill( north_nhits );
  }
  if ( triggervec&emcalmbd )
  {
      bbc_zvertex_emcalmbd->Fill(zvtx);
      bbc_nhit_emcalmbd[0]->Fill( south_nhits );
      bbc_nhit_emcalmbd[1]->Fill( north_nhits );
  }
  if ( triggervec&hcalmbd )
  {
      bbc_zvertex_hcalmbd->Fill(zvtx);
      bbc_nhit_hcalmbd[0]->Fill( south_nhits );
      bbc_nhit_hcalmbd[1]->Fill( north_nhits );
  }

  // now fill in histograms when gl1 bypass is requested
  if ( gl1badflag )
  {
      bbc_zvertex_ns->Fill(zvtx);
      bbc_zvertex_10->Fill(zvtx);
  }
  //with all triggers
  bbc_zvertex_alltrigger->Fill(zvtx);

  // only process for primary mbd or zdcns trigger
  if ( ((triggervec&mbdtrig) == 0) && ((triggervec&zdcns)==0) && (gl1badflag==0) )
  {
      return 0;
  }

  bbc_tzero_zvtx->Fill(zvtx, t0);

  //== Send zvtx to MCR
  if ( prev_send_time == 0 )
  {
      prev_send_time = time(0);
  }
  time_t timediff = time(0) - prev_send_time;

  // send the vtx at a min of 5 seconds of data, and when we have > 1000 events
  // or we always send at 60 seconds if the above two conditions are not satisfied
  if ( ((timediff > 5) && (bbc_zvertex_short->Integral() >= 1000)) || (timediff > 60) )
  {
      f_zvtx->SetRange(-75., 75.);
      f_zvtx->SetParameters(250, 0., 10);
      {
        bbc_zvertex_short->Fit(f_zvtx, "RNQ");

        // Report z-vertex mean and width
        Double_t mean = f_zvtx->GetParameter(1);
        Double_t rms = f_zvtx->GetParameter(2);
        // we should do a check of a good fit here (skip for now)

        std::ostringstream msg;
        msg << "MBD zvertex mean/width: " << mean << " " << rms;
        se->send_message(this, MSG_SOURCE_BBC, MSG_SEV_INFORMATIONAL, msg.str(), 1);
        std::cout << "MBD zvtx mean/width: " << mean << " " << rms << std::endl;

        if ( useGL1==1 && GetSendFlag() == 1 )
        {
          TString cmd = "/home/phnxrc/mbd/chiu/mbd_operations/httpRequestDemo.py -s sphenix.detector zMeanM "; cmd += mean;
          cmd += "; /home/phnxrc/mbd/chiu/mbd_operations/httpRequestDemo.py -s sphenix.detector zRmsM "; cmd += rms;
          gSystem->Exec( cmd );
        }

      }
      bbc_zvertex_short->Reset();
      prev_send_time = time(0);
  }

  for (int ipmt = 0; ipmt < 128; ipmt++)
  {
      float q = m_mbdpmts->get_pmt(ipmt)->get_q();
      bbc_adc->Fill(ipmt, q);

      // std::cout << f_evt << "\tipmt " << ipmt << "\t" << q << "\t";
      if (q > 0.5)
      {
          float tt = m_mbdpmts->get_pmt(ipmt)->get_time();
          if (ipmt < 64)
          {
              bbc_south_hittime->Fill(tt);
          }
          else
          {
              bbc_north_hittime->Fill(tt);
          }

          // std::cout << tq;
      }
      // std::cout << std::endl;
  }

  // charge
  bbc_south_chargesum->Fill(qsum[0]);
  bbc_north_chargesum->Fill(qsum[1]);

  // raw waveforms
  for (int ipmt = 0; ipmt < 128; ipmt++)
  {
      int tch = (ipmt / 8) * 16 + ipmt % 8;
      MbdSig *bbcsig = bevt->GetSig(tch);
      TGraphErrors *gwave = bbcsig->GetGraph();
      Int_t n = gwave->GetN();
      Double_t *x = gwave->GetX();
      Double_t *y = gwave->GetY();
      // make a threshold cut 
      for (int jsamp = 0; jsamp < n; jsamp++)
      {
          if ( y[jsamp]>20 )
          {
              for (int isamp = 0; isamp < n; isamp++)
              {
                  bbc_time_wave->Fill(x[isamp], ipmt, y[isamp]);
              }
              break;
          }
      }

      int qch = tch + 8;
      bbcsig = bevt->GetSig(qch);
      gwave = bbcsig->GetGraph();
      n = gwave->GetN();
      x = gwave->GetX();
      y = gwave->GetY();
      for (int isamp = 0; isamp < n; isamp++)
      {
          bbc_charge_wave->Fill(x[isamp], ipmt, y[isamp]);
      }

      // hit map
      float q = m_mbdpmts->get_pmt(ipmt)->get_q();

      if ( q>0. )
      {
          float xcent = _mbdgeom->get_x(ipmt);
          float ycent = _mbdgeom->get_y(ipmt);
          int arm = _mbdgeom->get_arm(ipmt);
          //std::cout << "q " << arm << "\t" << q << std::endl;
          if (arm == 0)
          {
              bbc_south_hitmap->Fill(xcent, ycent, q);
          }
          else if (arm == 1)
          {
              bbc_north_hitmap->Fill(xcent, ycent, q);
          }
      }
  }

  return 0;
}

int BbcMon::Reset()
{
    // reset our internal counters
    evtcnt = 0;
    // idummy = 0;

    bbc_south_nhit->Reset();
    bbc_north_nhit->Reset();
    for (int iarm=0; iarm<2; iarm++)
    {
        bbc_nhit_emcal[iarm]->Reset();
        bbc_nhit_hcal[iarm]->Reset();
        bbc_nhit_emcalmbd[iarm]->Reset();
        bbc_nhit_hcalmbd[iarm]->Reset();
    }
    bbc_adc->Reset();
    bbc_tdc->Reset();
    bbc_tdc_overflow->Reset();
    bbc_tdc_armhittime->Reset();
    bbc_nevent_counter->Reset();
    bbc_zvertex->Reset();
    bbc_zvertex_short->Reset();
    bbc_zvertex_ns->Reset();
    bbc_zvertex_10->Reset();
    bbc_zvertex_30->Reset();
    bbc_zvertex_60->Reset();
    bbc_zvertex_10_chk->Reset();
    bbc_zvertex_30_chk->Reset();
    bbc_zvertex_60_chk->Reset();
    bbc_zvertex_zdcns->Reset();
    bbc_zvertex_emcal->Reset();
    bbc_zvertex_hcal->Reset();
    bbc_zvertex_emcalmbd->Reset();
    bbc_zvertex_hcalmbd->Reset();
    bbc_tzero_zvtx->Reset();
    bbc_avr_hittime->Reset();
    bbc_south_hittime->Reset();
    bbc_north_hittime->Reset();
    bbc_south_chargesum->Reset();
    bbc_north_chargesum->Reset();
    bbc_prescale_hist->Reset();
    bbc_time_wave->Reset();
    bbc_charge_wave->Reset();
    for ( int i = 0; i < TriggerEnum::NUM_MBD_TRIGGERS; i++ ){
      bbc_zvertex_autoupdate[i]->Reset();
      bbc_last_update_ticker[i] = 0;
    } 

    return 0;
}

int BbcMon::DBVarInit()
{
    // variable names are not case sensitive
    /*
       std::string varname;
       varname = "bbcmoncount";
       dbvars->registerVar(varname);
       varname = "bbcmondummy";
       dbvars->registerVar(varname);
       varname = "bbcmonnew";
       dbvars->registerVar(varname);
       if (verbosity > 0)
       {
       dbvars->Print();
       }
       dbvars->DBInit();
       */
    return 0;
}

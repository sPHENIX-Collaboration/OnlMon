#include "InttMon.h"

#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <TH1.h>

InttMon::InttMon(const std::string &name)
  : OnlMon(name)
{
  plist = new Packet *[1];
  return;
}

InttMon::~InttMon()
{
  delete[] plist;
}

int InttMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();

  // histograms
  EvtHist = new TH1I("InttEvtHist", "InttEvtHist", 1, 0.0, 1.0);
  // 26*14
  HitHist = new TH1I("InttHitHist", "InttHitHist", (NFEES * NCHIPS), 0.0, 1.0);
  // 128*14
  BcoHist = new TH1I("InttBcoHist", "InttBcoHist", (NFEES * NBCOS), 0.0, 1.0);
  //...

  se->registerHisto(this, EvtHist);
  se->registerHisto(this, HitHist);
  se->registerHisto(this, BcoHist);
  //...

  return 0;
}

int InttMon::BeginRun(const int /* run_num */)
{
  return 0;
}

int InttMon::process_event(Event *evt)
{
  evt->getPacketList(plist, 1);
  if (plist[0])
  {
    for (int n = 0, N = plist[0]->iValue(0, "NR_HITS"); n < N; ++n)
    {
      int fee = plist[0]->iValue(n, "FEE");
      int chp = (plist[0]->iValue(n, "CHIP_ID") + 25) % 26;
      int bco = ((0x7f & plist[0]->lValue(n, "BCO")) - plist[0]->iValue(n, "FPHX_BCO") + 128) % 128;
      HitHist->AddBinContent(fee * NCHIPS + chp + 1);  // +1 to start at bin 1
      BcoHist->AddBinContent(fee * NBCOS + bco + 1);   // +1 to start at bin 1
    }
    delete plist[0];
	plist[0] = nullptr;
	// getPacketList doesn't set plist to an array of nullptrs if it fails
	// you just get the dangling pointer from the previous loop
  }

  EvtHist->AddBinContent(1);

  return 0;
}

int InttMon::Reset()
{
  return 0;
}

int InttMon::MiscDebug()
{
  for (int fee = 0; fee < 14; ++fee)
  {
    for (int chp = 0; chp < 26; ++chp)
    {
      HitHist->SetBinContent(fee * NCHIPS + chp + 1, chp);
    }
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    for (int bco = 0; bco < 128; ++bco)
    {
      BcoHist->SetBinContent(fee * NBCOS + bco + 1, fee);
    }
  }

  return 0;
}

//             Ladder Structure                //
//=============================================//
//      U14     U1   Ladder_z  Type B  North   //
//      U15     U2      .        .       .     //
//      U16     U3      3        .       .     //
//      U17     U4      .        .       .     //
//      U18     U5   Ladder_z  Type B    .     //
//------------------------------------   .     //
//      U19     U6   Ladder_z  Type A    .     //
//      U20     U7      .        .       .     //
//      U21     U8      .        .       .     //
//      U22     U9      2        .       .     //
//      U23     U10     .        .       .     //
//      U24     U11     .        .       .     //
//      U25     U12     .        .       .     //
//      U26     U13  Ladder_z  Type A  North   //
//---------------------------------------------//
//      U13     U26  Ladder_z  Type A  South   //
//      U12     U25     .        .       .     //
//      U11     U24     .        .       .     //
//      U10     U23     .        .       .     //
//      U9      U22     0        .       .     //
//      U8      U21     .        .       .     //
//      U7      U20     .        .       .     //
//      U6      U19  Ladder_z  Type A    .     //
//------------------------------------   .     //
//      U5      U18  Ladder_z  Type B    .     //
//      U4      U17     .        .       .     //
//      U3      U16     1        .       .     //
//      U2      U15     .        .       .     //
//      U1      U14  Ladder_z  Type B  South   //
//=============================================//

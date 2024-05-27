#include "InttMon.h"

InttMon::InttMon(const std::string &name)
  : OnlMon(name)
{
  return;
}

InttMon::~InttMon()
{
  delete dbvars;

  delete EvtHist;
  delete HitHist;
  delete BcoHist;
}

int InttMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();

  // dbvars
  dbvars = new OnlMonDB(ThisName);
  DBVarInit();

  // histograms
  EvtHist = new TH1D("InttEvtHist", "InttEvtHist", 1, 0.0, 1.0);
  // The helper binning functions return the index of the overflow bin when an invalid value is passed
  HitHist = new TH1D("InttHitHist", "InttHitHist", HitBin({-1, -1}) - 1, 0.0, 1.0);
  BcoHist = new TH1D("InttBcoHist", "InttBcoHist", BcoBin({-1, -1}) - 1, 0.0, 1.0);
  //...

  se->registerHisto(this, EvtHist);
  se->registerHisto(this, HitHist);
  se->registerHisto(this, BcoHist);
  //...

  // Read in calibrartion data from InttMonData.dat
  const char *inttcalib = getenv("INTTCALIB");
  if (!inttcalib)
  {
    std::cerr << "INTTCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(inttcalib) + "/" + "InttMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();

  Reset();

  return 0;
}

int InttMon::BeginRun(const int /* run_num */)
{
  return 0;
}

int InttMon::process_event(Event *evt)
{
  for (int pid = 3001; pid < 3009; ++pid)
  {
    Packet *p = evt->getPacket(pid);
    if (!p)
    {
      continue;
    }

    for (int n = 0, N = p->iValue(0, "NR_HITS"); n < N; ++n)
    {
      int fee = p->iValue(n, "FEE");
      int chp = (p->iValue(n, "CHIP_ID") + 25) % 26;
      int bco = ((0x7f & p->lValue(n, "BCO")) - p->iValue(n, "FPHX_BCO") + 128) % 128;

      HitHist->AddBinContent(HitBin({.fee = fee,
                                     .chp = chp}));

      BcoHist->AddBinContent(BcoBin({.fee = fee,
                                     .bco = bco}));
    }

    delete p;
  }

  EvtHist->AddBinContent(1);
  DBVarUpdate();

  return 0;
}

int InttMon::Reset()
{
  // reset our DBVars
  evtcnt = 0;

  // clear our histogram entries
  EvtHist->Reset();
  HitHist->Reset();
  BcoHist->Reset();

  return 0;
}

int InttMon::MiscDebug()
{
  for (int fee = 0; fee < 14; ++fee)
  {
    for (int chp = 0; chp < 26; ++chp)
    {
      HitHist->SetBinContent(HitBin({.fee = fee,
                                     .chp = chp}),
                             chp);
    }
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    for (int bco = 0; bco < 128; ++bco)
    {
      BcoHist->SetBinContent(BcoBin({.fee = fee,
                                     .bco = bco}),
                             fee);
    }
  }

  return 0;
}

int InttMon::DBVarInit()
{
  std::string var_name;

  var_name = "intt_evtcnt";
  dbvars->registerVar(var_name);

  dbvars->DBInit();

  return 0;
}

int InttMon::DBVarUpdate()
{
  dbvars->SetVar("intt_evtcnt", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);

  return 0;
}

int InttMon::HitBin(
    struct InttMon::HitData_s const& hit_data)
{
  int b = 1;
  int s = 1;

  if (0 < b && 0 <= hit_data.chp && hit_data.chp < 26)
  {
    b += hit_data.chp * s;
  }
  else
  {
    b = -1;
  }
  s *= 26;

  if (0 < b && 0 <= hit_data.fee && hit_data.fee < 14)
  {
    b += hit_data.fee * s;
  }
  else
  {
    b = -1;
  }
  s *= 14;

  // if(0 < b && 3001 <= hit_data.pid && hit_data.pid < 3009) {
  // 	b += (hit_data.pid - 3001) * s;
  // } else {
  // 	b = -1;
  // }
  // s *= 8;

  // return the bin index if the parameters are valid
  // otherwise, return the overflow bin index
  return (0 < b) ? b : ++s;
}

int InttMon::BcoBin(
    struct InttMon::BcoData_s const& bco_data)
{
  int b = 1;
  int s = 1;

  if (0 < b && 0 <= bco_data.bco && bco_data.bco < 128)
  {
    b += bco_data.bco * s;
  }
  else
  {
    b = -1;
  }
  s *= 128;

  if (0 < b && 0 <= bco_data.fee && bco_data.fee < 14)
  {
    b += bco_data.fee * s;
  }
  else
  {
    b = -1;
  }
  s *= 14;

  // if(0 < b && 3001 <= bco_data.pid && bco_data.pid < 3009) {
  // 	b += (bco_data.pid - 3001)  * s;
  // } else {
  // 	b = -1;
  // }
  // s *= 8;

  // return the bin index if the parameters are valid
  // otherwise, return the overflow bin index
  return (0 < b) ? b : ++s;
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

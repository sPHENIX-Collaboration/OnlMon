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

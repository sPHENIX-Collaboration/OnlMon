// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "TpotMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>

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

TpotMon::TpotMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new TpotMon()
  return;
}

int TpotMon::Init()
{
  // read our calibrations from TpotMonData.dat
  {
    std::string fullfile = std::string(getenv("TPOTCALIB")) + "/" + "TpotMonData.dat";
    std::ifstream calib(fullfile);
    calib.close();
  }
  auto se = OnlMonServer::instance();

  
  const auto detector_names = m_mapping.get_detnames_sphenix();
  for( size_t idet=0; idet<detector_names.size(); ++idet )
  {

    // local copy of detector name
    const auto& detector_name=detector_names[idet];

    // adc vs sample
    m_adc_vs_sample[idet] = new TH2I(
      Form( "m_adc_sample_%s", detector_name.c_str() ),
      Form( "adc count vs sample id (%s);sample id;adc", detector_name.c_str() ),
      350, 0, 350,
      1024, 0, 1024 );
    se->registerHisto(this, m_adc_vs_sample[idet]);

    // hit charge
    m_hit_charge[idet] = new TH1I(
      Form( "m_hit_charge_%s", detector_name.c_str() ),
      Form( "hit charge distribution (%s);adc", detector_name.c_str() ),
      1024, 0, 1024 );
    se->registerHisto(this, m_hit_charge[idet]);

    // hit multiplicity
    m_hit_multiplicity[idet] = new TH1I(
      Form( "m_hit_multiplicity_%s", detector_name.c_str() ),
      Form( "hit multiplicity (%s);#hits", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, m_hit_multiplicity[idet]);

    // hit per channel
    m_hit_vs_channel[idet] = new TH1I(
      Form( "m_hit_vs_channel_%s", detector_name.c_str() ),
      Form( "hit profile (%s);channel", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, m_hit_vs_channel[idet]);
  }

  // use monitor name for db table name
  dbvars.reset( new OnlMonDB(ThisName) );
  DBVarInit();
  Reset();
  return 0;
}

int TpotMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int TpotMon::process_event(Event * /* evt */)
{
  // increment event counter
  ++evtcnt;

  auto se = OnlMonServer::instance();
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

//   // get temporary pointers to histograms
//   // one can do in principle directly se->getHisto("tpothist1")->Fill()
//   // but the search in the histogram Map is somewhat expensive and slows
//   // things down if you make more than one operation on a histogram
//   tpothist1->Fill((float) idummy);
//   tpothist2->Fill((float) idummy, (float) idummy, 1.);
//   tpothist3->Fill((float) idummy);

  if (idummy++ > 10)
  {
    if (dbvars)
    {
      dbvars->SetVar("tpotmoncount", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("tpotmondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("tpotmonnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    std::ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
  }
  return 0;
}

int TpotMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int TpotMon::DBVarInit()
{
  // variable names are not case sensitive
  std::string varname;
  varname = "tpotmoncount";
  dbvars->registerVar(varname);
  varname = "tpotmondummy";
  dbvars->registerVar(varname);
  varname = "tpotmonnew";
  dbvars->registerVar(varname);
  if (verbosity > 0)
  {
    dbvars->Print();
  }
  dbvars->DBInit();
  return 0;
}

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
#include <TH2Poly.h>

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
  
//__________________________________________________
TpotMon::TpotMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new TpotMon()
}

//__________________________________________________
int TpotMon::Init()
{
  setup_tiles();
  
  // read our calibrations from TpotMonData.dat
  {
    std::string fullfile = std::string(getenv("TPOTCALIB")) + "/" + "TpotMonData.dat";
    std::ifstream calib(fullfile);
    calib.close();
  }
  auto se = OnlMonServer::instance();
  
  // global occupancy
  m_global_occupancy_phi = new TH2Poly;
  m_global_occupancy_phi->SetName( "m_global_occupancy_phi" );
  m_global_occupancy_phi->SetTitle( "occupancy; z (cm); x (cm)" );
  setup_bins(m_global_occupancy_phi);
  se->registerHisto(this, m_global_occupancy_phi);
  
  m_global_occupancy_z = new TH2Poly;
  m_global_occupancy_z->SetName( "m_global_occupancy_z" );
  m_global_occupancy_z->SetTitle( "occupancy; z (cm); x(cm)" );
  setup_bins(m_global_occupancy_z);
  se->registerHisto(this, m_global_occupancy_z);

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

//________________________________
int TpotMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

//________________________________
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

  for( const auto& point:m_tile_centers )
  {
    m_global_occupancy_phi->Fill(point.first, point.second);  
    m_global_occupancy_z->Fill(point.first, point.second);  
  }
  
  
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

//________________________________
int TpotMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

//________________________________
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

//________________________________
void TpotMon::setup_tiles()
{
  // clear previous tiles
  m_tile_centers.clear();

  /*
   * to convert sphenix coordinates into a x,y 2D histogram, 
   * we transform z(3D) = x(2D)
   * and x (3D) = y (2D)
   */

  {
    const double tile_x = 0;
    for( const double& tile_z:{ -84.6, -28.2, 28.2, 84.6 } )
    { m_tile_centers.push_back( {tile_z, tile_x} ); }
  }
    
  {
    // neighbor sectors have two modules, separated by 10cm
    for( const double& tile_x: { -m_tile_width - 2, m_tile_width+2 } )
      for( const double& tile_z:{ -37.1, 37.1 } )
    { m_tile_centers.push_back( {tile_z, tile_x} ); }
  }
  
}


//________________________________
void TpotMon::setup_bins(TH2Poly* h2)
{
  // loop over tile centers
  for( const auto& point:m_tile_centers )
  {
    const std::array<double,4> x = 
    {
      point.first-m_tile_length/2,
      point.first-m_tile_length/2,
      point.first+m_tile_length/2,
      point.first+m_tile_length/2
    };

    const std::array<double,4> y = 
    {
      point.second-m_tile_width/2,
      point.second+m_tile_width/2,
      point.second+m_tile_width/2,
      point.second-m_tile_width/2
    };
      
    h2->AddBin( 4, &x[0], &y[0] );
  }
}

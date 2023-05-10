// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "TpotMon.h"
#include "TpotMonDefs.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>
#include <Event/Event.h>

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
  m_det_index_map.clear();
  
  // read our calibrations from TpotMonData.dat
  {
    std::string fullfile = std::string(getenv("TPOTCALIB")) + "/" + "TpotMonData.dat";
    std::ifstream calib(fullfile);
    calib.close();
  }
  auto se = OnlMonServer::instance();
  
  // counters
  /* arbitrary counters. First bin is number of events */
  m_counters = new TH1I( "m_counters", "counters", 10, 0, 10 );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kEventCounter, "events" );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kValidEventCounter, "valid events" );
  se->registerHisto(this, m_counters);

  
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
  const auto fee_id_list = m_mapping.get_fee_id_list();
  for( size_t idet=0; idet<detector_names.size(); ++idet )
  {

    // local copy of detector name
    const auto& detector_name=detector_names[idet];
    
    // fill detector index map
    m_det_index_map.emplace( fee_id_list[idet],idet );
    
    // adc vs sample
    static constexpr int max_sample = 32;
    m_detector_histograms[idet].m_adc_vs_sample = new TH2I(
      Form( "m_adc_sample_%s", detector_name.c_str() ),
      Form( "adc count vs sample id (%s);sample id;adc", detector_name.c_str() ),
      max_sample, 0, max_sample,
      1024, 0, 1024 );
    se->registerHisto(this, m_detector_histograms[idet].m_adc_vs_sample);

    // hit charge
    static constexpr double max_hit_charge = 1024;
    m_detector_histograms[idet].m_hit_charge = new TH1I(
      Form( "m_hit_charge_%s", detector_name.c_str() ),
      Form( "hit charge distribution (%s);adc", detector_name.c_str() ),
      100, 0, max_hit_charge );
    se->registerHisto(this, m_detector_histograms[idet].m_hit_charge);

    // hit multiplicity
    m_detector_histograms[idet].m_hit_multiplicity = new TH1I(
      Form( "m_hit_multiplicity_%s", detector_name.c_str() ),
      Form( "hit multiplicity (%s);#hits", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, m_detector_histograms[idet].m_hit_multiplicity);

    // hit per channel
    m_detector_histograms[idet].m_hit_vs_channel = new TH1I(
      Form( "m_hit_vs_channel_%s", detector_name.c_str() ),
      Form( "hit profile (%s);channel", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, m_detector_histograms[idet].m_hit_vs_channel);
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
int TpotMon::process_event(Event* event)
{
  
  // increment by one a given bin number
  auto increment = []( TH1* h, int bin )
  { h->SetBinContent(bin, h->GetBinContent(bin)+1 ); };
  
  // increment total number of event
  increment( m_counters, TpotMonDefs::kEventCounter );
  
  // check event and event type
  if( !event ) { return 0; }
  if(event->getEvtType() >= 8) { return 0; }
  
  // increment total number of valid events
  ++evtcnt;
  
  increment( m_counters, TpotMonDefs::kValidEventCounter );
  
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

  // read the data
  auto packet = event->getPacket(MicromegasDefs::m_packet_id);
  if( !packet )
  {
    // no data
    std::cout << "TpotMon::process_event - event contains no TPOT data" << std::endl;
    return 0;
  }
    
  // hit multiplicity
  std::array<int,MicromegasDefs::m_nfee> multiplicity = {{0}};
  
  // get number of datasets (also call waveforms)
  const auto n_waveforms = packet->iValue(0, "NR_WF" );
  if( Verbosity() )
  { std::cout << "TpotMon::process_event - n_waveforms: " << n_waveforms << std::endl; }
  for( int i=0; i<n_waveforms; ++i )
  {
    auto channel = packet->iValue( i, "CHANNEL" );
    int fee_id = packet->iValue(i, "FEE" );
    int samples = packet->iValue( i, "SAMPLES" );

    // get detector index from fee id
    const auto iter = m_det_index_map.find( fee_id );
    if( iter == m_det_index_map.end() )
    {
      std::cout << "TpotMon::process_event - invalid fee_id: " << fee_id << std::endl;
      continue;
    }
    
    const auto& det_index = iter->second;
    
    if( Verbosity()>1 )
    {
      std::cout
        << "TpotMon::process_event -"
        << " waveform: " << i
        << " fee: " << fee_id
        << " channel: " << channel
        << " samples: " << samples
        << std::endl;
    }
       
    for( int is = 0; is < samples; ++is )
    {
      const auto adc =  packet->iValue( i, is );
      m_detector_histograms[det_index].m_adc_vs_sample->Fill( is, adc );
      m_detector_histograms[det_index].m_hit_charge->Fill( adc );
    }
    
    // update multiplicity for this detector
    ++multiplicity[det_index];
    
    // fill hit profile for this channel
    const auto strip_index = m_mapping.get_physical_strip(fee_id, channel );
    m_detector_histograms[det_index].m_hit_vs_channel->Fill( strip_index );
    
  }
  
  // fill hit multiplicities
  for( size_t idet = 0; idet < m_detector_histograms.size(); ++idet )
  { m_detector_histograms[idet].m_hit_multiplicity->Fill( multiplicity[idet] ); }
    

//   for( const auto& point:m_tile_centers )
//   {
//     m_global_occupancy_phi->Fill(point.first, point.second);  
//     m_global_occupancy_z->Fill(point.first, point.second);  
//   }
  
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

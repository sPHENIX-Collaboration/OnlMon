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
#include <memory>
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
{}

//__________________________________________________
int TpotMon::Init()
{
  // read our calibrations from TpotMonData.dat
  {
  const char *tpotcalib = getenv("TPOTCALIB");
  if (!tpotcalib)
  {
    std::cout << "TPOTCALIB environment variable not set" << std::endl;
    exit(1);
  }
    std::string fullfile = std::string(tpotcalib) + "/" + "TpotMonData.dat";
    std::ifstream calib(fullfile);
    calib.close();
  }
  auto se = OnlMonServer::instance();
  
  // map tile centers to fee id
  const auto fee_id_list = m_mapping.get_fee_id_list();
  for( const auto& fee_id:fee_id_list )
  {
    const auto tile_id = MicromegasDefs::getTileId( m_mapping.get_hitsetkey(fee_id));
    m_tile_centers.emplace(fee_id, m_geometry.get_tile_center(tile_id)); 
  }
  
  // counters
  /* arbitrary counters. First bin is number of events */
  m_counters = new TH1I( "m_counters", "counters", 10, 0, 10 );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kEventCounter, "events" );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kValidEventCounter, "valid events" );
  se->registerHisto(this, m_counters);
  
  // global occupancy
  m_detector_multiplicity_phi = new TH2Poly( "m_detector_multiplicity_phi", "multiplicity (#phi); z (cm); x (cm)", -120, 120, -60, 60 );
  m_detector_occupancy_phi = new TH2Poly( "m_detector_occupancy_phi", "occupancy (#phi); z (cm); x (cm)", -120, 120, -60, 60 );
  se->registerHisto(this, m_detector_occupancy_phi);
  
  m_detector_multiplicity_z = new TH2Poly( "m_detector_multiplicity_z", "multiplicity (z); z (cm); x (cm)", -120, 120, -60, 60 );
  m_detector_occupancy_z = new TH2Poly( "m_detector_occupancy_z", "occupancy (z); z (cm); x(cm)", -120, 120, -60, 60 );
  se->registerHisto(this, m_detector_occupancy_z);

  // setup bins
  for( auto&& h:{m_detector_multiplicity_phi, m_detector_occupancy_phi, m_detector_multiplicity_z, m_detector_occupancy_z } )
  { 
    setup_detector_bins( h ); 
    h->GetXaxis()->SetTitleOffset(1);
    h->GetYaxis()->SetTitleOffset(0.65);
  }
  
  for( const auto& fee_id:fee_id_list )
  {
    // local copy of detector name
    const auto& detector_name=m_mapping.get_detname_sphenix(fee_id);
    
    detector_histograms_t detector_histograms;
    
    // adc vs sample
    static constexpr int max_sample = 32;
    auto h = detector_histograms.m_adc_vs_sample = new TH2I(
      Form( "m_adc_sample_%s", detector_name.c_str() ),
      Form( "adc count vs sample (%s);sample;adc", detector_name.c_str() ),
      max_sample, 0, max_sample,
      1024, 0, 1024 );
    h->GetXaxis()->SetTitleOffset(1.);
    h->GetYaxis()->SetTitleOffset(1.65);
    se->registerHisto(this, detector_histograms.m_adc_vs_sample);

    // hit charge
    static constexpr double max_hit_charge = 1024;
    detector_histograms.m_hit_charge = new TH1I(
      Form( "m_hit_charge_%s", detector_name.c_str() ),
      Form( "hit charge distribution (%s);adc", detector_name.c_str() ),
      100, 0, max_hit_charge );
    se->registerHisto(this, detector_histograms.m_hit_charge);

    // hit multiplicity
    detector_histograms.m_hit_multiplicity = new TH1I(
      Form( "m_hit_multiplicity_%s", detector_name.c_str() ),
      Form( "hit multiplicity (%s);#hits", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, detector_histograms.m_hit_multiplicity);

    // hit per channel
    detector_histograms.m_hit_vs_channel = new TH1I(
      Form( "m_hit_vs_channel_%s", detector_name.c_str() ),
      Form( "hit profile (%s);channel", detector_name.c_str() ),
      256, 0, 256 );
    se->registerHisto(this, detector_histograms.m_hit_vs_channel);

    // store in map
    m_detector_histograms.emplace( fee_id, std::move( detector_histograms ) );
    
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
  std::unique_ptr<Packet> packet(event->getPacket(MicromegasDefs::m_packet_id));
  if( !packet )
  {
    // no data
    std::cout << "TpotMon::process_event - event contains no TPOT data" << std::endl;
    return 0;
  }
    
  // hit multiplicity vs fee id
  std::map<int, int> multiplicity;
  
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
    const auto iter = m_detector_histograms.find( fee_id );
    if( iter == m_detector_histograms.end() )
    {
      std::cout << "TpotMon::process_event - invalid fee_id: " << fee_id << std::endl;
      continue;
    }
    const auto& detector_histograms = iter->second;

    // get tile center, segmentation
    const auto& [tile_x, tile_y]  = m_tile_centers.at(fee_id);
    const auto segmentation = MicromegasDefs::getSegmentationType( m_mapping.get_hitsetkey(fee_id));

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
      detector_histograms.m_adc_vs_sample->Fill( is, adc );
      detector_histograms.m_hit_charge->Fill( adc );
    }
    
    // update multiplicity for this detector
    ++multiplicity[fee_id];
    
    // fill detector multiplicity
    switch( segmentation )
    {
      case MicromegasDefs::SegmentationType::SEGMENTATION_Z: 
      m_detector_multiplicity_z->Fill( tile_x, tile_y );
      break;
    
      case MicromegasDefs::SegmentationType::SEGMENTATION_PHI: 
      m_detector_multiplicity_phi->Fill( tile_x, tile_y );
      break;
    }

    // fill hit profile for this channel
    const auto strip_index = m_mapping.get_physical_strip(fee_id, channel );
    detector_histograms.m_hit_vs_channel->Fill( strip_index );
    
  }
  
  // fill hit multiplicities
  for( auto&& [fee_id, detector_histograms]:m_detector_histograms )
  { detector_histograms.m_hit_multiplicity->Fill( multiplicity[fee_id] ); }
      
  // convert multiplicity histogram in occupancy
  for( const auto& [fee_id, tile_center]:m_tile_centers )
  {
    const auto& [tile_x,tile_y] = tile_center;
    auto bin = m_detector_multiplicity_z->FindBin( tile_x, tile_y );
    m_detector_occupancy_z->SetBinContent( bin, m_detector_multiplicity_z->GetBinContent( bin )/(evtcnt*MicromegasDefs::m_nchannels_fee));
    m_detector_occupancy_phi->SetBinContent( bin, m_detector_multiplicity_phi->GetBinContent( bin )/(evtcnt*MicromegasDefs::m_nchannels_fee));
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
void TpotMon::setup_detector_bins(TH2Poly* h2)
{
  
  // get first member of pairs into a list
  auto get_x = []( const MicromegasGeometry::point_list_t& point_list )
  {
    std::vector<double> out;
    std::transform( point_list.begin(), point_list.end(), std::back_inserter( out ), []( const auto& p ) { return p.first; } );
    return out;
  };
  
  // get second member of pairs into a list
  auto get_y = []( const MicromegasGeometry::point_list_t& point_list )
  {
    std::vector<double> out;
    std::transform( point_list.begin(), point_list.end(), std::back_inserter( out ), []( const auto& p ) { return p.second; } );
    return out;
  };
  
  // loop over tile centers
  for( size_t i = 0; i < m_geometry.get_ntiles(); ++i )
  {
    const auto boundaries = m_geometry.get_tile_boundaries( i );
    h2->AddBin( boundaries.size(), &get_x( boundaries )[0], &get_y( boundaries )[0] );
  }
}

#include "TpotMon.h"
#include "TpotMonDefs.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>
#include <Event/Event.h>

#include <micromegas/MicromegasDefs.h>

#include <TH1.h>
#include <TH2.h>
#include <TH2Poly.h>

#include <cmath>
#include <cstdio>  // for printf
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>  // for allocator, string, char_traits

namespace
{

  enum
  {
    TRGMESSAGE = 1,
    FILLMESSAGE = 2
  };

  //! number of active packets used by TPOT.
  /** This is used to properly normalize the number of trigger received */
  static constexpr int m_npackets_active = 2;

  // get first member of pairs into a list
  std::vector<double> get_x( const MicromegasGeometry::point_list_t& point_list )
  {
    std::vector<double> out;
    std::transform( point_list.begin(), point_list.end(), std::back_inserter( out ), []( const auto& p ) { return p.first; } );
    return out;
  }

  // get second member of pairs into a list
  std::vector<double> get_y( const MicromegasGeometry::point_list_t& point_list )
  {
    std::vector<double> out;
    std::transform( point_list.begin(), point_list.end(), std::back_inserter( out ), []( const auto& p ) { return p.second; } );
    return out;
  }

  // streamer for sample window
  std::ostream& operator << ( std::ostream&o, const TpotMon::sample_window_t& window )
  {
    o << "{ " << window.first << ", " << window.second << "}";
    return o;
  }

}

//__________________________________________________
TpotMon::TpotMon(const std::string &name)
  : OnlMon(name)
{
  // setup default calibration filename
  // note: this can be overriden by calling set_calibration_filename from the parent macro
  const auto tpotcalib = getenv("TPOTCALIB");
  if (!tpotcalib)
  {
    std::cout << "TpotMon::TpotMon - TPOTCALIB environment variable not set" << std::endl;
    exit(1);
  }

  m_calibration_filename = std::string(tpotcalib) + "/" + "TPOT_Pedestal-000.root";
}

//__________________________________________________
int TpotMon::Init()
{

  if( Verbosity() )
  {
    std::cout << "TpotMon::Init - m_calibration_filename: " << m_calibration_filename << std::endl;
    std::cout << "TpotMon::Init - m_max_sample: " << m_max_sample << std::endl;
    std::cout << "TpotMon::Init - m_sample_window_signal: " << m_sample_window_signal << std::endl;
    std::cout << "TpotMon::Init - m_n_sigma: " << m_n_sigma << std::endl;
  }

  // setup calibrations
  if( std::ifstream( m_calibration_filename.c_str() ).good() )
  {
    m_calibration_data.read( m_calibration_filename );
  } else {
    std::cout << "TpotMon::Init -"
      << " file " << m_calibration_filename << " cannot be opened."
      << " No calibration loaded"
      << std::endl;
  }

  // server instance
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
  m_counters = new TH1F( "m_counters", "counters", 10, 0, 10 );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kEventCounter, "RCDAQ frame" );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kValidEventCounter, "valid RCDAQ frames" );
  m_counters->GetXaxis()->SetBinLabel(TpotMonDefs::kTriggerCounter, "triggers" );
  se->registerHisto(this, m_counters);

  // global occupancy
  m_detector_multiplicity_phi = new TH2Poly( "m_detector_multiplicity_phi", "multiplicity (#phi); z (cm); x (cm)", -120, 120, -60, 60 );
  m_detector_occupancy_phi = new TH2Poly( "m_detector_occupancy_phi", "occupancy (#phi); z (cm); x (cm);occupancy (%)", -120, 120, -60, 60 );
  se->registerHisto(this, m_detector_occupancy_phi);

  m_detector_multiplicity_z = new TH2Poly( "m_detector_multiplicity_z", "multiplicity (z); z (cm); x (cm)", -120, 120, -60, 60 );
  m_detector_occupancy_z = new TH2Poly( "m_detector_occupancy_z", "occupancy (z); z (cm); x(cm);occupancy (%)", -120, 120, -60, 60 );
  se->registerHisto(this, m_detector_occupancy_z);

  // setup bins
  for( auto&& h:{m_detector_multiplicity_phi, m_detector_occupancy_phi, m_detector_multiplicity_z, m_detector_occupancy_z } )
  {
    setup_detector_bins( h );
    h->SetMinimum(0);
  }

  // resist region occupancy
  m_resist_multiplicity_phi = new TH2Poly( "m_resist_multiplicity_phi", "multiplicity (#phi); z (cm); x (cm)", -120, 120, -60, 60 );
  m_resist_occupancy_phi = new TH2Poly( "m_resist_occupancy_phi", "occupancy (#phi); z (cm); x (cm);occupancy (%)", -120, 120, -60, 60 );
  se->registerHisto(this, m_resist_occupancy_phi);

  m_resist_multiplicity_z = new TH2Poly( "m_resist_multiplicity_z", "multiplicity (z); z (cm); x (cm)", -120, 120, -60, 60 );
  m_resist_occupancy_z = new TH2Poly( "m_resist_occupancy_z", "occupancy (z); z (cm); x(cm);occupancy (%)", -120, 120, -60, 60 );
  se->registerHisto(this, m_resist_occupancy_z);

  // setup bins
  for( auto&& h:{m_resist_multiplicity_z, m_resist_occupancy_z } )
  {
    setup_resist_bins( h, MicromegasDefs::SegmentationType::SEGMENTATION_Z );
    h->SetMinimum(0);
  }

  for( auto&& h:{m_resist_multiplicity_phi, m_resist_occupancy_phi } )
  {
    setup_resist_bins( h, MicromegasDefs::SegmentationType::SEGMENTATION_PHI );
    h->SetMinimum(0);
  }

  for( const auto& fee_id:fee_id_list )
  {
    // local copy of detector name
    const auto& detector_name=m_mapping.get_detname_sphenix(fee_id);

    detector_histograms_t detector_histograms;

    detector_histograms.m_counts_sample = new TH1I(
      Form( "m_counts_sample_%s", detector_name.c_str() ),
      Form( "hit count vs sample (%s);sample;counts", detector_name.c_str() ),
      m_max_sample, 0, m_max_sample );
    se->registerHisto(this, detector_histograms.m_counts_sample);

    static constexpr int max_adc = 1100;
    detector_histograms.m_adc_sample = new TH2I(
      Form( "m_adc_sample_%s", detector_name.c_str() ),
      Form( "adc count vs sample (%s);sample;adc", detector_name.c_str() ),
      m_max_sample, 0, m_max_sample,
      max_adc, 0, max_adc );
    se->registerHisto(this, detector_histograms.m_adc_sample);

    detector_histograms.m_adc_channel = new TH2I(
      Form( "m_adc_channel_%s", detector_name.c_str() ),
      Form( "adc count vs strip (%s);strip;adc", detector_name.c_str() ),
      MicromegasDefs::m_nchannels_fee, 0, MicromegasDefs::m_nchannels_fee,
      max_adc, 0, max_adc );
    se->registerHisto(this, detector_histograms.m_adc_channel);

    // hit charge
    static constexpr double max_hit_charge = 1100;
    detector_histograms.m_hit_charge = new TH1I(
      Form( "m_hit_charge_%s", detector_name.c_str() ),
      Form( "hit charge distribution (%s);adc", detector_name.c_str() ),
      100, 0, max_hit_charge );
    se->registerHisto(this, detector_histograms.m_hit_charge);

    // hit multiplicity
    detector_histograms.m_hit_multiplicity = new TH1I(
      Form( "m_hit_multiplicity_%s", detector_name.c_str() ),
      Form( "hit multiplicity (%s);#hits", detector_name.c_str() ),
      MicromegasDefs::m_nchannels_fee+5, 0, MicromegasDefs::m_nchannels_fee+5 );
    se->registerHisto(this, detector_histograms.m_hit_multiplicity);

    // hit per channel
    detector_histograms.m_hit_vs_channel = new TH1F(
      Form( "m_hit_vs_channel_%s", detector_name.c_str() ),
      Form( "hit profile (%s);strip", detector_name.c_str() ),
      MicromegasDefs::m_nchannels_fee, 0, MicromegasDefs::m_nchannels_fee );
    se->registerHisto(this, detector_histograms.m_hit_vs_channel);

    // store in map
    m_detector_histograms.emplace( fee_id, std::move( detector_histograms ) );

  }

  // use monitor name for db table name
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

  // increment a given bin number by weight
  auto increment = []( TH1* h, int bin, double weight = 1.0 )
  { h->SetBinContent(bin, h->GetBinContent(bin)+weight ); };

  // increment total number of event
  increment( m_counters, TpotMonDefs::kEventCounter );

  // check event and event type
  if( !event ) { return 0; }
  if(event->getEvtType() >= 8) { return 0; }

  // increment total number of valid events
  ++m_evtcnt;

  increment( m_counters, TpotMonDefs::kValidEventCounter );

  // hit multiplicity vs fee id
  std::map<int, int> multiplicity;

  // read the data
  for( const auto& packet_id:MicromegasDefs::m_packet_ids )
  {
    std::unique_ptr<Packet> packet(event->getPacket(packet_id));
    if( !packet )
    {
      // no data
      if( Verbosity() )
      { std::cout << "TpotMon::process_event - packet " << packet_id << " not found" << std::endl; }
      continue;
    }

    // get number of lvl1 tagger
    const int n_tagger = packet->lValue(0, "N_TAGGER");
    int n_lvl1_tagger = 0;
    for (int t = 0; t < n_tagger; t++)
    {
      const bool is_lvl1_tagger( static_cast<uint8_t>(packet->lValue(t, "IS_LEVEL1_TRIGGER" )));
      if( is_lvl1_tagger ) ++n_lvl1_tagger;
    }

    // increment counter
    increment( m_counters, TpotMonDefs::kTriggerCounter, double(n_lvl1_tagger)/m_npackets_active );
    m_triggercnt += double(n_lvl1_tagger)/m_npackets_active;

    // get number of datasets (also call waveforms)
    const auto n_waveforms = packet->iValue(0, "NR_WF" );

    if( Verbosity() )
    { std::cout << "TpotMon::process_event - n_waveforms: " << n_waveforms << std::endl; }
    for( int i=0; i<n_waveforms; ++i )
    {
      const auto channel = packet->iValue( i, "CHANNEL" );

      // bound check channel
      if( channel < 0 || channel >= MicromegasDefs::m_nchannels_fee )
      {
        if( Verbosity() )
        { std::cout << "TpotMon::process_event - invalid channel: " << channel << std::endl; }
        continue;
      }

      const int fee_id = packet->iValue(i, "FEE" );
      const auto strip_index = m_mapping.get_physical_strip(fee_id, channel );
      const int samples = packet->iValue( i, "SAMPLES" );

      // get channel rms and pedestal from calibration data
      const double pedestal = m_calibration_data.get_pedestal( fee_id, channel );
      const double rms = m_calibration_data.get_rms( fee_id, channel );

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
        const uint16_t adc =  packet->iValue( i, is );
        if( adc == MicromegasDefs::m_adc_invalid ) continue;
        const bool is_signal = rms>0 && (adc > m_min_adc) && (adc> pedestal+m_n_sigma*rms);
        if( is_signal ) detector_histograms.m_counts_sample->Fill( is );
        detector_histograms.m_adc_sample->Fill( is, adc );
        detector_histograms.m_hit_charge->Fill( adc );

        detector_histograms.m_adc_channel->Fill( strip_index, adc );

      }

      // define if hit is signal
      bool is_signal = false;
      for( int is = std::max<int>(0,m_sample_window_signal.first); is < std::min<int>(samples,m_sample_window_signal.second); ++is )
      {
        const uint16_t adc =  packet->iValue( i, is );
        if( adc == MicromegasDefs::m_adc_invalid ) continue;
        if( rms>0 && (adc > m_min_adc) && (adc>pedestal + m_n_sigma*rms) )
        {
          is_signal = true;
          break;
        }
      }

      // fill hit profile for this channel
      if( is_signal )
      {
        detector_histograms.m_hit_vs_channel->Fill( strip_index );

        // update multiplicity for this detector
        ++multiplicity[fee_id];

        // fill detector multiplicity
        switch( segmentation )
        {
          case MicromegasDefs::SegmentationType::SEGMENTATION_Z:
          m_detector_multiplicity_z->Fill( tile_x, tile_y );
          m_resist_multiplicity_z->Fill( tile_x + MicromegasGeometry::m_tile_length*( double(strip_index)/MicromegasDefs::m_nchannels_fee - 0.5), tile_y );
          break;

          case MicromegasDefs::SegmentationType::SEGMENTATION_PHI:
          m_detector_multiplicity_phi->Fill( tile_x, tile_y );
          m_resist_multiplicity_phi->Fill( tile_x, tile_y + MicromegasGeometry::m_tile_width*( double(strip_index)/MicromegasDefs::m_nchannels_fee - 0.5) );
          break;
        }
      }
    }
  }

  // fill hit multiplicities
  for( auto&& [fee_id, detector_histograms]:m_detector_histograms )
  { detector_histograms.m_hit_multiplicity->Fill( multiplicity[fee_id] ); }

  // convert multiplicity histogram into occupancy
  auto copy_content = []( TH2Poly* source, TH2Poly* destination, double scale )
  {
    for( int bin = 0; bin < source->GetNumberOfBins(); ++bin )
    { destination->SetBinContent( bin+1, source->GetBinContent( bin+1 )*scale ); }
  };

  copy_content( m_detector_multiplicity_z, m_detector_occupancy_z, 100./(m_triggercnt*MicromegasDefs::m_nchannels_fee) );
  copy_content( m_detector_multiplicity_phi, m_detector_occupancy_phi, 100./(m_triggercnt*MicromegasDefs::m_nchannels_fee) );
  copy_content( m_resist_multiplicity_z, m_resist_occupancy_z, 400./(m_triggercnt*MicromegasDefs::m_nchannels_fee) );
  copy_content( m_resist_multiplicity_phi, m_resist_occupancy_phi, 400./(m_triggercnt*MicromegasDefs::m_nchannels_fee) );

  return 0;
}

//________________________________
int TpotMon::Reset()
{
  // reset our internal counters
  m_evtcnt = 0;
  m_triggercnt = 0;

  // reset all histograms
  for( TH1* h:{
    m_detector_multiplicity_z, m_detector_multiplicity_phi,
    m_detector_occupancy_z, m_detector_occupancy_phi,
    m_resist_multiplicity_z, m_resist_multiplicity_phi,
    m_resist_occupancy_z, m_resist_occupancy_phi } )
  { h->Reset(); }

  for( const auto& [fee_id,hlist]:m_detector_histograms )
  {
    for( TH1* h:std::initializer_list<TH1*>{hlist.m_counts_sample, hlist.m_adc_sample, hlist.m_adc_channel, hlist.m_hit_charge,  hlist.m_hit_multiplicity,   hlist.m_hit_vs_channel } )
    { h->Reset(); }
  }

  return 0;
}

//________________________________
void TpotMon::setup_detector_bins(TH2Poly* h2)
{
  // loop over tile centers
  for( size_t i = 0; i < m_geometry.get_ntiles(); ++i )
  {
    const auto boundaries = m_geometry.get_tile_boundaries( i );
    h2->AddBin( boundaries.size(), &get_x( boundaries )[0], &get_y( boundaries )[0] );
  }
}

//________________________________
void TpotMon::setup_resist_bins(TH2Poly* h2, MicromegasDefs::SegmentationType segmentation)
{
  // loop over tile centers
  for( size_t itile = 0; itile < m_geometry.get_ntiles(); ++itile )
  {
    for( size_t iresist = 0; iresist < MicromegasGeometry::m_nresist; ++iresist )
    {
      const auto boundaries = m_geometry.get_resist_boundaries( itile, iresist, segmentation );
      h2->AddBin( boundaries.size(), &get_x( boundaries )[0], &get_y( boundaries )[0] );
    }
  }
}

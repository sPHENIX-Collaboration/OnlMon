#include "CommonFuncs.C"
#include <onlmon/tpot/TpotMonDraw.h>
#include <onlmon/OnlMonClient.h>

#include <array>
#include <fstream>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpotmon_client.so)

void tpotDrawInit(const int online = 0)
{
  // client
  auto cl = OnlMonClient::instance();

  // create drawing object and register
  auto tpotmon = new TpotMonDraw("TPOT");

  // prefer local calibration filename if exists
  const std::string local_calibration_filename( "TPOT_Pedestal-000.root" );
  if( std::ifstream( local_calibration_filename ).good() )
  { tpotmon->set_calibration_file( local_calibration_filename ); }

  tpotmon->set_sample_window( {0, 50} );
  tpotmon->set_sample_window_signal( {5, 20} );

  cl->registerDrawer(tpotmon);

  // get detector names
  const auto detector_names = tpotmon->get_detnames_sphenix();

  // register histograms
  for( const std::string& hname: { "m_counters", "m_detector_occupancy_phi", "m_detector_occupancy_z", "m_resist_occupancy_phi", "m_resist_occupancy_z" } )
  { cl->registerHisto( hname, "TPOTMON_0" ); }

  for( const std::string& hname: { "m_adc_sample", "m_adc_channel", "m_counts_sample", "m_hit_charge", "m_hit_multiplicity", "m_hit_vs_channel" } )
  {
    for( const auto& detname : detector_names )
    { cl->registerHisto( hname+"_"+detname, "TPOTMON_0" ); }
  }

  // for local host, just call tpotDrawInit(2)
  CreateSubsysHostlist("tpot_hosts.list", online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("TPOTMON_0", 1);
}

void tpotDraw(const char *what = "ALL")
{
  auto cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TPOTMON_0",1);
  cl->Draw("TPOT", what);
}

void tpotSavePlot()
{ OnlMonClient::instance()->SavePlot("TPOT"); }

void tpotHtml()
{ OnlMonClient::instance()->MakeHtml("TPOT"); }

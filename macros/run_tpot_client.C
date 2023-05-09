#include <CommonFuncs.C>
#include <onlmon/tpot/TpotMonDraw.h>
#include <onlmon/tpot/MicromegasMapping.h>
#include <onlmon/OnlMonClient.h>

#include <array>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpotmon_client.so)

void tpotDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  MicromegasMapping mapping;
  const auto detector_names = mapping.get_detnames_sphenix();
  
  for( const std::string& hname: { "m_adc_sample", "m_hit_charge", "m_hit_multiplicity", "m_hit_vs_channel" } )
  {
    for( const auto& detname : detector_names )
    { cl->registerHisto( hname+"_"+detname, "TPOTMON_0" ); }
  }
  
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("TPOTMON_0", 1);
  OnlMonDraw *tpotmon = new TpotMonDraw("TPOTMONDRAW");  // create Drawing Object
  cl->registerDrawer(tpotmon);              // register with client framework
}

void tpotDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("TPOTMON_0");       // update histos
  cl->Draw("TPOTMONDRAW", what);                    // Draw Histos of registered Drawers
}

void tpotPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("TPOTMONDRAW");                        // Create PS files
  return;
}

void tpotHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("TPOTMONDRAW");                      // Create html output
  return;
}

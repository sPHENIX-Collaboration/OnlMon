#define ONLINE

#include "CommonFuncs.C"

#include <onlmon/bbc/BbcMonDraw.h>

#include <onlmon/OnlMonClient.h>

#include <onlmon/triggerEnum.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlbbcmon_client.so)

// 0 is default (looks for things online)
// 1 is online (shift crew)
// 2 localhost
void bbcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  //  cl->Verbosity(100);
  OnlMonDraw *bbcmon = new BbcMonDraw("BBCMONDRAW");  // create Drawing Object
  std::string servername = "BBCMON_0";
  bbcmon->AddServer(servername);
  // south/north to keep them together
  cl->registerHisto("bbc_north_chargesum", servername);
  cl->registerHisto("bbc_south_chargesum", servername);
  cl->registerHisto("bbc_north_hitmap", servername);
  cl->registerHisto("bbc_south_hitmap", servername);
  cl->registerHisto("bbc_north_hittime", servername);
  cl->registerHisto("bbc_south_hittime", servername);
  cl->registerHisto("bbc_north_nhit", servername);
  cl->registerHisto("bbc_south_nhit", servername);
  // all others alphabetically
  cl->registerHisto("bbc_adc", servername);
  cl->registerHisto("bbc_avr_hittime", servername);
  cl->registerHisto("bbc_charge_wave", servername);
  cl->registerHisto("bbc_nevent_counter", servername);
  cl->registerHisto("bbc_nhit_emcal0", servername);
  cl->registerHisto("bbc_nhit_hcal0", servername);
  cl->registerHisto("bbc_nhit_emcalmbd0", servername);
  cl->registerHisto("bbc_nhit_hcalmbd0", servername);
  cl->registerHisto("bbc_nhit_emcal1", servername);
  cl->registerHisto("bbc_nhit_hcal1", servername);
  cl->registerHisto("bbc_nhit_emcalmbd1", servername);
  cl->registerHisto("bbc_nhit_hcalmbd1", servername);
  cl->registerHisto("bbc_prescale_hist", servername);
  cl->registerHisto("bbc_tdc", servername);
  cl->registerHisto("bbc_tdc_overflow", servername);
  cl->registerHisto("bbc_tdc_armhittime", servername);
  cl->registerHisto("bbc_time_wave", servername);
  cl->registerHisto("bbc_trigs", servername);
  cl->registerHisto("bbc_tzero_zvtx", servername);
  cl->registerHisto("bbc_zvertex", servername);
  cl->registerHisto("bbc_zvertex_alltrigger", servername);
  cl->registerHisto("bbc_zvertex_ns", servername);
  cl->registerHisto("bbc_zvertex_10", servername);
  cl->registerHisto("bbc_zvertex_30", servername);
  cl->registerHisto("bbc_zvertex_60", servername);
  cl->registerHisto("bbc_zvertex_ns_chk", servername);
  cl->registerHisto("bbc_zvertex_10_chk", servername);
  cl->registerHisto("bbc_zvertex_30_chk", servername);
  cl->registerHisto("bbc_zvertex_60_chk", servername);
  cl->registerHisto("bbc_zvertex_zdcns", servername);
  cl->registerHisto("bbc_zvertex_emcal", servername);
  cl->registerHisto("bbc_zvertex_hcal", servername);
  cl->registerHisto("bbc_zvertex_emcalmbd", servername);
  cl->registerHisto("bbc_zvertex_hcalmbd", servername);
  cl->registerHisto("bbc_runvtx", servername);
  cl->registerHisto("bbc_runvtxerr", servername);
  cl->registerHisto("bbc_runvtxtime", servername);
  for ( int i = 0; i < TriggerEnum::NUM_MBD_TRIGGERS; i++ ){
      std::string name = Form("bbc_zvertex_autoupdate_%i", i);
      cl->registerHisto(name, servername);
  }
  for(int i = 0; i < 6; i++)cl->registerHisto(Form("h1_packet_status_%d",i),servername.c_str());


  // for local host, just call bbcDrawInit(2)
  CreateSubsysHostlist("bbc_hosts.list", online);

  // says I know they are all on the same node
  cl->requestHistoBySubSystem(servername, 1);
  cl->registerDrawer(bbcmon);             // register with client framework
}

void bbcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  OnlMonDraw *bbcdraw = cl->GetDrawer("BBCMONDRAW");
  for (auto iter = bbcdraw->ServerBegin(); iter != bbcdraw->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("BBCMONDRAW", what);                     // Draw Histos of registered Drawers
}

void bbcSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("BBCMONDRAW");                         // Save Plots
  return;
}

void bbcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("BBCMONDRAW");                       // Create html output
  return;
}

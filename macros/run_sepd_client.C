#include "CommonFuncs.C"

#include <onlmon/sepd/SepdMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlsepdmon_client.so)

std::string DrawerName="SEPDMONDRAW";

void sepdDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  // first histogram uses the TH1->GetName() as key
  OnlMonDraw *sepdmon = new SepdMonDraw(DrawerName);  // create Drawing Object
  std::string servername = "SEPDMON_0";
  sepdmon->AddServer(servername);

  cl->registerHisto("h_ADC_corr", servername);
  cl->registerHisto("h_hits_corr", servername);
  cl->registerHisto("h_ADC_all_channel", servername);
  cl->registerHisto("h_hits_all_channel", servername);
  cl->registerHisto("h_event", servername);
  cl->registerHisto("h1_waveform_twrAvg", servername);
  cl->registerHisto("h1_waveform_time", servername);
  cl->registerHisto("h1_waveform_pedestal", servername);
  cl->registerHisto("h2_sepd_waveform", servername);
  cl->registerHisto("h1_packet_chans", servername);
  cl->registerHisto("h1_packet_length", servername);
  cl->registerHisto("h1_packet_number", servername);
  cl->registerHisto("h1_packet_event", servername);

  for ( int i = 0; i < 768; ++i )
  {
      cl->registerHisto(Form("h_ADC_channel_%d",i), servername);
  }
  cl->registerHisto("p_noiserms_all_channel", servername);

  // for local host, just call ohcalDrawInit(2)
  CreateSubsysHostlist("sepd_hosts.list", online);

  // says I know they are all on the same node
  cl->requestHistoBySubSystem(servername, 1);
  cl->registerDrawer(sepdmon);                           // register with client framework
}

void sepdDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  OnlMonDraw *sepddraw = cl->GetDrawer(DrawerName);
  for (auto iter = sepddraw->ServerBegin(); iter != sepddraw->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("SEPDMONDRAW", what);                // Draw Histos of registered Drawers
}

void sepdSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("SEPDMONDRAW");                  // Save Plots
  return;
}

void sepdHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("SEPDMONDRAW");                  // Create html output
  return;
}

#include "CommonFuncs.C"

#include <onlmon/zdc/ZdcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlzdcmon_client.so)

std::string DrawerName="ZDCMONDRAW";

void zdcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  // first histogram uses the TH1->GetName() as key
  OnlMonDraw *zdcmon = new ZdcMonDraw(DrawerName);    // create Drawing Object
  std::string servername = "ZDCMON_0";
  zdcmon->AddServer(servername);

  // register histos we want with monitor name
  //zdc
  cl->registerHisto("zdc_adc_north",servername);
  cl->registerHisto("zdc_adc_south",servername);
  

  cl->registerHisto("smd_adc_n_hor_ind0", servername);
  cl->registerHisto("zdc_S1",servername);
  cl->registerHisto("zdc_S2",servername);
  cl->registerHisto("zdc_S3",servername);
  cl->registerHisto("zdc_N1",servername);
  cl->registerHisto("zdc_N2",servername);
  cl->registerHisto("zdc_N3",servername);
    
  cl->registerHisto("h_waveformZDC",servername);
  cl->registerHisto("h_waveformSMD_North",servername);
  cl->registerHisto("h_waveformSMD_South",servername);
  cl->registerHisto("h_waveformVeto_North",servername);
  cl->registerHisto("h_waveformVeto_South",servername);
  cl->registerHisto("h_waveform_timez", servername);
  cl->registerHisto("h_waveform_timess", servername);
  cl->registerHisto("h_waveform_timesn", servername);
  cl->registerHisto("h_waveform_timevs", servername);
  cl->registerHisto("h_waveform_timevn", servername);





 //veto
  cl->registerHisto("veto_NF",servername);
  cl->registerHisto("veto_NB",servername);
  cl->registerHisto("veto_SF",servername);
  cl->registerHisto("veto_SB",servername);
 
 // smd
  // Individual smd_adc channel histos
  
  for(int i=0; i<8;i++)
  {
   cl->registerHisto(Form("smd_adc_n_hor_ind%d", i),servername);
   cl->registerHisto(Form("smd_adc_s_hor_ind%d", i),servername);
  }

  for(int i=0; i<7;i++)
  {
   cl->registerHisto(Form("smd_adc_n_ver_ind%d", i),servername);
   cl->registerHisto(Form("smd_adc_s_ver_ind%d", i),servername);
  }

  // SMD hit Multiplicities
  cl->registerHisto("smd_north_hor_hits", servername);
  cl->registerHisto("smd_north_ver_hits", servername);
  cl->registerHisto("smd_south_hor_hits", servername);
  cl->registerHisto("smd_south_ver_hits", servername);
  
  // north smd
  cl->registerHisto("smd_hor_north", servername);
  cl->registerHisto("smd_ver_north", servername);
  cl->registerHisto("smd_sum_hor_north", servername);
  cl->registerHisto("smd_sum_ver_north", servername);
  cl->registerHisto("smd_hor_north_small", servername);
  cl->registerHisto("smd_ver_north_small", servername);
  cl->registerHisto("smd_hor_north_good", servername);
  cl->registerHisto("smd_ver_north_good", servername);
  // south smd
  cl->registerHisto("smd_hor_south", servername);
  cl->registerHisto("smd_ver_south", servername);
  cl->registerHisto("smd_hor_south_good", servername);
  cl->registerHisto("smd_ver_south_good", servername);
  cl->registerHisto("smd_sum_hor_south", servername);
  cl->registerHisto("smd_sum_ver_south", servername);
  // smd values
  cl->registerHisto("smd_value", servername);
  cl->registerHisto("smd_value_good", servername);
  cl->registerHisto("smd_value_small", servername);
  cl->registerHisto("smd_xy_north", servername);
  cl->registerHisto("smd_xy_south", servername);

  CreateSubsysHostlist("zdc_hosts.list", online);

  // get my histos from server, the second parameter = 1 
  // says I know they are all on the same node
  cl->requestHistoBySubSystem(servername, 1);
  cl->registerDrawer(zdcmon);              // register with client framework
}

void zdcDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  //cl->requestHistoBySubSystem("ZDCMON_0",1);      // update histos
  //cl->Draw("ZDCMONDRAW",what);                      // Draw Histos of registered Drawers
  OnlMonDraw *zdcdraw = cl->GetDrawer(DrawerName);
  for (auto iter = zdcdraw->ServerBegin(); iter != zdcdraw->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("ZDCMONDRAW", what);                // Draw Histos of registered Drawers
}

void zdcSavePlot()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->SavePlot("ZDCMONDRAW");                  // Save Plots
    return;
}

void zdcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("ZDCMONDRAW");                       // create html output
  return;
}

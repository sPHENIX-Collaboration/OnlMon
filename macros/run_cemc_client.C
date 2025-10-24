#include "CommonFuncs.C"

#include <onlmon/cemc/CemcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_client.so)
const int nServers = 16;
  //call with online > 1 if you want to just use the localhost
void cemcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->Verbosity(0);
  CemcMonDraw *cemcmon = new CemcMonDraw("CEMCMONDRAW");  // create Drawing Object
  cemcmon->setSave(0);
  // register histos we want with monitor name
  
  for(int serverid = 0; serverid < nServers; serverid++)
    {
      std::string servername = "CEMCMON_" + std::to_string(serverid);
      cemcmon->AddServer(servername);
      cl->registerHisto("h2_cemc_hits", servername.c_str());
      for(int itrig = 0; itrig < 64; itrig++)
      {
        cl->registerHisto(Form("h2_cemc_hits_trig_bit_%d", itrig), servername.c_str());
      }
      cl->registerHisto("p2_zsFrac_etaphi"  , servername.c_str());
      cl->registerHisto("p2_zsFrac_etaphi_all", servername.c_str());
      cl->registerHisto("p2_bad_chi2", servername.c_str());
      cl->registerHisto("h1_cemc_trig", servername.c_str());
      cl->registerHisto("h_evtRec", servername.c_str());
      cl->registerHisto("h2_cemc_rm", servername.c_str());
      cl->registerHisto("h2_cemc_rmhits", servername.c_str());
      cl->registerHisto("h2_cemc_rmhits_alltrig", servername.c_str());
      cl->registerHisto("h2_cemc_mean", servername.c_str());
      cl->registerHisto("h1_event", servername.c_str());
      cl->registerHisto("h2_waveform_twrAvg", servername.c_str());
      cl->registerHisto("h1_waveform_time", servername.c_str());
      cl->registerHisto("h1_waveform_pedestal", servername.c_str());
      cl->registerHisto("h1_fitting_sigDiff",servername.c_str());
      cl->registerHisto("h1_fitting_pedDiff",servername.c_str());
      cl->registerHisto("h1_fitting_timeDiff",servername.c_str());
      cl->registerHisto("h1_packet_number",servername.c_str());
      cl->registerHisto("h1_packet_length",servername.c_str());
      cl->registerHisto("h1_packet_chans",servername.c_str());
      cl->registerHisto("h1_cemc_adc",servername.c_str());
      cl->registerHisto("p2_pre_post",servername.c_str());
      for(int i = 0; i < 6; i++)cl->registerHisto(Form("h1_packet_status_%d",i),servername.c_str());

    }
  CreateSubsysHostlist("cemc_hosts.list", online);
  //  get my histos from server, the second parameter = 1
  //  says I know they are all on the same node
  for (auto iter = cemcmon->ServerBegin(); iter != cemcmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->registerDrawer(cemcmon);  // register with client framework
}

void cemcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();         // get pointer to framewrk
  OnlMonDraw *cemcmon = cl->GetDrawer("CEMCMONDRAW");  // get pointer to this drawer
  for (auto iter = cemcmon->ServerBegin(); iter != cemcmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("CEMCMONDRAW", what);  // Draw Histos of registered Drawers
}

void cemcSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("CEMCMONDRAW");                  // Save Plots
  return;
}

void cemcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("CEMCMONDRAW");                  // Create html output
  return;
}

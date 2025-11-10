#include "CommonFuncs.C"

#include <onlmon/ll1/LL1MonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlll1mon_client.so)

void ll1DrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("h_nhit_n1", "LL1MON_0");
  cl->registerHisto("h_nhit_n2", "LL1MON_0");
  cl->registerHisto("h_nhit_s1", "LL1MON_0");
  cl->registerHisto("h_nhit_s2", "LL1MON_0");
  cl->registerHisto("h_nhit_corr", "LL1MON_0");
  cl->registerHisto("h_line_up", "LL1MON_0");
  cl->registerHisto("h_line_up", "LL1MON_0");
  cl->registerHisto("h_8x8_sum_emcal", "LL1MON_0");
  cl->registerHisto("h_8x8_sum_emcal_above_threshold_0", "LL1MON_0");
  cl->registerHisto("h_8x8_sum_emcal_above_threshold_1", "LL1MON_0");
  cl->registerHisto("h_8x8_sum_emcal_above_threshold_2", "LL1MON_0");
  cl->registerHisto("h_8x8_sum_emcal_above_threshold_3", "LL1MON_0");
  cl->registerHisto("h_hit_format", "LL1MON_0");
  cl->registerHisto("h_jet_input", "LL1MON_0");
  cl->registerHisto("h_jet_output", "LL1MON_0");
  cl->registerHisto("h_jet_output_above_threshold_0", "LL1MON_0");
  cl->registerHisto("h_jet_output_above_threshold_1", "LL1MON_0");
  cl->registerHisto("h_jet_output_above_threshold_2", "LL1MON_0");
  cl->registerHisto("h_jet_output_above_threshold_3", "LL1MON_0");
  cl->registerHisto("h_sample_diff_emcal", "LL1MON_0");
  for (int i = 0; i < 16;i++)
    {
      std::string histname = "h_2x2_sum_emcal_"+std::to_string(i);
      cl->registerHisto(histname.c_str(), "LL1MON_0");
    }
  cl->registerHisto("h_sample_diff_jet_input", "LL1MON_0");

  // for local host, just call ll1DrawInit(2)
  CreateSubsysHostlist("ll1_hosts.list", online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  

  cl->requestHistoBySubSystem("LL1MON_0", 1);
  OnlMonDraw *ll1mon = new LL1MonDraw("LL1MONDRAW");  // create Drawing Object
  cl->registerDrawer(ll1mon);                // register with client framework
}

void ll1Draw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("LL1MON_0",1);     // update histos
  cl->Draw("LL1MONDRAW", what);                  // Draw Histos of registered Drawers
}

void ll1SavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("LL1MONDRAW");                      // Save Plots
  return;
}

void ll1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("LL1MONDRAW");                    // Create html output
  return;
}

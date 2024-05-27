#include "CommonFuncs.C"

#include <onlmon/hcal/HcalMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlhcalmon_client.so)

void ihcalDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *hcalmon = new HcalMonDraw("IHCALMONDRAW");  // create Drawing Object
  // register histos we want with monitor name
  for (int serverid = 0; serverid < 2; serverid++)
  {
    std::string servername = "IHCALMON_" + std::to_string(serverid);
    hcalmon->AddServer(servername);
    cl->registerHisto("h2_hcal_hits", servername);
    cl->registerHisto("h2_hcal_rm", servername);
    cl->registerHisto("h2_hcal_mean", servername);
    cl->registerHisto("h2_hcal_time", servername);
    cl->registerHisto("h2_hcal_waveform", servername);
    cl->registerHisto("h2_hcal_correlation", servername);
    cl->registerHisto("h_event", servername);
    cl->registerHisto("h_sectorAvg_total", servername);
    cl->registerHisto("h_waveform_twrAvg", servername);
    cl->registerHisto("h_waveform_time", servername);
    cl->registerHisto("h_waveform_pedestal", servername);
    cl->registerHisto("h_ntower", servername);
    cl->registerHisto("h1_packet_chans", servername);
    cl->registerHisto("h1_packet_length", servername);
    cl->registerHisto("h1_packet_number", servername);
    cl->registerHisto("h1_packet_event", servername);
    cl->registerHisto("h_evtRec", servername);
    for (int itrig = 0; itrig < 64; itrig++)
    {
      cl->registerHisto(Form("h2_hcal_hits_trig_bit_%d", itrig), servername);
    }
    cl->registerHisto("h_hcal_trig", servername);
    cl->registerHisto("pr_zsFrac_etaphi", servername);

    for (int ih = 0; ih < 32; ih++)
    {
      cl->registerHisto(Form("h_rm_sectorAvg_s%d", ih), servername);
    }
    for (int ieta = 0; ieta < 24; ieta++)
    {
      for (int iphi = 0; iphi < 64; iphi++)
      {
        cl->registerHisto(Form("h_rm_tower_%d_%d", ieta, iphi), servername);
      }
    }
  }

  // for local host, just call ihcalDrawInit(2)
  CreateSubsysHostlist("hcal_hosts.list", online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  for (auto iter = hcalmon->ServerBegin(); iter != hcalmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }

  cl->registerDrawer(hcalmon);  // register with client framework
}

void ihcalDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();          // get pointer to framewrk
  OnlMonDraw *hcalmon = cl->GetDrawer("IHCALMONDRAW");  // get pointer to this drawer
  for (auto iter = hcalmon->ServerBegin(); iter != hcalmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("IHCALMONDRAW", what);  // Draw Histos of registered Drawers
}

void ihcalSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("IHCALMONDRAW");                 // Save Plots
  return;
}

void ihcalHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("IHCALMONDRAW");                 // Create html output
  return;
}

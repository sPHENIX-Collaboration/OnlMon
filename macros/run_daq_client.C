#include "CommonFuncs.C"

#include <onlmon/daq/DaqMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonldaqmon_client.so)

void daqDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *daqmon = new DaqMonDraw("DAQMONDRAW");  // create Drawing Object

  for (int serverid = 0; serverid < 20; serverid++)
  {
    std::string servername = "DAQMON_" + std::to_string(serverid);
    daqmon->AddServer(servername);
    cl->registerHisto("h_gl1_clock_diff", servername.c_str());
    cl->registerHisto("h_fem_match", servername.c_str());
    //      cl->registerHisto("h_unlock_hist",servername.c_str());
    //      cl->registerHisto("h_unlock_clock",servername.c_str());
  }

  // for local host, just call daqDrawInit(2)
  CreateSubsysHostlist("daq_hosts.list", online);

  for (auto iter = daqmon->ServerBegin(); iter != daqmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }

  cl->registerDrawer(daqmon);  // register with client framework
}

void daqDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();       // get pointer to framewrk
  OnlMonDraw *daqmon = cl->GetDrawer("DAQMONDRAW");  // get pointer to this drawer
  for (auto iter = daqmon->ServerBegin(); iter != daqmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("DAQMONDRAW", what);  // Draw Histos of registered Drawers
}

void daqSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("DAQMONDRAW");                   // Save Plots
  return;
}

void daqHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("DAQMONDRAW");                   // Create html output
  return;
}

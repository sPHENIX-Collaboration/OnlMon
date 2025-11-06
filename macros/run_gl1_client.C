//
// You need to run 2 servers by executing (in 2 separate root sessions)
// .x run_example_server0.C
// .x run_example_server1.C
// and then start the client
// if you only start one server only one set of histograms will be displayed

#include "CommonFuncs.C"

#include <onlmon/gl1/GL1MonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlgl1mon_client.so)

void gl1DrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  for (int i=0; i<64; i++)
  {
    std::string hname = "gl1_scaledtrigger_" + std::to_string(i);
    cl->registerHisto(hname, "GL1MON_0");
    hname = "gl1_livetrigger_" + std::to_string(i);
    cl->registerHisto(hname, "GL1MON_0");
    hname = "gl1_rawtrigger_" + std::to_string(i);
    cl->registerHisto(hname, "GL1MON_0");
  }
//  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("GL1MON_0", 1);
  OnlMonDraw *mymon = new GL1MonDraw("GL1DRAW");  // create Drawing Object
  cl->registerDrawer(mymon);            // register with client framework
}

void gl1Draw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("GL1MON_0",1);         // update histos
  cl->Draw("GL1DRAW", what);                      // Draw Histos of registered Drawers
}

void gl1SavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("GL1DRAW");                          // Save Plots
  return;
}

void gl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("GL1DRAW");                        // Create html output
  return;
}

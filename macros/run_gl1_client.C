#include "CommonFuncs.C"

#include <onlmon/gl1/GL1MonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlgl1mon_client.so)

std::string GL1DrawerName="GL1MONDRAW";

void gl1DrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *gl1mon = new GL1MonDraw(GL1DrawerName);  // create Drawing Object
  std::string servername = "GL1MON_0";
  gl1mon->AddServer(servername);
  // register histos we want with monitor name
  cl->registerHisto("gl1_stats", servername);

  for (int i=0; i<8; i++)
  {
    std::string hname = "gl1_reject_" + std::to_string(i);
    cl->registerHisto(hname, servername);
  }
  for (int i=0; i<5; i++)
  {
    std::string hname = "gl1_timetolastevent" + std::to_string(i);
    cl->registerHisto(hname, servername);
  }
  for (int i=0; i<64; i++)
  {
    std::string hname = "gl1_scaledtrigger_" + std::to_string(i);
    cl->registerHisto(hname, servername);
    hname = "gl1_livetrigger_" + std::to_string(i);
    cl->registerHisto(hname, servername);
    hname = "gl1_rawtrigger_" + std::to_string(i);
    cl->registerHisto(hname, servername);
  }
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem(servername, 1);
  cl->registerDrawer(gl1mon);            // register with client framework
}

void gl1Draw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  OnlMonDraw *drawer = cl->GetDrawer(GL1DrawerName);
  for (auto iter = drawer->ServerBegin(); iter != drawer->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(*iter, 1);
  }
  cl->Draw(GL1DrawerName, what);                      // Draw Histos of registered Drawers
}

void gl1SavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot(GL1DrawerName);                          // Save Plots
  return;
}

void gl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml(GL1DrawerName);                        // Create html output
  return;
}

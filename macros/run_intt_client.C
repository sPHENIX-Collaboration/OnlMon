#include "CommonFuncs.C"

#include <onlmon/OnlMonClient.h>
#include <onlmon/intt/InttMonDraw.h>

R__LOAD_LIBRARY(libonlinttmon_client.so)

void inttDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  OnlMonDraw *inttmon = new InttMonDraw("INTTMONDRAW");    // create Drawing Object

  for (int serverid = 0; serverid < 8; serverid++)
  {
    std::string servername = "INTTMON_" + std::to_string(serverid);
    inttmon->AddServer(servername);

    cl->registerHisto("InttEvtHist",servername);
    cl->registerHisto("InttHitHist",servername);
    cl->registerHisto("InttBcoHist",servername);
    cl->registerHisto("InttLogHist",servername);
	//...
  }

  // for local host, just call inttDrawInit(2)
  CreateSubsysHostlist("intt_hosts.list", online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
   for (auto iter = inttmon->ServerBegin(); iter != inttmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->registerDrawer(inttmon);              // register with client framework
}

void inttDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();		// get pointer to framewrk
  OnlMonDraw *inttmon = cl->GetDrawer("INTTMONDRAW");  // get pointer to this drawer
  for (auto iter = inttmon->ServerBegin(); iter != inttmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("INTTMONDRAW",what);				// Draw Histos of registered Drawers
}

void inttSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("INTTMONDRAW");                          // Save Plots
  return;
}

void inttHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("INTTMONDRAW");                        // Create html output
  return;
}

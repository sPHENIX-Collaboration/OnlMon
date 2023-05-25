#include <CommonFuncs.C>

#include <onlmon/epd/EpdMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlepdmon_client.so)

void epdDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("h_ADC0_s", "EPDMON_0");
  cl->registerHisto("h_hits0_s", "EPDMON_0");
  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("EPDMON_0", 1);
  OnlMonDraw *epdmon = new EpdMonDraw("EPDMONDRAW");  // create Drawing Object
  cl->registerDrawer(epdmon);             // register with client framework
}

void epdDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("EPDMON_0");        // update histos
  cl->Draw("EPDMONDRAW", what);                     // Draw Histos of registered Drawers
}

void epdPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("EPDMONDRAW");                         // Create PS files
  return;
}

void epdHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("EPDMONDRAW");                       // Create html output
  return;
}

#include "CommonFuncs.C"

#include <onlmon/spin/SpinMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlspinmon_client.so)

void spinDrawInit(const int online = 0)
{

  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("h1_pCspinpatternBlue","SPINMON_0");
  cl->registerHisto("h1_pCspinpatternYellow","SPINMON_0");
  cl->registerHisto("h1_spinpatternBlue","SPINMON_0");
  cl->registerHisto("h1_spinpatternYellow","SPINMON_0");

  cl->registerHisto("h2_pCspinpatternBlueUp","SPINMON_0");
  cl->registerHisto("h2_pCspinpatternBlueDown","SPINMON_0");
  cl->registerHisto("h2_pCspinpatternBlueUnpol","SPINMON_0");

  cl->registerHisto("h2_pCspinpatternYellowUp","SPINMON_0");
  cl->registerHisto("h2_pCspinpatternYellowDown","SPINMON_0");
  cl->registerHisto("h2_pCspinpatternYellowUnpol","SPINMON_0");

  cl->registerHisto("h2_spinpatternBlueUp","SPINMON_0");
  cl->registerHisto("h2_spinpatternBlueDown","SPINMON_0");
  cl->registerHisto("h2_spinpatternBlueUnpol","SPINMON_0");

  cl->registerHisto("h2_spinpatternYellowUp","SPINMON_0");
  cl->registerHisto("h2_spinpatternYellowDown","SPINMON_0");
  cl->registerHisto("h2_spinpatternYellowUnpol","SPINMON_0");
  
  cl->registerHisto("h1_polBlue","SPINMON_0");
  cl->registerHisto("h1_polYellow","SPINMON_0");

  cl->registerHisto("h1_xingshift","SPINMON_0");
  cl->registerHisto("h1_fillnumber","SPINMON_0");
  cl->registerHisto("h1_filltypeBlue","SPINMON_0");
  cl->registerHisto("h1_filltypeYellow","SPINMON_0");

  cl->registerHisto("hCorrect","SPINMON_0");
  cl->registerHisto("hAbortgap","SPINMON_0");
  cl->registerHisto("hForbidden","SPINMON_0");


  for (int i = 0; i < 16; i++){
    cl->registerHisto(Form("gl1_counter_trig%d",i),"SPINMON_0");
  }

  // for local host, just call spinDrawInit(2)
  CreateSubsysHostlist("spin_hosts.list", online);

// says I know they are all on the same node
  cl->requestHistoBySubSystem("SPINMON_0", 1);
  OnlMonDraw *spinmon = new SpinMonDraw("SPINMONDRAW");    // create Drawing Object
  cl->registerDrawer(spinmon);              // register with client framework
}

void spinDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("SPINMON_0",1);      // update histos
  cl->Draw("SPINMONDRAW",what);                      // Draw Histos of registered Drawers
}

void spinSavePlot()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->SavePlot("SPINMONDRAW");                  // Save Plots
    return;
}

void spinHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("SPINMONDRAW");                       // create html output
  return;
}

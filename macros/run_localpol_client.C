#include "CommonFuncs.C"

#include <onlmon/localpol/LocalPolMonDraw.h>

#include <onlmon/OnlMonClient.h>
#include <TString.h>

R__LOAD_LIBRARY(libonllocalpolmon_client.so)

void localpolDrawInit(const int online = 0)
{

  OnlMonClient *cl = OnlMonClient::instance();
  //cl->Verbosity(3);
  // register histos we want with monitor name

  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  for(int beam=0; beam<2; beam++){
    for(int method=0; method<2; method++){
      for(int orient=0; orient<2; orient++){
	cl->registerHisto(Form("h_Asym%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),"LOCALPOLMON_0");
	cl->registerHisto(Form("h_AsymScramble%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),"LOCALPOLMON_0");
      }
    }
  }
  for(int i=0; i<16;i++){
    cl->registerHisto(Form("h_trigger%d",i),"LOCALPOLMON_0");
  }
  cl->registerHisto("h_BlueCountsUD","LOCALPOLMON_0");
  cl->registerHisto("h_BlueCountsLR","LOCALPOLMON_0");
  cl->registerHisto("h_YellCountsUD","LOCALPOLMON_0");
  cl->registerHisto("h_YellCountsLR","LOCALPOLMON_0");

  cl->registerHisto("h_BlueCountsScrambleUD","LOCALPOLMON_0");
  cl->registerHisto("h_BlueCountsScrambleLR","LOCALPOLMON_0");
  cl->registerHisto("h_YellCountsScrambleUD","LOCALPOLMON_0");
  cl->registerHisto("h_YellCountsScrambleLR","LOCALPOLMON_0");

  cl->registerHisto("h_events","LOCALPOLMON_0");
  cl->registerHisto("h_times","LOCALPOLMON_0");


  cl->registerHisto("hmultiplicitySMD_NH","LOCALPOLMON_0");
  cl->registerHisto("hmultiplicitySMD_NV","LOCALPOLMON_0");
  cl->registerHisto("hmultiplicitySMD_SH","LOCALPOLMON_0");
  cl->registerHisto("hmultiplicitySMD_SV","LOCALPOLMON_0");

  cl->registerHisto("hpositionSMD_NH_up","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_NV_up","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_SH_up","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_SV_up","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_NH_dn","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_NV_dn","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_SH_dn","LOCALPOLMON_0");
  cl->registerHisto("hpositionSMD_SV_dn","LOCALPOLMON_0");

  cl->registerHisto("hadcsumSMD_NH","LOCALPOLMON_0");
  cl->registerHisto("hadcsumSMD_NV","LOCALPOLMON_0");
  cl->registerHisto("hadcsumSMD_SH","LOCALPOLMON_0");
  cl->registerHisto("hadcsumSMD_SV","LOCALPOLMON_0");

  cl->registerHisto("hspinpattern","LOCALPOLMON_0");

  for(int i=0; i<6; i++){
    cl->registerHisto(Form("hwaveform%d",i),"LOCALPOLMON_0");
  }
  
  //cl->registerHisto("htimesync","LOCALPOLMON_0");
  cl->registerHisto("hsyncfrac","LOCALPOLMON_0");
  
  cl->registerHisto("Bluespace","LOCALPOLMON_0");
  cl->registerHisto("Yellowspace","LOCALPOLMON_0");

  cl->registerHisto("hclocks","LOCALPOLMON_0");

  cl->registerHisto("hevolsync","LOCALPOLMON_0");
  cl->registerHisto("hshiftevol","LOCALPOLMON_0");

  // for local host, just call localpolDrawInit(2)
  //cl->AddServerHost("localhost");  // check local host first
  CreateSubsysHostlist("localpol_hosts.list", online);

// says I know they are all on the same node
  cl->requestHistoBySubSystem("LOCALPOLMON_0", 1);

  OnlMonDraw *localpolmon = new LocalPolMonDraw("LOCALPOLMONDRAW");    // create Drawing Object
  cl->registerDrawer(localpolmon);              // register with client framework
}

void localpolDraw(const char *what="ALL")
{

  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("LOCALPOLMON_0",1);      // update histos
  cl->Draw("LOCALPOLMONDRAW",what);                      // Draw Histos of registered Drawers

}

void localpolSavePlot()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->SavePlot("LOCALPOLMONDRAW");                  // Save Plots
    return;
}

void localpolHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("LOCALPOLMONDRAW");                       // create html output
  return;
}

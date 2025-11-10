#include "CommonFuncs.C"

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

//const std::string DrawerName = "MVTXMONDRAW";

R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *mvtxmon = new MvtxMonDraw("MVTXMONDRAW");  // create Drawing Object
  // register histos we want with monitor name
  std::string mLaneStatusFlag[3] = {"WARNING", "ERROR", "FAULT"};
  const int NStaves[3] = {12, 16, 20};

  for (int serverid = 0; serverid < 6; serverid++)
  {
    std::string servername = "MVTXMON_" + std::to_string(serverid);
    mvtxmon->AddServer(servername);
    cl->registerHisto("OCC_ChipStaveOcc", servername);
    cl->registerHisto("OCC_ChipStave1D", servername);
    cl->registerHisto("OCC_ChipFiredFLX", servername);
    cl->registerHisto("OCC_HitChipPerStrobe", servername);
    cl->registerHisto("OCC_HitFLXPerStrobe", servername);
    cl->registerHisto("MVTXMON_General_Occupancy", servername);
    cl->registerHisto("FEE_LaneStatus_Overview_FlagPROBLEM", servername);
    cl->registerHisto("FEE_LaneStatus_Overview_FlagWARNING", servername);
    cl->registerHisto("FEE_LaneStatus_Overview_FlagERROR", servername);
    cl->registerHisto("FEE_LaneStatus_Overview_FlagFAULT", servername);

    cl->registerHisto("General_hChipStrobes", servername);
    cl->registerHisto("General_ChipL1", servername);
    cl->registerHisto("General_hfeeStrobes", servername);
    cl->registerHisto("General_feeL1", servername);

    cl->registerHisto("hStrobesDMA", servername);
    cl->registerHisto("hDMAstatus", servername);

    // FEE

    for (int i = 0; i < MvtxMonDraw::NFlags; i++)
    {
      cl->registerHisto(Form("FEE_LaneStatus_Flag_%s", mLaneStatusFlag[i].c_str()), servername);
      cl->registerHisto(Form("FEE_LaneStatusFromSOX_Flag_%s", mLaneStatusFlag[i].c_str()), servername);
    }

    // raw task
    cl->registerHisto("General_DecErrors", servername);
    cl->registerHisto("General_DecErrorsTime", servername);
    cl->registerHisto("General_DecErrorsEndpoint", servername);

    for (int aLayer = 0; aLayer < 3; aLayer++)
    {
      cl->registerHisto(Form("OCC_Occupancy1D_Layer%d", aLayer), servername);
      cl->registerHisto(Form("OCC_OccupancyChipStave_Layer_%d", aLayer), servername);
      cl->registerHisto(Form("FHR_NoisyChipStave_Layer%d", aLayer), servername);
    }

    // fhr
    cl->registerHisto("FHR_ErrorVsFeeid", servername);
    cl->registerHisto("MVTXMON_General_Noisy_Pixel", servername);
    cl->registerHisto("RCDAQ_evt", servername);

    for (int mLayer = 0; mLayer < 3; mLayer++)
    {
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dDeadChipPos", mLayer, mLayer), servername);
    }

    cl->registerHisto("RDHErrors_hfeeRDHErrors", servername);

  }

  // for local host, just call mvtxDrawInit(2)
  CreateSubsysHostlist("mvtx_hosts.list", online);
  // says I know they are all on the same node
  for (auto iter = mvtxmon->ServerBegin(); iter != mvtxmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }

  cl->registerDrawer(mvtxmon);  // register with client framework
}

void mvtxDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();         // get pointer to framewrk
  OnlMonDraw *mvtxmon = cl->GetDrawer("MVTXMONDRAW");  // get pointer to this drawer
  for (auto iter = mvtxmon->ServerBegin(); iter != mvtxmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("MVTXMONDRAW", what);  // Draw Histos of registered Drawers
}

void mvtxSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("MVTXMONDRAW");                  // Save Plots
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMONDRAW");                  // Create html output
  return;
}

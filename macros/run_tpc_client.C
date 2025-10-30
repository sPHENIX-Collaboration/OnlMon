#include "CommonFuncs.C"

#include <onlmon/tpc/TpcMonDraw.h>

#include <onlmon/OnlMonClient.h>

//const std::string DrawerName = "TPCMONDRAW";

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpcmon_client.so)

void tpcFetchHistos();

time_t lastupdate {0};
time_t max_time_to_last_update = 5*60; // 5 minutes

void tpcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *tpcmon = new TpcMonDraw("TPCMONDRAW");  // create Drawing Object
  // register histos we want with monitor name

  // TPC ADC pie chart
  for (int serverid = 0; serverid < 48; serverid++)
  {
    std::string servername = "TPCMON_" + std::to_string(serverid);
    tpcmon->AddServer(servername);
    //std::cout<<"You registered the NSIDEADC/SSIDEADC "<<i<<" histo"<<std::endl;

    cl->registerHisto("NorthSideADC", servername);

    cl->registerHisto("NorthSideADC_clusterXY_R1", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R2", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R3", servername);

    cl->registerHisto("NorthSideADC_clusterXY_R1_LASER", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R2_LASER", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R3_LASER", servername);

    cl->registerHisto("NorthSideADC_clusterXY_R1_u5", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R2_u5", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R3_u5", servername);

    cl->registerHisto("NorthSideADC_clusterXY_R1_unw", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R2_unw", servername);
    cl->registerHisto("NorthSideADC_clusterXY_R3_unw", servername);

    cl->registerHisto("SouthSideADC", servername);

    cl->registerHisto("SouthSideADC_clusterXY_R1", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R2", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R3", servername);

    cl->registerHisto("SouthSideADC_clusterXY_R1_LASER", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R2_LASER", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R3_LASER", servername);

    cl->registerHisto("SouthSideADC_clusterXY_R1_u5", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R2_u5", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R3_u5", servername);

    cl->registerHisto("SouthSideADC_clusterXY_R1_unw", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R2_unw", servername);
    cl->registerHisto("SouthSideADC_clusterXY_R3_unw", servername);

    cl->registerHisto("sample_size_hist",servername);
    cl->registerHisto("Check_Sum_Error",servername);
    cl->registerHisto("Parity_Error",servername);
    cl->registerHisto("Check_Sums",servername);
    cl->registerHisto("Stuck_Channels",servername);
    cl->registerHisto("Channels_in_Packet",servername);
    cl->registerHisto("Channels_Always",servername);
    cl->registerHisto("LVL_1_TAGGER_per_EBDC",servername);
    cl->registerHisto("Num_non_ZS_channels_vs_SAMPA",servername);
    cl->registerHisto("First_ADC_vs_First_Time_Bin",servername);
    cl->registerHisto("ZS_Trigger_ADC_vs_Sample",servername);
    cl->registerHisto("ADC_vs_SAMPLE",servername); 
    cl->registerHisto("ADC_vs_SAMPLE_large",servername);
    cl->registerHisto("PEDEST_SUB_ADC_vs_SAMPLE",servername);
    cl->registerHisto("PEDEST_SUB_ADC_vs_SAMPLE_R1",servername);
    cl->registerHisto("PEDEST_SUB_ADC_vs_SAMPLE_R2",servername);
    cl->registerHisto("PEDEST_SUB_ADC_vs_SAMPLE_R3",servername);
    cl->registerHisto("MAXADC",servername);

    cl->registerHisto("RAWADC_1D_R1",servername);
    cl->registerHisto("MAXADC_1D_R1",servername);
    cl->registerHisto("PEDEST_SUB_1D_R1",servername);
    cl->registerHisto("COUNTS_vs_SAMPLE_1D_R1",servername);
    cl->registerHisto("RAWADC_1D_R2",servername);
    cl->registerHisto("MAXADC_1D_R2",servername);
    cl->registerHisto("PEDEST_SUB_1D_R2",servername);
    cl->registerHisto("COUNTS_vs_SAMPLE_1D_R2",servername);
    cl->registerHisto("RAWADC_1D_R3",servername);
    cl->registerHisto("MAXADC_1D_R3",servername);
    cl->registerHisto("PEDEST_SUB_1D_R3",servername);
    cl->registerHisto("COUNTS_vs_SAMPLE_1D_R3",servername);

    cl->registerHisto("NorthSideADC_clusterZY", servername);
    cl->registerHisto("SouthSideADC_clusterZY", servername);

    cl->registerHisto("NorthSideADC_clusterZY_unw",servername);
    cl->registerHisto("SouthSideADC_clusterZY_unw",servername);

    cl->registerHisto("Layer_ChannelPhi_ADC_weighted",servername);
    cl->registerHisto("NEvents_vs_EBDC",servername);
    cl->registerHisto("NStreaks_vs_EventNo",servername);
    
    cl->registerHisto("Packet_Type_Fraction_HB",servername);
    cl->registerHisto("Packet_Type_Fraction_NORM",servername);
    cl->registerHisto("Packet_Type_Fraction_ELSE",servername);
    cl->registerHisto("Packet_Type_vs_sample_ADC",servername);

    cl->registerHisto("Noise_Channel_Plots",servername);

    cl->registerHisto("DC_vs_SAMPA",servername);
    cl->registerHisto("DC_SAMPA_vs_TIME",servername);
  } //



  // for local host, just call tpcDrawInit(2)
  CreateSubsysHostlist("tpc_hosts.list", online);

  cl->registerDrawer(tpcmon);             // register with client framework
}

void tpcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  if (std::time(nullptr) - lastupdate > max_time_to_last_update)
    {
      OnlMonDraw *tpcmon = cl->GetDrawer("TPCMONDRAW");  // get pointer to this drawer
      for (auto iter = tpcmon->ServerBegin(); iter != tpcmon->ServerEnd(); ++iter)
	{
	  cl->requestHistoBySubSystem(iter->c_str(), 1);
	}
      lastupdate = std::time(nullptr);
    }
  cl->Draw("TPCMONDRAW", what);                     // Draw Histos of registered Drawers
}

void tpcSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("TPCMONDRAW");                         // Save Plots
  return;
}

void tpcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("TPCMONDRAW");                       // Create html output
  return;
}

void tpcFetchHistos()
{
  lastupdate = 0;
  return;
}

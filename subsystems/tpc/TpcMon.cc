// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "TpcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
#include <Event/msg_profile.h>
#include <Event/oncsSubConstants.h>

#include <tpc/TpcMap.h> 

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TTree.h>
#include <TLatex.h>

#include <vector>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

TpcMon::TpcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new TpcMon()

  serverid = 0; //default case - initializing in constructor
  //BCO initialization in TPCMon
  starting_BCO = -1;
  rollover_value = 0;
  current_BCOBIN = 0;
  M.setMapNames("AutoPad-R1-RevA.sch.ChannelMapping.csv", "AutoPad-R2-RevA-Pads.sch.ChannelMapping.csv", "AutoPad-R3-RevA.sch.ChannelMapping.csv");
  return;
}

TpcMon::~TpcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int TpcMon::Init()
{
  // read our calibrations from TpcMonData.dat
  const char *tpccalib = getenv("TPCCALIB");
  if (!tpccalib)
  {
    std::cout << "TPCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(tpccalib) + "/" + "TpcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  //TPC GEM Module Displays
  NorthSideADC = new TH2F("NorthSideADC" , "ADC Counts North Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  SouthSideADC = new TH2F("SouthSideADC" , "ADC Counts South Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  //

  //TPC "cluster" XY heat maps WEIGHTED
  //NorthSideADC_clusterXY_R1 = new TH2F("NorthSideADC_clusterXY_R1" , "ADC Peaks North Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R1 = new TH2F("NorthSideADC_clusterXY_R1" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R1->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R1->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R2 = new TH2F("NorthSideADC_clusterXY_R2" , "ADC Peaks North Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R2 = new TH2F("NorthSideADC_clusterXY_R2" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R2->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R2->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R3 = new TH2F("NorthSideADC_clusterXY_R3" , "ADC Peaks North Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R3 = new TH2F("NorthSideADC_clusterXY_R3" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R3->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R3->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R1 = new TH2F("SouthSideADC_clusterXY_R1" , "ADC Peaks South Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R1 = new TH2F("SouthSideADC_clusterXY_R1" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R1->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R1->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R2 = new TH2F("SouthSideADC_clusterXY_R2" , "ADC Peaks South Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R2 = new TH2F("SouthSideADC_clusterXY_R2" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R2->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R2->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R3 = new TH2F("SouthSideADC_clusterXY_R3" , "ADC Peaks South Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);  
  SouthSideADC_clusterXY_R3 = new TH2F("SouthSideADC_clusterXY_R3" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R3->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R3->SetYTitle("Y [mm]");

  //____________________________________________________________________//

  //TPC "cluster" XY heat maps DURING LASER TIME WEIGHTED
  //
  NorthSideADC_clusterXY_R1_LASER = new TH2F("NorthSideADC_clusterXY_R1_LASER" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R1_LASER->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R1_LASER->SetYTitle("Y [mm]");

  //
  NorthSideADC_clusterXY_R2_LASER = new TH2F("NorthSideADC_clusterXY_R2_LASER" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R2_LASER->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R2_LASER->SetYTitle("Y [mm]");

  //
  NorthSideADC_clusterXY_R3_LASER = new TH2F("NorthSideADC_clusterXY_R3_LASER" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R3_LASER->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R3_LASER->SetYTitle("Y [mm]");

  //
  SouthSideADC_clusterXY_R1_LASER = new TH2F("SouthSideADC_clusterXY_R1_LASER" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R1_LASER->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R1_LASER->SetYTitle("Y [mm]");

  //
  SouthSideADC_clusterXY_R2_LASER = new TH2F("SouthSideADC_clusterXY_R2_LASER" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R2_LASER->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R2_LASER->SetYTitle("Y [mm]");

  //  
  SouthSideADC_clusterXY_R3_LASER = new TH2F("SouthSideADC_clusterXY_R3_LASER" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R3_LASER->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R3_LASER->SetYTitle("Y [mm]");

  //____________________________________________________________________//

  //TPC "cluster" XY heat maps UNWEIGHTED
  //NorthSideADC_clusterXY_R1 = new TH2F("NorthSideADC_clusterXY_R1" , "ADC Peaks North Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R1_unw = new TH2F("NorthSideADC_clusterXY_R1_unw" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R1_unw->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R1_unw->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R2 = new TH2F("NorthSideADC_clusterXY_R2" , "ADC Peaks North Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R2_unw = new TH2F("NorthSideADC_clusterXY_R2_unw" , "(ADC-Pedestal) > 5#sigma Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R2_unw->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R2_unw->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R3 = new TH2F("NorthSideADC_clusterXY_R3" , "ADC Peaks North Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R3_unw = new TH2F("NorthSideADC_clusterXY_R3_unw" , "(ADC-Pedestal) > 5#sigma Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R3_unw->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R3_unw->SetYTitle("Y [mm]");


  //SouthSideADC_clusterXY_R1 = new TH2F("SouthSideADC_clusterXY_R1" , "ADC Peaks South Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R1_unw = new TH2F("SouthSideADC_clusterXY_R1_unw" , "(ADC-Pedestal) > 5#sigma", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R1_unw->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R1_unw->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R2 = new TH2F("SouthSideADC_clusterXY_R2" , "ADC Peaks South Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R2_unw = new TH2F("SouthSideADC_clusterXY_R2_unw" , "(ADC-Pedestal) > 5#sigma", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R2_unw->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R2_unw->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R3 = new TH2F("SouthSideADC_clusterXY_R3" , "ADC Peaks South Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);  
  SouthSideADC_clusterXY_R3_unw = new TH2F("SouthSideADC_clusterXY_R3_unw" , "(ADC-Pedestal) > 5#sigma", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R3_unw->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R3_unw->SetYTitle("Y [mm]");

  //____________________________________________________________________//

  //TPC 5 event "cluster" XY heat maps WEIGHTED
  //NorthSideADC_clusterXY_R1 = new TH2F("NorthSideADC_clusterXY_R1" , "ADC Peaks North Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R1_u5 = new TH2F("NorthSideADC_clusterXY_R1_u5" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R1_u5->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R1_u5->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R2 = new TH2F("NorthSideADC_clusterXY_R2" , "ADC Peaks North Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R2_u5 = new TH2F("NorthSideADC_clusterXY_R2_u5" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R2_u5->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R2_u5->SetYTitle("Y [mm]");

  //NorthSideADC_clusterXY_R3 = new TH2F("NorthSideADC_clusterXY_R3" , "ADC Peaks North Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  NorthSideADC_clusterXY_R3_u5 = new TH2F("NorthSideADC_clusterXY_R3_u5" , "(ADC-Pedestal) > 5#sigma North Side", 400, -800, 800, 400, -800, 800);
  NorthSideADC_clusterXY_R3_u5->SetXTitle("X [mm]");
  NorthSideADC_clusterXY_R3_u5->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R1 = new TH2F("SouthSideADC_clusterXY_R1" , "ADC Peaks South Side", N_phi_binx_XY_R1, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R1_u5 = new TH2F("SouthSideADC_clusterXY_R1_u5" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R1_u5->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R1_u5->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R2 = new TH2F("SouthSideADC_clusterXY_R2" , "ADC Peaks South Side", N_phi_binx_XY_R2, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);
  SouthSideADC_clusterXY_R2_u5 = new TH2F("SouthSideADC_clusterXY_R2_u5" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R2_u5->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R2_u5->SetYTitle("Y [mm]");

  //SouthSideADC_clusterXY_R3 = new TH2F("SouthSideADC_clusterXY_R3" , "ADC Peaks South Side", N_phi_binx_XY_R3, -TMath::Pi(), TMath::Pi(), N_rBins_XY, r_bins);  
  SouthSideADC_clusterXY_R3_u5 = new TH2F("SouthSideADC_clusterXY_R3_u5" , "(ADC-Pedestal) > 5#sigma South Side", 400, -800, 800, 400, -800, 800);
  SouthSideADC_clusterXY_R3_u5->SetXTitle("X [mm]");
  SouthSideADC_clusterXY_R3_u5->SetYTitle("Y [mm]");

  //____________________________________________________________________//


  //TPC "cluster" ZY heat maps WEIGHTED
   NorthSideADC_clusterZY = new TH2F("NorthSideADC_clusterZY" , "(ADC-Pedestal) > 5#sigma North Side", 206, -1030, 1030, 400, -800, 800);
   SouthSideADC_clusterZY = new TH2F("SouthSideADC_clusterZY" , "(ADC-Pedestal) > 5#sigma South Side", 206, -1030, 1030, 400, -800, 800);

  //____________________________________________________________________//

  //TPC "cluster" ZY heat maps UNWEIGHTED
   NorthSideADC_clusterZY_unw = new TH2F("NorthSideADC_clusterZY_unw" , "(ADC-Pedestal) > 5#sigma North Side", 206, -1030, 1030, 400, -800, 800);
   SouthSideADC_clusterZY_unw = new TH2F("SouthSideADC_clusterZY_unw" , "(ADC-Pedestal) > 5#sigma South Side", 206, -1030, 1030, 400, -800, 800);

  //____________________________________________________________________//

  //

  // ADC vs Sample (small)
  char ADC_vs_SAMPLE_str[100];
  char ADC_vs_SAMPLE_xaxis_str[100];
  sprintf(ADC_vs_SAMPLE_str,"ADC Counts vs Sample: SECTOR %i",MonitorServerId());
  sprintf(ADC_vs_SAMPLE_xaxis_str,"Sector %i: ADC Time bin [1/20MHz]",MonitorServerId());
  ADC_vs_SAMPLE = new TH2F("ADC_vs_SAMPLE", ADC_vs_SAMPLE_str, 500, 0, 500, 256, 0, 1024);
  ADC_vs_SAMPLE -> SetXTitle(ADC_vs_SAMPLE_xaxis_str);
  ADC_vs_SAMPLE -> SetYTitle("ADC [ADU]");

  ADC_vs_SAMPLE -> GetXaxis() -> SetLabelSize(0.05);
  ADC_vs_SAMPLE -> GetXaxis() -> SetTitleSize(0.05);
  ADC_vs_SAMPLE -> GetYaxis() -> SetLabelSize(0.05);
  ADC_vs_SAMPLE -> GetYaxis() -> SetTitleSize(0.05);
  ADC_vs_SAMPLE -> GetYaxis() -> SetTitleOffset(1.0); 

  // PEDEST SUB ADC vs Sample (small)
  char PEDEST_SUB_ADC_vs_SAMPLE_str[100];
  char PEDEST_SUB_ADC_vs_SAMPLE_xaxis_str[100];
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_str,"ADC Counts vs Sample: SECTOR %i",MonitorServerId());
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_xaxis_str,"Sector %i: ADC Time bin [1/20MHz]",MonitorServerId());
  PEDEST_SUB_ADC_vs_SAMPLE = new TH2F("PEDEST_SUB_ADC_vs_SAMPLE", PEDEST_SUB_ADC_vs_SAMPLE_str, 500, 0, 500, 281, -100, 1024);
  PEDEST_SUB_ADC_vs_SAMPLE -> SetXTitle(PEDEST_SUB_ADC_vs_SAMPLE_xaxis_str);
  PEDEST_SUB_ADC_vs_SAMPLE -> SetYTitle("ADC-ped. [ADU]");

  PEDEST_SUB_ADC_vs_SAMPLE -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE -> GetYaxis() -> SetTitleOffset(1.0); 

  // ADC vs Sample (small)
  char PEDEST_SUB_ADC_vs_SAMPLE_R1_str[100];
  char PEDEST_SUB_ADC_vs_SAMPLE_R1_xaxis_str[100];
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R1_str,"ADC Counts vs Sample: SECTOR %i R1",MonitorServerId());
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R1_xaxis_str,"Sector %i R1: ADC Time bin [1/20MHz]",MonitorServerId());
  PEDEST_SUB_ADC_vs_SAMPLE_R1 = new TH2F("PEDEST_SUB_ADC_vs_SAMPLE_R1", PEDEST_SUB_ADC_vs_SAMPLE_R1_str, 500, 0, 500, 281, -100, 1024);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> SetXTitle(PEDEST_SUB_ADC_vs_SAMPLE_R1_xaxis_str);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> SetYTitle("ADC-ped. [ADU]");

  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetYaxis() -> SetTitleOffset(1.0); 

  // ADC vs Sample (small)
  char PEDEST_SUB_ADC_vs_SAMPLE_R2_str[100];
  char PEDEST_SUB_ADC_vs_SAMPLE_R2_xaxis_str[100];
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R2_str,"ADC Counts vs Sample: SECTOR %i R2",MonitorServerId());
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R2_xaxis_str,"Sector %i R2: ADC Time bin [1/20MHz]",MonitorServerId());
  PEDEST_SUB_ADC_vs_SAMPLE_R2 = new TH2F("PEDEST_SUB_ADC_vs_SAMPLE_R2", PEDEST_SUB_ADC_vs_SAMPLE_R2_str, 500, 0, 500, 281, -100, 1024);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> SetXTitle(PEDEST_SUB_ADC_vs_SAMPLE_R2_xaxis_str);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> SetYTitle("ADC-ped. [ADU]");
  
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R2 -> GetYaxis() -> SetTitleOffset(1.0); 

  // ADC vs Sample (small)
  char PEDEST_SUB_ADC_vs_SAMPLE_R3_str[100];
  char PEDEST_SUB_ADC_vs_SAMPLE_R3_xaxis_str[100];
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R3_str,"ADC Counts vs Sample: SECTOR %i R3",MonitorServerId());
  sprintf(PEDEST_SUB_ADC_vs_SAMPLE_R3_xaxis_str,"Sector %i R3: ADC Time bin [1/20MHz]",MonitorServerId());
  PEDEST_SUB_ADC_vs_SAMPLE_R3 = new TH2F("PEDEST_SUB_ADC_vs_SAMPLE_R3", PEDEST_SUB_ADC_vs_SAMPLE_R3_str, 500, 0, 500, 281, -100, 1024);
  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> SetXTitle(PEDEST_SUB_ADC_vs_SAMPLE_R3_xaxis_str);
  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> SetYTitle("ADC-ped. [ADU]");

  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R3 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_ADC_vs_SAMPLE_R1 -> GetYaxis() -> SetTitleOffset(1.0); 

  // ADC vs Sample (large)
  char ADC_vs_SAMPLE_large_str[100];
  char ADC_vs_SAMPLE_xaxis_large_str[100];
  sprintf(ADC_vs_SAMPLE_large_str,"ADC Counts vs Large Sample: SECTOR %i",MonitorServerId());
  sprintf(ADC_vs_SAMPLE_xaxis_large_str,"Sector %i: ADC Time bin [1/20MHz]",MonitorServerId());
  ADC_vs_SAMPLE_large = new TH2F("ADC_vs_SAMPLE_large", ADC_vs_SAMPLE_large_str, 1080, 0, 1080, 256, 0, 1024);
  ADC_vs_SAMPLE_large -> SetXTitle(ADC_vs_SAMPLE_xaxis_large_str);

  ADC_vs_SAMPLE_large -> SetYTitle("ADC [ADU]");
  ADC_vs_SAMPLE_large -> GetXaxis() -> SetLabelSize(0.05);
  ADC_vs_SAMPLE_large -> GetXaxis() -> SetTitleSize(0.05);
  ADC_vs_SAMPLE_large -> GetYaxis() -> SetLabelSize(0.05);
  ADC_vs_SAMPLE_large -> GetYaxis() -> SetTitleSize(0.05);
  ADC_vs_SAMPLE_large -> GetYaxis() -> SetTitleOffset(1.0); 

  // Sample size distribution 1D histogram
  char sample_size_title_str[100];
  sprintf(sample_size_title_str,"Distribution of Sample Sizes in Events: SECTOR %i",MonitorServerId());
  sample_size_hist = new TH1F("sample_size_hist" , sample_size_title_str, 1500, 0.5, 1500.5);
  sample_size_hist->SetXTitle("sample size");
  sample_size_hist->SetYTitle("counts");

  sample_size_hist -> GetXaxis() -> SetLabelSize(0.05);
  sample_size_hist -> GetXaxis() -> SetTitleSize(0.05);
  sample_size_hist -> GetYaxis() -> SetLabelSize(0.05);
  sample_size_hist -> GetYaxis() -> SetTitleSize(0.05);
  sample_size_hist -> GetYaxis() -> SetTitleOffset(1.0); 

  // Stuck Channel 1D histogram
  char stuck_channel_title_str[100];
  sprintf(stuck_channel_title_str,"Stuck Channels per FEE in Events: Sector %i",MonitorServerId());
  Stuck_Channels = new TH1F("Stuck_Channels",stuck_channel_title_str,26,-0.5,25.5);
  Stuck_Channels->SetXTitle("FEE_NUM");
  Stuck_Channels->SetYTitle("NUM_STUCK_CHANNELS");

  Stuck_Channels -> GetXaxis() -> SetLabelSize(0.05);
  Stuck_Channels -> GetXaxis() -> SetTitleSize(0.05);
  Stuck_Channels -> GetYaxis() -> SetLabelSize(0.05);
  Stuck_Channels -> GetYaxis() -> SetTitleSize(0.05);
  Stuck_Channels -> GetYaxis() -> SetTitleOffset(1.0); 

  // entries vs FEE*8 + SAMPA Number
  Check_Sums = new TH1F("Check_Sums" , "Entries vs Fee*8 + SAMPA in Events",208,-0.5, 207.5);
  Check_Sums->SetXTitle("FEE_NUM*8 + SAMPA_ADRR");
  Check_Sums->SetYTitle("Entries");
  Check_Sums->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it
  Check_Sums->SetFillColor(4);

  Check_Sums -> GetXaxis() -> SetLabelSize(0.05);
  Check_Sums -> GetXaxis() -> SetTitleSize(0.05);
  Check_Sums -> GetYaxis() -> SetLabelSize(0.05);
  Check_Sums -> GetYaxis() -> SetTitleSize(0.05);
  Check_Sums -> GetYaxis() -> SetTitleOffset(1.0); 

  // checksum error vs FEE*8 + SAMPA Number
  char checksum_title_str[100];
  sprintf(checksum_title_str,"Check Sum Error Probability vs Fee*8 + SAMPA in Events: SECTOR %i",MonitorServerId());
  Check_Sum_Error = new TH1F("Check_Sum_Error" , checksum_title_str,208,-0.5, 207.5);
  Check_Sum_Error->SetXTitle("FEE_NUM*8 + SAMPA_ADDR");
  Check_Sum_Error->SetYTitle("Prob. Check Sum. Err.");
  Check_Sum_Error->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it
  Check_Sum_Error->SetFillColor(4);

  Check_Sum_Error -> GetXaxis() -> SetLabelSize(0.05);
  Check_Sum_Error -> GetXaxis() -> SetTitleSize(0.05);
  Check_Sum_Error -> GetYaxis() -> SetLabelSize(0.05);
  Check_Sum_Error -> GetYaxis() -> SetTitleSize(0.05);
  Check_Sum_Error -> GetYaxis() -> SetTitleOffset(1.0);

  // Parity error vs FEE*8 + SAMPA Number
  char parity_title_str[100];
  sprintf(parity_title_str,"Parity Error Probability vs Fee*8 + SAMPA in Events: SECTOR %i",MonitorServerId());
  Parity_Error = new TH1F("Parity_Error" , checksum_title_str,208,-0.5, 207.5);
  Parity_Error->SetXTitle("FEE_NUM*8 + SAMPA_ADDR");
  Parity_Error->SetYTitle("Prob. Parity Err.");
  Parity_Error->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it
  Parity_Error->SetFillColor(4);

  Parity_Error -> GetXaxis() -> SetLabelSize(0.05);
  Parity_Error -> GetXaxis() -> SetTitleSize(0.05);
  Parity_Error -> GetYaxis() -> SetLabelSize(0.05);
  Parity_Error -> GetYaxis() -> SetTitleSize(0.05);
  Parity_Error -> GetYaxis() -> SetTitleOffset(1.0);
 
  // number of nonZS channels - <number of channels in sampa with values != 65 K ADC> vs sampa number + (feeID * sampa number)
  char num_nonZS_channels_title_str[100];
  sprintf(num_nonZS_channels_title_str,"Number of non ZS Channels: # of samples in channel waveform != 65K vs SAMPA ID: SECTOR %i",MonitorServerId());  
  // x-axis is channel phi, y-axis is channel layer, z axis is ADC weithing
  Num_non_ZS_channels_vs_SAMPA = new TH2F("Num_non_ZS_channels_vs_SAMPA",num_nonZS_channels_title_str,208,-0.5,207.5,1324,-300.5,1023.5);
  Num_non_ZS_channels_vs_SAMPA->SetXTitle("SAMPA ID: SAMPA # + (feeID * 8)");
  Num_non_ZS_channels_vs_SAMPA->SetYTitle("# of Non-Zero Suppressed Samples in WF");

  Num_non_ZS_channels_vs_SAMPA -> GetXaxis() -> SetLabelSize(0.05);
  Num_non_ZS_channels_vs_SAMPA -> GetXaxis() -> SetTitleSize(0.05);
  Num_non_ZS_channels_vs_SAMPA -> SetLabelSize(0.05);
  Num_non_ZS_channels_vs_SAMPA -> GetYaxis() -> SetTitleSize(0.05);
  Num_non_ZS_channels_vs_SAMPA -> GetYaxis() -> SetTitleOffset(1.0);

  // # of times channels are in packet per RCDAQ event
  char chans_in_packet_title_str[100];
  sprintf(chans_in_packet_title_str,"Channel counts vs Channel in packets in  RCDAQ Events: SECTOR %i",MonitorServerId());  
  Channels_in_Packet = new TH1F("Channels_in_Packet" , chans_in_packet_title_str, 6656, -0.5, 6655.5);
  Channels_in_Packet->SetXTitle("(FEE # * 256) + chan. #");
  Channels_in_Packet->SetYTitle("Counts / Packet in RCDAQ Event");
  Channels_in_Packet->SetStats(0);

  Channels_in_Packet -> GetXaxis() -> SetLabelSize(0.05);
  Channels_in_Packet -> GetXaxis() -> SetTitleSize(0.05);
  Channels_in_Packet -> GetYaxis() -> SetLabelSize(0.05);
  Channels_in_Packet -> GetYaxis() -> SetTitleSize(0.05);
  Channels_in_Packet -> GetYaxis() -> SetTitleOffset(1.0);  
  
  // # of times channels could be in packet per RCDAQ event
  char chans_always_title_str[100];
  sprintf(chans_always_title_str,"Channel counts vs all channels (filled once per event w/ packets): SECTOR %i",MonitorServerId());  
  Channels_Always = new TH1F("Channels_Always" , chans_always_title_str, 6656, -0.5, 6655.5);
  Channels_Always->SetXTitle("(FEE # * 256) + chan. #");
  Channels_Always->SetYTitle("Counts");
  Channels_Always->SetStats(0);

  Channels_Always -> GetXaxis() -> SetLabelSize(0.05);
  Channels_Always -> GetXaxis() -> SetTitleSize(0.05);
  Channels_Always -> GetYaxis() -> SetLabelSize(0.05);
  Channels_Always -> GetYaxis() -> SetTitleSize(0.05);
  Channels_Always -> GetYaxis() -> SetTitleOffset(1.0);

  // # of LVL 1 tagger per EBDC
  LVL_1_TAGGER_per_EBDC = new TH1F("LVL_1_TAGGER_per_EBDC","Number of LEVEL 1 TAGGERS PER EBDC",24,-0.5,23.5);
  LVL_1_TAGGER_per_EBDC->SetXTitle("EBDC #");
  LVL_1_TAGGER_per_EBDC->SetYTitle("N_{LVL_1_TAGGER}");  

  //ZS ADC vs Sample (small)
  char ZS_ADC_vs_SAMPLE_str[100];
  char ZS_ADC_vs_SAMPLE_xaxis_str[100];
  sprintf(ZS_ADC_vs_SAMPLE_str,"ADC Counts vs Sample - Trigger QA: SECTOR %i",MonitorServerId());
  sprintf(ZS_ADC_vs_SAMPLE_xaxis_str,"Sector %i: ADC Time bin [1/20MHz]",MonitorServerId());
  ZS_Trigger_ADC_vs_Sample = new TH2F("ZS_Trigger_ADC_vs_Sample", ZS_ADC_vs_SAMPLE_str, 500, -0.5, 499.5, 1024, 0, 1024);
  ZS_Trigger_ADC_vs_Sample -> SetXTitle(ZS_ADC_vs_SAMPLE_xaxis_str);
  ZS_Trigger_ADC_vs_Sample -> SetYTitle("ADC [ADU]");

  ZS_Trigger_ADC_vs_Sample -> GetXaxis() -> SetLabelSize(0.05);
  ZS_Trigger_ADC_vs_Sample -> GetXaxis() -> SetTitleSize(0.05);
  ZS_Trigger_ADC_vs_Sample -> GetYaxis() -> SetLabelSize(0.05);
  ZS_Trigger_ADC_vs_Sample -> GetYaxis() -> SetTitleSize(0.05);
  ZS_Trigger_ADC_vs_Sample -> GetYaxis() -> SetTitleOffset(1.0);

  //First non-ZS ADC vs Sample (small)
  char First_ADC_vs_First_Time_Bin_str[100];
  char First_ADC_vs_First_Time_Bin_xaxis_str[100];
  sprintf(First_ADC_vs_First_Time_Bin_str,"1st nonZS ADC vs 1st nonZS Sample Time: SECTOR %i",MonitorServerId());
  sprintf(First_ADC_vs_First_Time_Bin_xaxis_str,"Sector %i: 1st non-ZS Time bin [1/20MHz]",MonitorServerId());
  First_ADC_vs_First_Time_Bin = new TH2F("First_ADC_vs_First_Time_Bin", First_ADC_vs_First_Time_Bin_str, 500, -0.5, 499.5, 256, 0, 1024);
  First_ADC_vs_First_Time_Bin -> SetXTitle(First_ADC_vs_First_Time_Bin_xaxis_str);
  First_ADC_vs_First_Time_Bin -> SetYTitle("1st non-ZS ADC [ADU]");

  First_ADC_vs_First_Time_Bin -> GetXaxis() -> SetLabelSize(0.05);
  First_ADC_vs_First_Time_Bin -> GetXaxis() -> SetTitleSize(0.05);
  First_ADC_vs_First_Time_Bin -> GetYaxis() -> SetLabelSize(0.05);
  First_ADC_vs_First_Time_Bin -> GetYaxis() -> SetTitleSize(0.05);
  First_ADC_vs_First_Time_Bin -> GetYaxis() -> SetTitleOffset(1.0);  

  // Max ADC per waveform dist for each module (R1, R2, R3)
  char MAXADC_str[100];
  char YLabel_str[5];

  sprintf(MAXADC_str,"MAX ADC per Waveform in SLIDING WINDOW: SECTOR %i",MonitorServerId());

  MAXADC = new TH2F("MAXADC" , MAXADC_str,1025,-0.5, 1024.5,3,-0.5,2.5);
  MAXADC->SetXTitle("LocalMAX ADC in Waveform [ADU]");
  MAXADC->SetYTitle("Entries");
  MAXADC->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it

  for(int i = 0; i < 3; i++ )
  {
    sprintf(YLabel_str,"R%i",i+1);
    MAXADC->GetYaxis()->SetBinLabel(i+1,YLabel_str);
    MAXADC->GetYaxis()->SetLabelSize(0.12);
    MAXADC->GetXaxis()->SetLabelSize(0.04);
  }

  //________For 1D ADC spectra - Doing this the hard way because I'm not sure if I can have a TH1 array
  char RAWADC_1D_titlestr[100];
  char MAXADC_1D_titlestr[100];
  char SUBADC_1D_titlestr[100];
  char COUNTS_SAMPLE_1D_titlestr[100];
  char COUNTS_SAMPLE_1D_xtitlestr[100];

  sprintf(RAWADC_1D_titlestr,"RAW ADC for Sector %i R1",MonitorServerId());
  sprintf(MAXADC_1D_titlestr,"MAX ADC in SLIDING WINDOW for Sector %i R1",MonitorServerId());
  sprintf(SUBADC_1D_titlestr,"PEDEST_SUB RAW ADC for Sector %i R1",MonitorServerId());
  sprintf(COUNTS_SAMPLE_1D_titlestr,"COUNTS_vs_SAMPLE for Sector %i R1",MonitorServerId());
  sprintf(COUNTS_SAMPLE_1D_xtitlestr,"Sector %i: Time bin [1/20MHz]",MonitorServerId()); 

  RAWADC_1D_R1 = new TH1F("RAWADC_1D_R1",RAWADC_1D_titlestr,1025,-0.5,1024.5);
  MAXADC_1D_R1 = new TH1F("MAXADC_1D_R1",MAXADC_1D_titlestr,1025,-0.5,1024.5);
  PEDEST_SUB_1D_R1 = new TH1F("PEDEST_SUB_1D_R1",SUBADC_1D_titlestr,1125,-100.5,1024.5);
  COUNTS_vs_SAMPLE_1D_R1 = new TH1F("COUNTS_vs_SAMPLE_1D_R1",COUNTS_SAMPLE_1D_titlestr,500,0,500);
  
  RAWADC_1D_R1->SetYTitle("Entries");
  RAWADC_1D_R1->SetXTitle("ADC [ADU]");

  MAXADC_1D_R1->SetYTitle("Entries");
  MAXADC_1D_R1->SetXTitle("(MAXADC-pedestal) [ADU]");

  PEDEST_SUB_1D_R1->SetYTitle("Entries");
  PEDEST_SUB_1D_R1->SetXTitle("(ADC-pedestal) [ADU]");

  PEDEST_SUB_1D_R1->SetYTitle("Entries");
  PEDEST_SUB_1D_R1->SetXTitle("(ADC-pedestal) [ADU]");

  COUNTS_vs_SAMPLE_1D_R1->SetYTitle("Entries");
  COUNTS_vs_SAMPLE_1D_R1->SetXTitle(COUNTS_SAMPLE_1D_xtitlestr);
  
  MAXADC_1D_R1->Sumw2(kFALSE);
  RAWADC_1D_R1->Sumw2(kFALSE);
  PEDEST_SUB_1D_R1->Sumw2(kFALSE);
  COUNTS_vs_SAMPLE_1D_R1->Sumw2(kFALSE);

  MAXADC_1D_R1->SetLineColor(2);
  RAWADC_1D_R1->SetLineColor(2);
  PEDEST_SUB_1D_R1->SetLineColor(2);
  COUNTS_vs_SAMPLE_1D_R1->SetLineColor(2);

  MAXADC_1D_R1 -> GetXaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R1 -> GetXaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R1 -> GetYaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R1 -> GetYaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R1 -> GetYaxis() -> SetTitleOffset(1.0);
  RAWADC_1D_R1 -> GetXaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R1 -> GetXaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R1 -> GetYaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R1 -> GetYaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R1 -> GetYaxis() -> SetTitleOffset(1.0); 
  PEDEST_SUB_1D_R1 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R1 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R1 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R1 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R1 -> GetYaxis() -> SetTitleOffset(1.0);
  COUNTS_vs_SAMPLE_1D_R1 -> GetXaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R1 -> GetXaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R1 -> GetYaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R1 -> GetYaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R1 -> GetYaxis() -> SetTitleOffset(1.0);

  sprintf(RAWADC_1D_titlestr,"RAW ADC for Sector %i R2",MonitorServerId());
  sprintf(MAXADC_1D_titlestr,"MAX ADC for Sector %i R2",MonitorServerId());
  sprintf(SUBADC_1D_titlestr,"PEDEST_SUB RAW ADC for Sector %i R2`",MonitorServerId());
  sprintf(COUNTS_SAMPLE_1D_titlestr,"COUNTS_vs_SAMPLE for Sector %i R2",MonitorServerId());  

  RAWADC_1D_R2 = new TH1F("RAWADC_1D_R2",RAWADC_1D_titlestr,1025,-0.5,1024.5);
  MAXADC_1D_R2 = new TH1F("MAXADC_1D_R2",MAXADC_1D_titlestr,1025,-0.5,1024.5);
  PEDEST_SUB_1D_R2 = new TH1F("PEDEST_SUB_1D_R2",SUBADC_1D_titlestr,1125,-100.5,1024.5);
  COUNTS_vs_SAMPLE_1D_R2 = new TH1F("COUNTS_vs_SAMPLE_1D_R2",COUNTS_SAMPLE_1D_titlestr,500,0,500);

  RAWADC_1D_R2->SetYTitle("Entries");
  RAWADC_1D_R2->SetXTitle("ADC [ADU]");

  MAXADC_1D_R2->SetYTitle("Entries");
  MAXADC_1D_R2->SetXTitle("(MAXADC-pedestal) [ADU]");

  PEDEST_SUB_1D_R2->SetYTitle("Entries");
  PEDEST_SUB_1D_R2->SetXTitle("(ADC-pedestal) [ADU]");

  COUNTS_vs_SAMPLE_1D_R2->SetYTitle("Entries");
  COUNTS_vs_SAMPLE_1D_R2->SetXTitle(COUNTS_SAMPLE_1D_xtitlestr);

  MAXADC_1D_R2->Sumw2(kFALSE);
  RAWADC_1D_R2->Sumw2(kFALSE);
  PEDEST_SUB_1D_R2->Sumw2(kFALSE);
  COUNTS_vs_SAMPLE_1D_R2->Sumw2(kFALSE);

  MAXADC_1D_R2->SetLineColor(3);
  RAWADC_1D_R2->SetLineColor(3);
  PEDEST_SUB_1D_R2->SetLineColor(3);
  COUNTS_vs_SAMPLE_1D_R2->SetLineColor(3);
 
  MAXADC_1D_R2 -> GetXaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R2 -> GetXaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R2 -> GetYaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R2 -> GetYaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R2 -> GetYaxis() -> SetTitleOffset(1.0);
  RAWADC_1D_R2 -> GetXaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R2 -> GetXaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R2 -> GetYaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R2 -> GetYaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R2 -> GetYaxis() -> SetTitleOffset(1.0); 
  PEDEST_SUB_1D_R2 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R2 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R2 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R2 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R2 -> GetYaxis() -> SetTitleOffset(1.0);
  COUNTS_vs_SAMPLE_1D_R2 -> GetXaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R2 -> GetXaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R2 -> GetYaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R2 -> GetYaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R2 -> GetYaxis() -> SetTitleOffset(1.0);

  sprintf(RAWADC_1D_titlestr,"RAW ADC for Sector %i R3",MonitorServerId());
  sprintf(MAXADC_1D_titlestr,"MAX ADC for Sector %i R3",MonitorServerId());
  sprintf(SUBADC_1D_titlestr,"PEDEST_SUB RAW ADC for Sector %i R3",MonitorServerId()); 

  RAWADC_1D_R3 = new TH1F("RAWADC_1D_R3",RAWADC_1D_titlestr,1025,-0.5,1024.5);
  MAXADC_1D_R3 = new TH1F("MAXADC_1D_R3",MAXADC_1D_titlestr,1025,-0.5,1024.5);
  PEDEST_SUB_1D_R3 = new TH1F("PEDEST_SUB_1D_R3",SUBADC_1D_titlestr,1125,-100.5,1024.5);
  COUNTS_vs_SAMPLE_1D_R3 = new TH1F("COUNTS_vs_SAMPLE_1D_R3",COUNTS_SAMPLE_1D_titlestr,500,0,500);

  RAWADC_1D_R3->SetYTitle("Entries");
  RAWADC_1D_R3->SetXTitle("ADC [ADU]");

  MAXADC_1D_R3->SetYTitle("Entries");
  MAXADC_1D_R3->SetXTitle("(MAXADC-pedestal) [ADU]");

  PEDEST_SUB_1D_R3->SetYTitle("Entries");
  PEDEST_SUB_1D_R3->SetXTitle("(ADC-pedestal) [ADU]");

  COUNTS_vs_SAMPLE_1D_R3->SetYTitle("Entries");
  COUNTS_vs_SAMPLE_1D_R3->SetXTitle(COUNTS_SAMPLE_1D_xtitlestr);

  MAXADC_1D_R3->Sumw2(kFALSE);  // ADC vs Sample (small)
  RAWADC_1D_R3->Sumw2(kFALSE);
  PEDEST_SUB_1D_R3->Sumw2(kFALSE);
  COUNTS_vs_SAMPLE_1D_R3->Sumw2(kFALSE);

  MAXADC_1D_R3->SetLineColor(4);
  RAWADC_1D_R3->SetLineColor(4);
  PEDEST_SUB_1D_R3->SetLineColor(4);
  COUNTS_vs_SAMPLE_1D_R3->SetLineColor(4);

  MAXADC_1D_R3 -> GetXaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R3 -> GetXaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R3 -> GetYaxis() -> SetLabelSize(0.05);
  MAXADC_1D_R3 -> GetYaxis() -> SetTitleSize(0.05);
  MAXADC_1D_R3 -> GetYaxis() -> SetTitleOffset(1.0);
  RAWADC_1D_R3 -> GetXaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R3 -> GetXaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R3 -> GetYaxis() -> SetLabelSize(0.05);
  RAWADC_1D_R3 -> GetYaxis() -> SetTitleSize(0.05);
  RAWADC_1D_R3 -> GetYaxis() -> SetTitleOffset(1.0); 
  PEDEST_SUB_1D_R3 -> GetXaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R3 -> GetXaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R3 -> GetYaxis() -> SetLabelSize(0.05);
  PEDEST_SUB_1D_R3 -> GetYaxis() -> SetTitleSize(0.05);
  PEDEST_SUB_1D_R3 -> GetYaxis() -> SetTitleOffset(1.0);
  COUNTS_vs_SAMPLE_1D_R3 -> GetXaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R3 -> GetXaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R3 -> GetYaxis() -> SetLabelSize(0.05);
  COUNTS_vs_SAMPLE_1D_R3 -> GetYaxis() -> SetTitleSize(0.05);
  COUNTS_vs_SAMPLE_1D_R3 -> GetYaxis() -> SetTitleOffset(1.0);
  
  char Layer_ChannelPhi_ADC_weighted_title_str[256];
  sprintf(Layer_ChannelPhi_ADC_weighted_title_str,"Layer vs Channel Phi");  
  // x-axis is channel phi, y-axis is channel layer, z axis is ADC weithing
  Layer_ChannelPhi_ADC_weighted = new TH2F("Layer_ChannelPhi_ADC_weighted",Layer_ChannelPhi_ADC_weighted_title_str,4610,-2305.5,2304.5,61,-0.5,59.5);
  Layer_ChannelPhi_ADC_weighted->SetXTitle("Channel # (#phi bin)");
  Layer_ChannelPhi_ADC_weighted->SetYTitle("Layer");

  char NStreakers_vs_Event_title_str[256];
  sprintf(NStreakers_vs_Event_title_str,"Number of Streakers vs Event, Sector # %i",MonitorServerId());
  NStreaks_vs_EventNo = new TH1F("NStreaks_vs_EventNo",NStreakers_vs_Event_title_str, 1000000, -0.5, 999999.5 );
  NStreaks_vs_EventNo->SetXTitle("Event #");
  NStreaks_vs_EventNo->SetYTitle("Number of horizontal streak channels");
  NStreaks_vs_EventNo -> GetXaxis() -> SetLabelSize(0.05);
  NStreaks_vs_EventNo -> GetXaxis() -> SetTitleSize(0.05);
  NStreaks_vs_EventNo -> GetYaxis() -> SetLabelSize(0.05);
  NStreaks_vs_EventNo -> GetYaxis() -> SetTitleSize(0.05);
  NStreaks_vs_EventNo -> GetYaxis() -> SetTitleOffset(1.0);

  char NEvents_vs_EBDC_title_str[256];
  sprintf(NEvents_vs_EBDC_title_str,"N_{Events} vs EBDC");
  NEvents_vs_EBDC = new TH1F("NEvents_vs_EBDC",NEvents_vs_EBDC_title_str,24,-0.5,23.5);
  NEvents_vs_EBDC->SetXTitle("EBDC #");
  NEvents_vs_EBDC->SetYTitle("N_{Events}");  

  char Packet_Type_Fraction_title_str[256];
  sprintf(Packet_Type_Fraction_title_str,"Numer of Waveforms per Packet Type, Sector # %i",MonitorServerId());
  Packet_Type_Fraction_HB = new TH1F("Packet_Type_Fraction_HB",Packet_Type_Fraction_title_str,7,0,7);
  Packet_Type_Fraction_HB->SetYTitle("N_{Waveforms}");
  Packet_Type_Fraction_HB->SetLineColor(4);
  Packet_Type_Fraction_HB->SetFillColor(4);  
 
  Packet_Type_Fraction_NORM = new TH1F("Packet_Type_Fraction_NORM",Packet_Type_Fraction_title_str,7,0,7);
  Packet_Type_Fraction_NORM->SetYTitle("N_{Waveforms}");
  Packet_Type_Fraction_NORM->SetLineColor(3);
  Packet_Type_Fraction_NORM->SetFillColor(3); 

  Packet_Type_Fraction_ELSE = new TH1F("Packet_Type_Fraction_ELSE",Packet_Type_Fraction_title_str,7,0,7);
  Packet_Type_Fraction_ELSE->SetYTitle("N_{Waveforms}");
  Packet_Type_Fraction_ELSE->SetLineColor(2);
  Packet_Type_Fraction_ELSE->SetFillColor(2); 

  const char* label[7] = { "HEARTBEAT_T", "TRUNCATED_DATA_T ", "TRUNCATED_TRIG_EARLY_DATA_T", "NORMAL_DATA_T", "LARGE_DATA_T", "TRIG_EARLY_DATA_T", "TRIG_EARLY_LARGE_DATA_T" };
  for (int i=0; i!=7; ++i) {Packet_Type_Fraction_HB->GetXaxis()->SetBinLabel(i+1,label[i]);Packet_Type_Fraction_NORM->GetXaxis()->SetBinLabel(i+1,label[i]);Packet_Type_Fraction_ELSE->GetXaxis()->SetBinLabel(i+1,label[i]);}

  Packet_Type_Fraction_HB -> GetXaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_HB -> GetYaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_HB -> GetYaxis() -> SetTitleSize(0.08);
  Packet_Type_Fraction_HB -> GetYaxis() -> SetTitleOffset(0.6);

  Packet_Type_Fraction_NORM -> GetXaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_NORM -> GetYaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_NORM -> GetYaxis() -> SetTitleSize(0.08);
  Packet_Type_Fraction_NORM -> GetYaxis() -> SetTitleOffset(0.6);

  Packet_Type_Fraction_ELSE -> GetXaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_ELSE -> GetYaxis() -> SetLabelSize(0.08);
  Packet_Type_Fraction_ELSE -> GetYaxis() -> SetTitleSize(0.08);
  Packet_Type_Fraction_ELSE -> GetYaxis() -> SetTitleOffset(0.6);

  char Noise_Channel_Plots_title_str[256];
  sprintf(Noise_Channel_Plots_title_str,"Counts of ADC-Ped. > 300, t < 360 ,Sector # %i",MonitorServerId());
  Noise_Channel_Plots = new TH1F("Noise_Channel_Plots","",6656,-0.5,6655.5);
  Noise_Channel_Plots -> GetXaxis() -> SetLabelSize(0.05);
  Noise_Channel_Plots -> GetXaxis() -> SetTitleSize(0.05);
  Noise_Channel_Plots -> GetYaxis() -> SetLabelSize(0.05);
  Noise_Channel_Plots -> GetYaxis() -> SetTitleSize(0.05);
  Noise_Channel_Plots -> GetYaxis() -> SetTitleOffset(1.0);

  Noise_Channel_Plots->SetXTitle("(FEE # * 256) + chan. #");
  Noise_Channel_Plots->SetYTitle("Counts/Event");
  Noise_Channel_Plots->SetStats(0); 

  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, NorthSideADC);
  se->registerHisto(this, SouthSideADC);
  se->registerHisto(this, sample_size_hist);
  se->registerHisto(this, Check_Sum_Error);
  se->registerHisto(this, Parity_Error);
  se->registerHisto(this, Check_Sums);
  se->registerHisto(this, Stuck_Channels);
  se->registerHisto(this, Channels_in_Packet);
  se->registerHisto(this, Channels_Always);
  se->registerHisto(this, LVL_1_TAGGER_per_EBDC);
  se->registerHisto(this, Num_non_ZS_channels_vs_SAMPA);
  se->registerHisto(this, ZS_Trigger_ADC_vs_Sample);
  se->registerHisto(this, First_ADC_vs_First_Time_Bin);
  se->registerHisto(this, ADC_vs_SAMPLE);
  se->registerHisto(this, PEDEST_SUB_ADC_vs_SAMPLE);
  se->registerHisto(this, PEDEST_SUB_ADC_vs_SAMPLE_R1);
  se->registerHisto(this, PEDEST_SUB_ADC_vs_SAMPLE_R2);
  se->registerHisto(this, PEDEST_SUB_ADC_vs_SAMPLE_R3);
  se->registerHisto(this, ADC_vs_SAMPLE_large);
  se->registerHisto(this, MAXADC);
  se->registerHisto(this, RAWADC_1D_R1);
  se->registerHisto(this, MAXADC_1D_R1);
  se->registerHisto(this, PEDEST_SUB_1D_R1);
  se->registerHisto(this, COUNTS_vs_SAMPLE_1D_R1);
  se->registerHisto(this, RAWADC_1D_R2);
  se->registerHisto(this, MAXADC_1D_R2);
  se->registerHisto(this, PEDEST_SUB_1D_R2);
  se->registerHisto(this, COUNTS_vs_SAMPLE_1D_R2);
  se->registerHisto(this, RAWADC_1D_R3);
  se->registerHisto(this, MAXADC_1D_R3);
  se->registerHisto(this, PEDEST_SUB_1D_R3);
  se->registerHisto(this, COUNTS_vs_SAMPLE_1D_R3);

  se->registerHisto(this, NorthSideADC_clusterXY_R1);
  se->registerHisto(this, NorthSideADC_clusterXY_R2);
  se->registerHisto(this, NorthSideADC_clusterXY_R3);

  se->registerHisto(this, SouthSideADC_clusterXY_R1);
  se->registerHisto(this, SouthSideADC_clusterXY_R2);
  se->registerHisto(this, SouthSideADC_clusterXY_R3);

  se->registerHisto(this, NorthSideADC_clusterXY_R1_unw);
  se->registerHisto(this, NorthSideADC_clusterXY_R2_unw);
  se->registerHisto(this, NorthSideADC_clusterXY_R3_unw);

  se->registerHisto(this, SouthSideADC_clusterXY_R1_unw);
  se->registerHisto(this, SouthSideADC_clusterXY_R2_unw);
  se->registerHisto(this, SouthSideADC_clusterXY_R3_unw);

  se->registerHisto(this, NorthSideADC_clusterXY_R1_LASER);
  se->registerHisto(this, NorthSideADC_clusterXY_R2_LASER);
  se->registerHisto(this, NorthSideADC_clusterXY_R3_LASER);

  se->registerHisto(this, SouthSideADC_clusterXY_R1_LASER);
  se->registerHisto(this, SouthSideADC_clusterXY_R2_LASER);
  se->registerHisto(this, SouthSideADC_clusterXY_R3_LASER);

  se->registerHisto(this, NorthSideADC_clusterXY_R1_u5);
  se->registerHisto(this, NorthSideADC_clusterXY_R2_u5);
  se->registerHisto(this, NorthSideADC_clusterXY_R3_u5);

  se->registerHisto(this, SouthSideADC_clusterXY_R1_u5);
  se->registerHisto(this, SouthSideADC_clusterXY_R2_u5);
  se->registerHisto(this, SouthSideADC_clusterXY_R3_u5);

  se->registerHisto(this, NorthSideADC_clusterZY);
  se->registerHisto(this, SouthSideADC_clusterZY);

  se->registerHisto(this, NorthSideADC_clusterZY_unw);
  se->registerHisto(this, SouthSideADC_clusterZY_unw);

  se->registerHisto(this, Layer_ChannelPhi_ADC_weighted); 
  se->registerHisto(this, NEvents_vs_EBDC);
  se->registerHisto(this, NStreaks_vs_EventNo);
  se->registerHisto(this, Packet_Type_Fraction_HB);
  se->registerHisto(this, Packet_Type_Fraction_NORM);
  se->registerHisto(this, Packet_Type_Fraction_ELSE);

  se->registerHisto(this, Noise_Channel_Plots);

  Reset();
  return 0;
}

int TpcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  // we reset the BCO for the new run
  starting_BCO = -1;
  rollover_value = 0;
  current_BCOBIN = 0;

  

  return 0;
}

int TpcMon::process_event(Event *evt/* evt */)
{

  //std::cout << "TpcMon::process_event(Event * evt) Processing Event" << std::endl;

  if (evt == nullptr)
  {
    std::cout << "TpcMon::process_event - Event not found" << std::endl;
    return -1;
  }

  if (evt->getEvtType() >= 8)  /// special events
  {
    std::cout << "TpcMon::process_event - Special Event type >= 8, moving on" << std::endl;
    return -1;
  }

  //reset these each event
  float North_Side_Arr[36] = {0};
  float South_Side_Arr[36] = {0};

  std::vector<int> store_ten; 
  std::vector<int> median_and_stdev_vec;


  // we check if we have legacy data and start with packet 4000
  // the range for the TPC is really 4001...4032
  // we assume we start properly at 4001, but check if not
  
//  int firstpacket=4000;
  //if (evt->existPacket(4000))
  //{
  //Packet *p = evt->getPacket(4000);
  // if(evt->getPacket(4000))
  // {
  //   Packet *p = evt->getPacket(4000);
  //   if (p->getHitFormat() == IDTPCFEEV3 || p->getHitFormat() == IDTPCFEEV4) firstpacket = 4000;
  //   delete p;
  // }
  //}
//  int lastpacket = firstpacket+232;

  NEvents_vs_EBDC->Fill(MonitorServerId());
  //std::cout<<"Event #"<< evtcnt <<std::endl;

    std::vector<Packet *> pktvec = evt->getPacketVector();
  //  if( evtcnt >= 1200 ){ //evtcnt debug  
  //for( int packet = firstpacket; packet < lastpacket; packet++) //packet 4001 or 4002 = Sec 00, packet 4231 or 4232 = Sec 23
    for (auto p : pktvec)
  {
//    Packet* p = evt->getPacket(packet);
    if (!p)
    {
      //std::cout << "TpcMon::process_event - No packet numbered " << packet << " in this event!!" << std::endl;
      continue;
    }
    else
    {
      //std::cout << "____________________________________" << std::endl;
      //std::cout << "Packet # " << packet << std::endl;
      int nr_of_waveforms = p->iValue(0, "NR_WF");
      //std::cout << "Hello Waveforms ! - There are " << nr_of_waveforms << " of you !" << std::endl;

      bool is_channel_stuck = 0;

      for(int ci = 0; ci < 6656; ci++){ Channels_Always->Fill(ci);}

      for( int wf = 0; wf < nr_of_waveforms; wf++)
      {
	//std::cout<<"START OF WF LOOP, "<<"current wf = "<<wf<<", total wf = "<<nr_of_waveforms<<" EVENT "<<evtcnt<<std::endl;
        int current_BCO = p->iValue(wf, "BCO") + rollover_value;
        if (starting_BCO < 0)
        {
          starting_BCO = current_BCO;
        }

        if (current_BCO < starting_BCO)  // we have a rollover
        {
          rollover_value += 0x100000;
          current_BCO = p->iValue(wf, "BCO") + rollover_value;
          starting_BCO = current_BCO;
          current_BCOBIN++;
        }

	int type = p->iValue(wf,"TYPE");
	switch(type)
	{
	case 0:
	  Packet_Type_Fraction_HB->Fill(0.5); //HEARTBEAT_T 0b000
	  break;
	case 1:
	  Packet_Type_Fraction_ELSE->Fill(1.5); //TRUNCATED_DATA_T 0b001
	  break;
	case 3:
	  Packet_Type_Fraction_ELSE->Fill(2.5); //TRUNCATED_TRIG_EARLY_DATA_T 0b011
	  break;
	case 4:
	  Packet_Type_Fraction_NORM->Fill(3.5); //NORMAL_DATA_T 0b100
	  break;
	case 5:
	  Packet_Type_Fraction_ELSE->Fill(4.5); //LARGE_DATA_T 0b101
	  break;
	case 6:
	  Packet_Type_Fraction_ELSE->Fill(5.5); //TRIG_EARLY_DATA_T 0b110
	  break;
	case 7:
	  Packet_Type_Fraction_ELSE->Fill(6.5); //TRIG_EARLY_LARGE_DATA_T 0b111
	  break;
	default:
	  std::cout << "unhandled type: " << type << std::endl;
	  break;
	}
        // if( p->iValue(wf,"TYPE")==0 ){Packet_Type_Fraction_HB->Fill(0.5);} //HEARTBEAT_T 0b000
        // if( p->iValue(wf,"TYPE")==1 ){Packet_Type_Fraction_ELSE->Fill(1.5);} //TRUNCATED_DATA_T 0b001
        // if( p->iValue(wf,"TYPE")==3 ){Packet_Type_Fraction_ELSE->Fill(2.5);} //TRUNCATED_TRIG_EARLY_DATA_T 0b011
        // if( p->iValue(wf,"TYPE")==4 ){Packet_Type_Fraction_NORM->Fill(3.5);} //NORMAL_DATA_T 0b100
        // if( p->iValue(wf,"TYPE")==5 ){Packet_Type_Fraction_ELSE->Fill(4.5);} //LARGE_DATA_T 0b101
        // if( p->iValue(wf,"TYPE")==6 ){Packet_Type_Fraction_ELSE->Fill(5.5);} //TRIG_EARLY_DATA_T 0b110
        // if( p->iValue(wf,"TYPE")==7 ){Packet_Type_Fraction_ELSE->Fill(6.5);} //TRIG_EARLY_LARGE_DATA_T 0b111

        const int n_tagger = p->lValue(0, "N_TAGGER");
        for (int t = 0; t < n_tagger; t++)
        {
          const bool is_lvl1_tagger( static_cast<uint8_t>(p->lValue(t, "IS_LEVEL1_TRIGGER" )));
          if( is_lvl1_tagger ){ LVL_1_TAGGER_per_EBDC->Fill(MonitorServerId()); }
	}

        int fee = p->iValue(wf, "FEE");
        int sampaAddress = p->iValue(wf, "SAMPAADDRESS");
        int checksumError = p->iValue(wf, "CHECKSUMERROR");
        int parityError = p->iValue(wf, "DATAPARITYERROR");
        int channel = p->iValue(wf, "CHANNEL");

        if( p->iValue(wf,"TYPE")!=0 ){
          Check_Sums->Fill(FEE_transform[fee]*8 + sampaAddress); 
          if( checksumError == 1){Check_Sum_Error->Fill(FEE_transform[fee]*8 + sampaAddress);}
          if( parityError == 1){Parity_Error->Fill(FEE_transform[fee]*8 + sampaAddress);}
        }
 
        if( (checksumError == 0 && parityError == 0) &&  p->iValue(wf,"TYPE")!= 0){Channels_in_Packet->Fill(channel + (256*FEE_transform[fee]));} // do not fill for heartbeat WFs

        int nr_Samples = p->iValue(wf, "SAMPLES");
        sample_size_hist->Fill(nr_Samples);


        // clockwise FEE mapping
        //int FEE_map[26]={5, 6, 1, 3, 2, 12, 10, 11, 9, 8, 7, 1, 2, 4, 8, 7, 6, 5, 4, 3, 1, 3, 2, 4, 6, 5};
        int FEE_R[26]={2, 2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 2, 2, 1, 2, 2, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3};
        // counter clockwise FEE mapping (From Takao - DEPRECATED AS OF 08.29)
        //int FEE_map[26]={3, 2, 5, 3, 4, 0, 2, 1, 3, 4, 5, 7, 6, 2, 0, 1, 0, 1, 4, 5, 11, 9, 10, 8, 6, 7};

        // FEE mapping from Jin
        int FEE_map[26]={4, 5, 0, 2, 1, 11, 9, 10, 8, 7, 6, 0, 1, 3, 7, 6, 5, 4, 3, 2, 0, 2, 1, 3, 5, 4};

        
        //int pads_per_sector[3] = {96, 128, 192};


        serverid = MonitorServerId();

        // setting the mapp of the FEE
        int feeM = FEE_map[fee];
        if(FEE_R[fee]==2) feeM += 6;
        if(FEE_R[fee]==3) feeM += 14;

        // getting R and Phi coordinates
        double R = M.getR(feeM, channel);
        double padphi = 0;

        if( side(serverid) == 0 ) //NS
        {
          padphi =  M.getPad(feeM, channel) + (serverid ) * (2304./12.); 
        }
        else if( side(serverid) == 1 ) //SS
        {
          padphi = -M.getPad(feeM, channel) - (serverid-12) * (2304./12) ; 
        }

        int layer = M.getLayer(feeM, channel) + (serverid);
        double phi = 0;

        //double phi = M.getPhi(feeM, channel) + (serverid - 12*side(serverid)) * M_PI / 6 ;

        if( side(serverid) == 0 ) //NS
        {
          phi = M.getPhi(feeM, channel) + (serverid ) * M_PI / 6 ; 
        }
        else if( side(serverid) == 1 ) //SS
        {
          phi = M.getPhi(feeM, channel) + (18 - serverid ) * M_PI / 6 ; 
        }

        //std::cout<<"Sector = "<< serverid <<" FEE = "<<fee<<" channel = "<<channel<<std::endl;

        //int mid = floor(360/2); //get median sample from 0-360 (we are assuming the sample > 360 is not useful to us as of 05.01.24)
        int num_of_nonZS_samples = 0; //start counter from 0

        int tr_samp = 0;
        int start_flag = 0;
        int prev_sample = 65000;
        int first_non_ZS_sample = 1;

        if( nr_Samples > 0)
        {
          //if( (p->iValue(wf,mid) == p->iValue(wf,mid-1)) && (p->iValue(wf,mid) == p->iValue(wf,mid-2)) && (p->iValue(wf,mid) == p->iValue(wf,mid+1)) && (p->iValue(wf,mid) == p->iValue(wf,mid+2)) )     
          //{
	  //is_channel_stuck = 1;
          //}

          for( int si=0;si < nr_Samples; si++ ) //get pedestal and noise before hand
          {
            if( (p->iValue(wf,si)) < 1025 && prev_sample > 64500) //start condition to record 
            { 
              start_flag = 1;
              if(first_non_ZS_sample == 1){First_ADC_vs_First_Time_Bin->Fill(si,(p->iValue(wf,si)));first_non_ZS_sample = 0;} //this is the first sample, its all we want
            } 
            if( (p->iValue(wf,si)) > 64500 && prev_sample < 1025){ tr_samp = 0; start_flag = 0; prev_sample =  (p->iValue(wf,si)); }  // end condition to record
            if( start_flag == 1){ ZS_Trigger_ADC_vs_Sample->Fill(tr_samp, p->iValue(wf,si)); tr_samp++; prev_sample = p->iValue(wf,si);} // record the ZS trigger histo if you should
            
	    if( (p->iValue(wf,si)) > 64500 && si > 1023){ break; } //for new firmware/ZS mode - we don't entries w/ ADC > 65 K after 1023 (50 us window), that's nonsense - per Jin's suggestion once you see this, BREAK out of loop
            if( (p->iValue(wf,si)) > 64500 ){ continue; }  //only use reasonable values to calculate median
            median_and_stdev_vec.push_back(p->iValue(wf,si));
            num_of_nonZS_samples++; 
          }
        } //Compare 5 values to determine stuck !!

        Num_non_ZS_channels_vs_SAMPA->Fill(sampaAddress + (8*FEE_transform[fee]),num_of_nonZS_samples);

        if( median_and_stdev_vec.size() == 0 ) // if all waveform values were 65 K
        { 
	  //std::cout<<"All values skipped, Event # "<<evtcnt<<std::endl;        
          is_channel_stuck = 0; //reset after looping through waveform samples
          store_ten.clear(); //clear this after every waveform
          median_and_stdev_vec.clear(); //clear this after every waveform
          continue; 
        }

        std::pair<float, float> result = calculateMedianAndStdDev(median_and_stdev_vec);
	//std::cout<<"pedestal = "<<result.first<<", RMS = "<<result.second<<" ADC, fee: "<<fee<<", channel: "<<channel<<", layer: "<<layer<<", phi: "<<phi<<", event num: "<<evtcnt<<std::endl;
        float pedestal = result.first; //average/pedestal -- based on MEDIAN OF ALL ENTRIES NOW, NOT MEAN OF FIRST 10 (02/12/24)
        float noise = result.second; //stdev - BASED ON REASONABLE SIGMA OF ENTRIES THAT ARE +/- 40 ADC WITHIN PEDESTAL

        float rawnoise =  calculateRawStdDev(median_and_stdev_vec);

        if(rawnoise==0. && median_and_stdev_vec.size() > 1 )
        {
          //for( int si=0;si < nr_Samples; si++ ){ std::cout<<"SAMPLE: "<<si<<", ADC: "<< p->iValue(wf,si) << std::endl; } 
	  stuck_channel_count[channel][FEE_transform[fee]]++;  // if the RMS is 0, this channel must be stuck
          if(stuck_channel_count[channel][FEE_transform[fee]] == 1){ Stuck_Channels->Fill(FEE_transform[fee]); } // only count # of unique channels in FEE that get stuck at least once
          is_channel_stuck = 1;
        } 

        int wf_max = 0;
        int t_max = 0;

        float pedest_sub_wf_max = 0.;

        int wf_max_laser_peak = 0;
        float pedest_sub_wf_max_laser_peak = 0.;

        int num_samples_over_threshold = 0;
     
        for( int s =0; s < nr_Samples ; s++ )
        {
	  //std::cout<<"MADE IT TO START OF sample LOOP, "<<"current sample = "<<s<<", total sample = "<<nr_Samples<<" EVENT "<<evtcnt<<std::endl;          
          //int t = s + 2 * (current_BCO - starting_BCO);

          int adc = p->iValue(wf,s);

          if( ((adc-pedestal) > 300 && (adc < 1500)) && s < 360){
	    //std::cout<<"FEE #: "<<fee<<", channel #: "<<channel<<", sample: "<<s<<", adc-pedestal: "<<(adc-pedestal)<<std::endl;
            Noise_Channel_Plots->Fill(channel + (256*FEE_transform[fee]));
          } // only intended to be looked at during pedestal runs with 360 time samples

	  //std::cout<<"adc = "<<adc<<" ADC, FEE = "<<fee<<", channel: "<<channel<<", layer: "<<layer<<", phi: "<<phi<<", event num: "<<evtcnt<<std::endl;

          if( adc > 64500 && s > 1023 ) { break;} //for new firmware/ZS mode - we don't entries w/ ADC > 65 K after 1023 (50 us), that's nonsense - per Jin's suggestion once you see this, BREAK out of loop
          else if( adc > 64500 ) { continue; } // we do not care about 65K ADC entries - ignore them

          Layer_ChannelPhi_ADC_weighted->Fill(padphi,layer,adc-pedestal);

          if( adc > wf_max){ wf_max = adc; t_max = s; pedest_sub_wf_max = adc - pedestal;}

          if( (s> 410 && s < 422) && (adc > wf_max_laser_peak) ){ wf_max_laser_peak = adc; pedest_sub_wf_max_laser_peak = adc - pedestal; }   

          if( (store_ten.size() < 10) ) // get first 10
          {
            store_ten.push_back(adc);
          }
          else  
          {

            //nine_max = Max_Nine(p->iValue(wf,s-9),p->iValue(wf,s-8),p->iValue(wf,s-7),p->iValue(wf,s-6),p->iValue(wf,s-5),p->iValue(wf,s-4),p->iValue(wf,s-3),p->iValue(wf,s-2),p->iValue(wf,s-1)); //take the previous 9 numbers

            //VERY IMPORTANT, erase first entry, push_back current
            store_ten.erase(store_ten.begin());
            store_ten.push_back(adc);
             
            int max_of_previous_10 = *max_element(store_ten.begin(), store_ten.end());

            if((adc == max_of_previous_10 && checksumError == 0) && (parityError == 0 && is_channel_stuck == 0))//if the new value is greater than the previous 9
            {
               MAXADC->Fill(adc - pedestal,Module_ID(fee)); 
               if(Module_ID(fee)==0){MAXADC_1D_R1->Fill(adc - pedestal);} //Raw 1D for R1
               else if(Module_ID(fee)==1){MAXADC_1D_R2->Fill(adc - pedestal);} //Raw 1D for R2
               else if(Module_ID(fee)==2){MAXADC_1D_R3->Fill(adc - pedestal);} //Raw 1D for R3
            }

          }

          //if( R < 290 ){std::cout<<"R = "<<R<<" mm, layer:" <<layer<<std::endl; ADC_vs_SAMPLE -> Fill(s, adc);}

          if( (checksumError == 0 && parityError == 0) && is_channel_stuck == 0)
          {
            ADC_vs_SAMPLE -> Fill(s, adc);
            PEDEST_SUB_ADC_vs_SAMPLE -> Fill(s, adc-pedestal);
            ADC_vs_SAMPLE_large -> Fill(s, adc);

            if(Module_ID(fee)==0){RAWADC_1D_R1->Fill(adc);PEDEST_SUB_1D_R1->Fill(adc-pedestal);PEDEST_SUB_ADC_vs_SAMPLE_R1->Fill(s,adc-pedestal);} //Raw/pedest_sub 1D for R1
            if(Module_ID(fee)==1){RAWADC_1D_R2->Fill(adc);PEDEST_SUB_1D_R2->Fill(adc-pedestal);PEDEST_SUB_ADC_vs_SAMPLE_R2->Fill(s,adc-pedestal);} //Raw/pedest_sub 1D for R2
            if(Module_ID(fee)==2){RAWADC_1D_R3->Fill(adc);PEDEST_SUB_1D_R3->Fill(adc-pedestal);PEDEST_SUB_ADC_vs_SAMPLE_R3->Fill(s,adc-pedestal);} //Raw/pedest_sub 1D for R3

            if(Module_ID(fee)==0 && ((adc-pedestal) > std::max(5.0*noise,20.)) && layer != 0){COUNTS_vs_SAMPLE_1D_R1->Fill(s);} //Drift window in R1
	    if(Module_ID(fee)==1 && ((adc-pedestal) > std::max(5.0*noise,20.)) && layer != 0){COUNTS_vs_SAMPLE_1D_R2->Fill(s);} //Drift window in R2
	    if(Module_ID(fee)==2 && ((adc-pedestal) > std::max(5.0*noise,20.)) && layer != 0){COUNTS_vs_SAMPLE_1D_R3->Fill(s);} //Drift window in R3

            //if(Module_ID(fee)==0 && (layer != 0)){COUNTS_vs_SAMPLE_1D_R1->Fill(s);} //Drift window in R1
	    //if(Module_ID(fee)==1 && (layer != 0)){COUNTS_vs_SAMPLE_1D_R2->Fill(s);} //Drift window in R2
	    //if(Module_ID(fee)==2 && (layer != 0)){COUNTS_vs_SAMPLE_1D_R3->Fill(s);} //Drift window in R3

            if( (adc-pedestal) > 25 ){ num_samples_over_threshold++ ;}
          }

          //increment 
          if(serverid >= 0 && serverid < 12 ){ North_Side_Arr[ Index_from_Module(serverid,fee) ] += adc;}
          else {South_Side_Arr[ Index_from_Module(serverid,fee)-36 ] += adc;}

	  //std::cout<<"MADE IT TO END OF sample LOOP, "<<"current sample = "<<s<<", total sample = "<<nr_Samples<<" EVENT "<<evtcnt<<std::endl;
        } //nr samples

        //for streaker diagnostic:
        if( num_samples_over_threshold > 15 ){ NStreaks_vs_EventNo->Fill(evtcnt); }

        //for complicated XY stuff ____________________________________________________
        //20 = 3-5 * sigma - hard-coded
        // OR 10*noise = 10 sigma

        float z = 0; //mm

        if( (serverid < 12 && (pedest_sub_wf_max) > std::max(5.0*noise,20.)) && layer != 0 )
        {
          if(Module_ID(fee)==0){NorthSideADC_clusterXY_R1->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);NorthSideADC_clusterXY_R1_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R1
          else if(Module_ID(fee)==1){NorthSideADC_clusterXY_R2->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);NorthSideADC_clusterXY_R2_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R2
          else if(Module_ID(fee)==2){NorthSideADC_clusterXY_R3->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);NorthSideADC_clusterXY_R3_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R3

	  //old gas (Ar:CF4 - 60:40)
          //if( t_max >= 10 && t_max <=255 ){z = 1030 - (t_max - 10)*(50 * 0.084);NorthSideADC_clusterZY->Fill(z,R*sin(phi),pedest_sub_wf_max);NorthSideADC_clusterZY_unw->Fill(z,R*sin(phi));}
          //new gas (Ar:CF4:ISO - 75:20:5)
          if( t_max >= 40 && t_max <=320 ){z = 1030 - (t_max - 40)*(50 * 0.0735);NorthSideADC_clusterZY->Fill(z,R*sin(phi),pedest_sub_wf_max);NorthSideADC_clusterZY_unw->Fill(z,R*sin(phi));}
        }
        else if( (serverid >=12 && (pedest_sub_wf_max) > std::max(5.0*noise,20.)) && layer != 0)
        {
          if(Module_ID(fee)==0){SouthSideADC_clusterXY_R1->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);SouthSideADC_clusterXY_R1_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R1
          else if(Module_ID(fee)==1){SouthSideADC_clusterXY_R2->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);SouthSideADC_clusterXY_R2_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R2
          else if(Module_ID(fee)==2){SouthSideADC_clusterXY_R3->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max);SouthSideADC_clusterXY_R3_unw->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R3

          //old gas (Ar:CF4 - 60:40)
          //if( t_max >= 10 && t_max <=255 ){z = -1030 + (t_max - 10)*(50 * 0.084);SouthSideADC_clusterZY->Fill(z,R*sin(phi),pedest_sub_wf_max);SouthSideADC_clusterZY_unw->Fill(z,R*sin(phi));}
          //new gas (Ar:CF4:ISO - 75:20:5)
          if( t_max >= 40 && t_max <=320 ){z = -1030 + (t_max - 40)*(50 * 0.0735);SouthSideADC_clusterZY->Fill(z,R*sin(phi),pedest_sub_wf_max);SouthSideADC_clusterZY_unw->Fill(z,R*sin(phi));}
        }
        //________________________________________________________________________________
        //XY laser peak
        if( (serverid < 12 && (pedest_sub_wf_max_laser_peak) > std::max(5.0*noise,20.)) && ((t_max > 410 && t_max < 422) && (layer != 0))) // only fill if the laser was the max
        {
          if(Module_ID(fee)==0){NorthSideADC_clusterXY_R1_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R1
          else if(Module_ID(fee)==1){NorthSideADC_clusterXY_R2_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R2
          else if(Module_ID(fee)==2){NorthSideADC_clusterXY_R3_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R3
        }
        else if( (serverid >=12 && (pedest_sub_wf_max_laser_peak) > std::max(5.0*noise,20.)) && ((t_max > 410 && t_max < 422) && (layer != 0))) // only fill if the laser was the max
        {
          if(Module_ID(fee)==0){SouthSideADC_clusterXY_R1_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R1
          else if(Module_ID(fee)==1){SouthSideADC_clusterXY_R2_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R2
          else if(Module_ID(fee)==2){SouthSideADC_clusterXY_R3_LASER->Fill(R*cos(phi),R*sin(phi),pedest_sub_wf_max_laser_peak);} //Raw 1D for R3
        }
        //________________________________________________________________________________
        //5 event displays
        if( (serverid < 12 && (pedest_sub_wf_max) > std::max(5.0*noise,20.)) && layer != 0 )
        {
          if(Module_ID(fee)==0){NorthSideADC_clusterXY_R1_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R1
          else if(Module_ID(fee)==1){NorthSideADC_clusterXY_R2_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R2
          else if(Module_ID(fee)==2){NorthSideADC_clusterXY_R3_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R3
        }
        if( (serverid >= 12 && (pedest_sub_wf_max) > std::max(5.0*noise,20.)) && layer != 0 )
        {
          if(Module_ID(fee)==0){SouthSideADC_clusterXY_R1_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R1
          else if(Module_ID(fee)==1){SouthSideADC_clusterXY_R2_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R2
          else if(Module_ID(fee)==2){SouthSideADC_clusterXY_R3_u5->Fill(R*cos(phi),R*sin(phi));} //Raw 1D for R3
        }
        //________________________________________________________________________________


        is_channel_stuck = 0; //reset after looping through waveform samples

        store_ten.clear(); //clear this after every waveform
        
        median_and_stdev_vec.clear(); //clear this after every waveform
	//std::cout<<"MADE IT TO END OF WF LOOP, "<<"current wf = "<<wf<<", total wf = "<<nr_of_waveforms<<" EVENT "<<evtcnt<<std::endl;

      } //nr waveforms
      //std::cout<<"ABOUT TO DELETE THE EXISTING PACKET, EVENT # "<<evtcnt<<std::endl;
      delete p;
      //std::cout<<"JUST DELETETED THE EXISTING PACKET, EVENT # "<<evtcnt<<std::endl;
    } // if packet exists
    //std::cout<<"MADE IT TO END OF PACKET LOOP, EVENT # "<<evtcnt<<std::endl;
  } //end of packet loop
  //} //debug if evt >=1203
  evtcnt++;
  if(evtcnt5 < 5) //increment 5 event counter for 5 events
  { 
    evtcnt5++;
  }
  else if(evtcnt5 >= 5) // reset to to 0 when get to event 5 (6th event starting from 0)
  {
    evtcnt5 = 0;
    NorthSideADC_clusterXY_R1_u5->Reset();
    NorthSideADC_clusterXY_R2_u5->Reset();
    NorthSideADC_clusterXY_R3_u5->Reset();

    SouthSideADC_clusterXY_R1_u5->Reset();
    SouthSideADC_clusterXY_R2_u5->Reset();
    SouthSideADC_clusterXY_R3_u5->Reset();    
  }

  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("tpchist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
 //fill the TPC module displays
  float r, theta;

  //dummy data
  //float North_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };
  //float South_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };

  for(int tpciter = 1; tpciter < 73 ; tpciter++)
  {
    Locate(tpciter, &r, &theta);
    //std::cout << "r is: "<< r <<" theta is: "<< theta <<"\n";
    if(tpciter < 37)
    { //South side
      NorthSideADC->Fill(theta,r, North_Side_Arr[tpciter-1]); //fill South side with the weight = bin content
    }
    else 
    { //North side
      SouthSideADC->Fill(theta,r,South_Side_Arr[tpciter-37]); //fill North side with the weight = bin content
    }
  } //end loop over tpciter

  return 0;
} // end PROCESS_EVENT FUNCTION

int TpcMon::Module_ID(int fee_id) //for simply determining which module you are in (doesn't care about sector)
{
  int mod_id;
 
  if( fee_id == 2 || fee_id == 3 || fee_id == 4 || fee_id == 13 || fee_id == 16 || fee_id == 17 ){mod_id = 0;} // R1 

  else if( fee_id == 0 || fee_id == 1 || fee_id == 11 || fee_id == 12 || fee_id == 14 || fee_id == 15 || fee_id == 18 || fee_id == 19 ){mod_id = 1;} // R2

  else if( fee_id == 5 || fee_id == 6 || fee_id ==7 || fee_id == 8 || fee_id == 9 || fee_id == 10 || fee_id == 20 || fee_id == 21 || fee_id == 22 || fee_id == 23 || fee_id == 24 || fee_id == 25 ){mod_id = 2;} // R3

  else mod_id = 0;

  return mod_id;
}


int TpcMon::Index_from_Module(int sec_id, int fee_id) //for placing in the array (takes into account sector)
{
  int mod_id;
 
  if( fee_id == 2 || fee_id == 3 || fee_id == 4 || fee_id == 13 || fee_id == 16 || fee_id == 17 ){mod_id = 3*sec_id + 0;} // R1 

  else if( fee_id == 0 || fee_id == 1 || fee_id == 11 || fee_id == 12 || fee_id == 14 || fee_id == 15 || fee_id == 18 || fee_id == 19 ){mod_id = 3*sec_id + 1;} // R2

  else if( fee_id == 5 || fee_id == 6 || fee_id ==7 || fee_id == 8 || fee_id == 9 || fee_id == 10 || fee_id == 20 || fee_id == 21 || fee_id == 22 || fee_id == 23 || fee_id == 24 || fee_id == 25 ){mod_id = 3*sec_id + 2;} // R3

  else mod_id = 0;

  return mod_id;
}

void TpcMon::Locate(int id, float *rbin, float *thbin)
{
   float CSIDE_angle_bins[12] = { 0.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 6.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 }; //CCW from x = 0 (RHS horizontal)

   float ASIDE_angle_bins[12] = { 6.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 0.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12  }; //CCW from x = 0 (RHS horizontal)

   int modid3 = id % 3;

   switch(modid3) {
     case 1:
       *rbin = 0.4; //R1
       break;
     case 2:
       *rbin = 0.6; //R2
       break;
     case 0:
       *rbin = 0.8; //R3
       break;
   }

  if( id < 37){
    *thbin = CSIDE_angle_bins[TMath::FloorNint((id-1)/3)];
  }
  else if( id >= 37){
    *thbin = ASIDE_angle_bins[TMath::FloorNint((id-37)/3)];
  }
}

int TpcMon::Max_Nine(int one, int two, int three, int four, int five, int six, int seven, int eight, int nine)
{ 
  int max = 0;
  int nine_array[9] = {one, two, three, four, five, six, seven, eight, nine};

  for( int i = 0; i < 9; i++ )
  {
    if( nine_array[i] > max ){max = nine_array[i];}
  }

  return max;
}

bool TpcMon::side(int server_id)
{
  bool side_id = 0; // side = 0 for NS, side = 1 for SS
  if(server_id >= 12){side_id=1;} // side = 1 when serverid 12 or more
  
  return side_id;
}

std::pair<float, float> TpcMon::calculateMedianAndStdDev(const std::vector<int>& values) {

    // Calculate the median
    // first, sort
    std::vector<int> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());    
    size_t size = sortedValues.size();

    //std::cout<<"SIZE OF INPUT VEC = "<<size<<std::endl;

    float median;
    if (size % 2 == 0) 
    {
      median = (sortedValues[size / 2 - 1] + sortedValues[size / 2]) / 2.0;
    } 
    else 
    {
      median = sortedValues[size / 2];
    }

    std::vector<int> selectedValues;

    // Select values within the range of median +/- 40
    for (int value : values) {
      if (value >= median - 40 && value <= median + 40)
      {
        selectedValues.push_back(value);
      }
    }

    float stdDev = 3; // default answer is 3 ADC if nothing passes the +/- 40 ADC band for stdDev calc.

    if(selectedValues.size() > 0 )
    {   

      //Calculate Mean of selected values
      float sum = 0.0;
      for (int value : selectedValues) 
      {
        sum += value;
      }
      float mean = sum / selectedValues.size();
  
      // Calculate RMS of selected values
      float sumSquares = 0.0;
  
      // Calculate the standard deviation of selected values only
      for (int value : selectedValues)
      {
        float diff = value - mean;
        sumSquares += std::pow(diff, 2);
      }
      float variance = sumSquares / selectedValues.size();
      stdDev = std::sqrt(variance);

    }
    //std::cout<<"PED = "<< median << "ADC, NOISE = "<< stdDev <<" ADC"<<std::endl;
    return std::make_pair(median, stdDev);
}

float TpcMon::calculateRawStdDev(const std::vector<int>& values) {
   
    float stdDev = 3; // default answer is 3 ADC if somehow you don't have anything in the values vector 

    if(values.size() > 0 )
    {   

      //Calculate Mean of values
      float sum = 0.0;
      for (int value : values) 
      {
        sum += value;
      }
      float mean = sum / values.size();
  
      // Calculate RMS of selected values
      float sumSquares = 0.0;
  
      // Calculate the standard deviation of values
      for (int value : values)
      {
        float diff = value - mean;
        sumSquares += std::pow(diff, 2);
      }
      float variance = sumSquares / values.size();
      stdDev = std::sqrt(variance);

    }
    //std::cout<<"PED = "<< median << "ADC, NOISE = "<< stdDev <<" ADC"<<std::endl;
    return stdDev;
}

int TpcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}


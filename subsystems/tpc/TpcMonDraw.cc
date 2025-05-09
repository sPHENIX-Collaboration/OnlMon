#include "TpcMonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>
#include <TMath.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TString.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TEllipse.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <limits>
#include <vector>  // for vector

#include <cmath>
#include <cstdio>  // for printf
#include <string>  // for allocator, string, char_traits

TpcMonDraw::TpcMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int TpcMonDraw::Init()
{
  return 0;
}

int TpcMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "TPCModules")
  {
    TC[0] = new TCanvas(name.c_str(), "ADC Count by GEM Example", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    TC[0]->Divide(2,1);
    //gStyle->SetPalette(57); //kBird CVD friendly
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "TPCSampleSize")
  {
    TC[1] = new TCanvas(name.c_str(), "TPC Sample Size Distribution in Events", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[1]->Divide(4,7);
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "TPCCheckSumError")
  {
    TC[2] = new TCanvas(name.c_str(), "TPC CheckSumError Probability in Events",-1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[2]->Divide(4,7);
    transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillStyle(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
  }
  else if (name == "TPCADCSample")
  {
    TC[3] = new TCanvas(name.c_str(), "TPC ADC vs Sample in Whole Sector",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[3]->Divide(4,7);
    transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(false);
  }
  else if (name == "TPCMaxADCModule")
  {
    TC[4] = new TCanvas(name.c_str(), "(MAX ADC - pedestal) in SLIDING WINDOW for each Module in Sector", -1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[4]->Divide(4,7);
    transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(false);
  }
  else if (name == "TPCRawADC1D")
  {
    TC[5] = new TCanvas(name.c_str(), "TPC RAW ADC 1D distribution", -1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[5]->Divide(4,7);
    transparent[5] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[5]->SetFillStyle(4000);
    transparent[5]->Draw();
    TC[5]->SetEditable(false);
  }
  else if (name == "TPCMaxADC1D")
  {
    TC[6] = new TCanvas(name.c_str(), "(MAX ADC - pedestal) in SLIDING WINDOW 1D distribution",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[6]->Divide(4,7);
    transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
    transparent[6]->SetFillStyle(4000);
    transparent[6]->Draw();
    TC[6]->SetEditable(false);
  }
  else if (name == "TPCClusterXY")
  {
    TC[7] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)>  (20 ADC || 5sigma) for NS and SS, WEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[7]->Divide(2,1);
    // this one is used to plot the run number on the canvas
    transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
    transparent[7]->SetFillStyle(4000);
    transparent[7]->Draw();
    TC[7]->SetEditable(false);
  }
  else if (name == "TPCClusterXY_unw")
  {
    TC[8] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)>  (20 ADC || 5sigma) for NS and SS, UNWEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[8]->Divide(2,1);
    // this one is used to plot the run number on the canvas
    transparent[8] = new TPad("transparent8", "this does not show", 0, 0, 1, 1);
    transparent[8]->SetFillStyle(4000);
    transparent[8]->Draw();
    TC[8]->SetEditable(false);
  }
  else if (name == "TPCADCSamplelarge")
  {
    TC[9] = new TCanvas(name.c_str(), "TPC ADC vs Large Sample in Whole Sector",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[9]->Divide(4,7);
    transparent[9] = new TPad("transparent9", "this does not show", 0, 0, 1, 1);
    transparent[9]->SetFillStyle(4000);
    transparent[9]->Draw();
    TC[9]->SetEditable(false);
  }

  else if (name == "TPCClusterZY")
  {
    TC[10] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)> (20 ADC || 5sigma) for NS and SS, WEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[10]->Divide(1,1);
    // this one is used to plot the run number on the canvas
    transparent[10] = new TPad("transparent10", "this does not show", 0, 0, 1, 1);
    transparent[10]->SetFillStyle(4000);
    transparent[10]->Draw();
    TC[10]->SetEditable(false);
  }

  else if (name == "TPCClusterZY_unw")
  {
    TC[11] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)> (20 ADC || 5sigma) for NS and SS, UNWEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[11]->Divide(1,1);
    // this one is used to plot the run number on the canvas
    transparent[11] = new TPad("transparent11", "this does not show", 0, 0, 1, 1);
    transparent[11]->SetFillStyle(4000);
    transparent[11]->Draw();
    TC[11]->SetEditable(false);
  }

  else if (name == "TPCLayerPhi")
  {
    TC[12] = new TCanvas(name.c_str(), "Layer vs Channel Phi for NS and SS, WEIGHTED by Sum(ADC-pedestal)", -1, 0, 1350,700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[12]->Divide(1,1);
    // this one is used to plot the run number on the canvas
    transparent[12] = new TPad("transparent12", "this does not show", 0, 0, 1, 1);
    transparent[12]->SetFillStyle(4000);
    transparent[12]->Draw();
    TC[12]->SetEditable(false);
  }
  else if (name == "TPCPedestSubADC1D")
  {
    TC[13] = new TCanvas(name.c_str(), "TPC PEDEST SUB ADC 1D distribution", -1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[13]->Divide(4,7);
    transparent[13] = new TPad("transparent13", "this does not show", 0, 0, 1, 1);
    transparent[13]->SetFillStyle(4000);
    transparent[13]->Draw();
    TC[13]->SetEditable(false);
  }
  else if (name == "TPCNEventsEBDC")
  {
    TC[14] = new TCanvas(name.c_str(), "TPC NUMBER EVENTS vs EBDC",1350,700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[14]->Divide(1,1);
    // this one is used to plot the run number on the canvas
    transparent[14] = new TPad("transparent14", "this does not show", 0, 0, 1, 1);
    transparent[14]->SetFillStyle(4000);
    transparent[14]->Draw();
    TC[14]->SetEditable(false);
  }
  else if (name == "TPCPedestSubADCSample")
  {
    TC[15] = new TCanvas(name.c_str(), "TPC PEDEST SUB ADC vs Sample in Whole Sector",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[15]->Divide(4,7);
    transparent[15] = new TPad("transparent15", "this does not show", 0, 0, 1, 1);
    transparent[15]->SetFillStyle(4000);
    transparent[15]->Draw();
    TC[15]->SetEditable(false);
  } 
  else if (name == "TPCPedestSubADCSample_R1")
  {
    TC[16] = new TCanvas(name.c_str(), "TPC PEDEST SUB ADC vs Sample in R1 ONLY",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[16]->Divide(4,7);
    transparent[16] = new TPad("transparent16", "this does not show", 0, 0, 1, 1);
    transparent[16]->SetFillStyle(4000);
    transparent[16]->Draw();
    TC[16]->SetEditable(false);
  }
  else if (name == "TPCPedestSubADCSample_R2")
  {
    TC[17] = new TCanvas(name.c_str(), "TPC PEDEST SUB ADC vs Sample in R2 ONLY",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[17]->Divide(4,7);
    transparent[17] = new TPad("transparent17", "this does not show", 0, 0, 1, 1);
    transparent[17]->SetFillStyle(4000);
    transparent[17]->Draw();
    TC[17]->SetEditable(false);
  }   
  else if (name == "TPCPedestSubADCSample_R3")
  {
    TC[18] = new TCanvas(name.c_str(), "TPC PEDEST SUB ADC vs Sample in R3 ONLY",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[18]->Divide(4,7);
    transparent[18] = new TPad("transparent18", "this does not show", 0, 0, 1, 1);
    transparent[18]->SetFillStyle(4000);
    transparent[18]->Draw();
    TC[18]->SetEditable(false);
  }
  else if (name == "TPCClusterXY_laser")
  {
    TC[19] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)> (20 ADC || 5sigma) for NS and SS, LASER FLASH ONLY, WEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[19]->Divide(2,1);
    // this one is used to plot the run number on the canvas
    transparent[19] = new TPad("transparent19", "this does not show", 0, 0, 1, 1);
    transparent[19]->SetFillStyle(4000);
    transparent[19]->Draw();
    TC[19]->SetEditable(false);
  }      
  else if (name == "TpcMonServerStats")
  {
    TC[20] = new TCanvas(name.c_str(), "TpcMon Server Stats", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    transparent[20] = new TPad("transparent20", "this does not show", 0, 0, 1, 1);
    transparent[20]->Draw();
    transparent[20]->SetFillColor(kGray);
    TC[20]->SetEditable(false);
    TC[20]->SetTopMargin(0.05);
    TC[20]->SetBottomMargin(0.05);
  }
  else if (name == "TPCStuckChannels")
  {
    TC[21] = new TCanvas(name.c_str(), "TPC Stuck Channels in Events", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[21]->Divide(4,7);
    transparent[21] = new TPad("transparent21", "this does not show", 0, 0, 1, 1);
    transparent[21]->SetFillStyle(4000);
    transparent[21]->Draw();
    TC[21]->SetEditable(false);
  }
  else if (name == "TPCClusterXY_u5")
  {
    TC[22] = new TCanvas(name.c_str(), "(MAX ADC - pedestal)>  (20 ADC || 5sigma) for NS and SS, <= 5 EVENTS WEIGHTED", -1, 0, 1350, 700);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[22]->Divide(2,1);
    // this one is used to plot the run number on the canvas
    transparent[22] = new TPad("transparent22", "this does not show", 0, 0, 1, 1);
    transparent[22]->SetFillStyle(4000);
    transparent[22]->Draw();
    TC[22]->SetEditable(false);
  }     
  else if (name == "TPCChan_in_Packets_NS")
  {
    TC[23] = new TCanvas(name.c_str(), "TPC Channel Fraction Present in Packet in RCDAQ Event, NS ONLY", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[23]->Divide(2,7);
    transparent[23] = new TPad("transparent23", "this does not show", 0, 0, 1, 1);
    transparent[23]->SetFillStyle(4000);
    transparent[23]->Draw();
    TC[23]->SetEditable(false);
  }
  else if (name == "TPCChan_in_Packets_SS")
  {
    TC[24] = new TCanvas(name.c_str(), "TPC Channel Fraction Present in Packet in RCDAQ Event, sS ONLY", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[24]->Divide(2,7);
    transparent[24] = new TPad("transparent24", "this does not show", 0, 0, 1, 1);
    transparent[24]->SetFillStyle(4000);
    transparent[24]->Draw();
    TC[24]->SetEditable(false);
  }
  else if (name == "TPCNonZSChannels")
  {
    TC[25] = new TCanvas(name.c_str(), "TPC non ZS Channels in SAMPAs acrosss whole Sector",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[25]->Divide(4,7);
    transparent[25] = new TPad("transparent25", "this does not show", 0, 0, 1, 1);
    transparent[25]->SetFillStyle(4000);
    transparent[25]->Draw();
    TC[25]->SetEditable(false);
  }
  else if (name == "TPCNonZSTriggerADCvsSample")
  {
    TC[26] = new TCanvas(name.c_str(), "",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[26]->Divide(4,7);
    transparent[26] = new TPad("transparent26", "this does not show", 0, 0, 1, 1);
    transparent[26]->SetFillStyle(4000);
    transparent[26]->Draw();
    TC[26]->SetEditable(false);
  }
  else if (name == "TPCFirstADCvsFirstSample")
  {
    TC[27] = new TCanvas(name.c_str(), "",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[27]->Divide(4,7);
    transparent[27] = new TPad("transparent27", "this does not show", 0, 0, 1, 1);
    transparent[27]->SetFillStyle(4000);
    transparent[27]->Draw();
    TC[27]->SetEditable(false);
  }
  else if (name == "TPCDriftWindow")
  {
    TC[28] = new TCanvas(name.c_str(), "",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[28]->Divide(4,7);
    transparent[28] = new TPad("transparent28", "this does not show", 0, 0, 1, 1);
    transparent[28]->SetFillStyle(4000);
    transparent[28]->Draw();
    TC[28]->SetEditable(false);
  }
  else if (name == "TPCNStreakersvsEventNo")
  {
    TC[29] = new TCanvas(name.c_str(), "TPC Number of Horizontal Tracks vs Event No.", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[29]->Divide(1,2);
    transparent[29] = new TPad("transparent29", "this does not show", 0, 0, 1, 1);
    transparent[29]->SetFillStyle(4000);
    transparent[29]->Draw();
    TC[29]->SetEditable(false);
  }
  else if (name == "TPCPacketType")
  {
    TC[30] = new TCanvas(name.c_str(), "",-1, 0, xsize , ysize);
    gSystem->ProcessEvents();
    //gStyle->SetPalette(57); //kBird CVD friendly
    TC[30]->Divide(4,7);
    transparent[30] = new TPad("transparent30", "this does not show", 0, 0, 1, 1);
    transparent[30]->SetFillStyle(4000);
    transparent[30]->Draw();
    TC[30]->SetEditable(false);
  }
  else if (name == "TPCChan_per_LVL1_NS")
  {
    TC[31] = new TCanvas(name.c_str(), "TPC Channels per LVL_1 in RCDAQ Event, NS ONLY", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[31]->Divide(2,7);
    transparent[31] = new TPad("transparent31", "this does not show", 0, 0, 1, 1);
    transparent[31]->SetFillStyle(4000);
    transparent[31]->Draw();
    TC[31]->SetEditable(false);
  }
  else if (name == "TPCChan_per_LVL1_SS")
  {
    TC[32] = new TCanvas(name.c_str(), "TPC Channels per LVL_1 in RCDAQ Event, SS ONLY", -1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[32]->Divide(2,7);
    transparent[32] = new TPad("transparent32", "this does not show", 0, 0, 1, 1);
    transparent[32]->SetFillStyle(4000);
    transparent[32]->Draw();
    TC[32]->SetEditable(false);
  }
  else if (name == "TPCParityError")
  {
    TC[33] = new TCanvas(name.c_str(), "TPC CheckSumError Probability in Events",-1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[33]->Divide(4,7);
    transparent[33] = new TPad("transparent33", "this does not show", 0, 0, 1, 1);
    transparent[33]->SetFillStyle(4000);
    transparent[33]->Draw();
    TC[33]->SetEditable(false);
  }     
  else if (name == "ShifterTPCDriftWindow")
  {
    TC[34] = new TCanvas(name.c_str(), "TPC Drift Window for Shifter Checks",-1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[34]->Divide(4,7);
    transparent[34] = new TPad("transparent34", "this does not show", 0, 0, 1, 1);
    transparent[34]->SetFillStyle(4000);
    transparent[34]->Draw();
    TC[34]->SetEditable(false);
  }
  else if (name == "TPCNoiseChannelsPlots")
  {
    TC[35] = new TCanvas(name.c_str(), "Noise Channel Check for nonZS events w/ no load in TPC",-1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[35]->Divide(4,7);
    transparent[35] = new TPad("transparent35", "this does not show", 0, 0, 1, 1);
    transparent[35]->SetFillStyle(4000);
    transparent[35]->Draw();
    TC[35]->SetEditable(false);
  }
  else if (name == "ShifterTPCTransmissionDist")
  {
    TC[36] = new TCanvas(name.c_str(), "Background Corrected Tranmission Distribution",-1, 0, xsize , ysize );
    gSystem->ProcessEvents();
    TC[36]->Divide(1,1);
    transparent[36] = new TPad("transparent36", "this does not show", 0, 0, 1, 1);
    transparent[36]->SetFillStyle(4000);
    transparent[36]->Draw();
    TC[36]->SetEditable(false);
  }          
  return 0;
}

int TpcMonDraw::Draw(const std::string &what)
{
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "TPCMODULE")
  {
    iret += DrawTPCModules(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCSAMPLESIZE")
  {
    iret += DrawTPCSampleSize(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCSTUCKCHANNELS")
  {
    iret += DrawTPCStuckChannels(what);
    idraw++; 
  }
  if (what == "ALL" || what == "TPCCHECKSUMERROR")
  {
    iret += DrawTPCCheckSum(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPARITYERROR")
  {
    iret += DrawTPCParity(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCADCVSSAMPLE")
  {
    iret += DrawTPCADCSample(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCMAXADCMODULE")
  {
    iret += DrawTPCMaxADCModule(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCRAWADC1D")
  {
    iret += DrawTPCRawADC1D(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCMAXADC1D")
  {
    iret += DrawTPCMaxADC1D(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCLUSTERSXYWEIGTHED")
  {
    iret += DrawTPCXYclusters(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCLUSTERSXYUNWEIGTHED")
  {
    iret += DrawTPCXYclusters_unweighted(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCADCVSSAMPLELARGE")
  {
    iret += DrawTPCADCSampleLarge(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCLUSTERSZYWEIGTHED")
  {
    iret += DrawTPCZYclusters(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCLUSTERSZYUNWEIGTHED")
  {
    iret += DrawTPCZYclusters_unweighted(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCHANNELPHI_LAYER_WEIGHTED")
  {
    iret += DrawTPCchannelphi_layer_weighted(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPEDESTSUBADC1D")
  {
    iret += DrawTPCPedestSubADC1D(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCNEVENTSEBDC")
  {
    iret += DrawTPCNEventsvsEBDC(what);
    idraw++;
  } 
  if (what == "ALL" || what == "TPCPEDESTSUBADCVSSAMPLE")
  {
    iret += DrawTPCPedestSubADCSample(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPEDESTSUBADCVSSAMPLE_R1" )
  {
    iret += DrawTPCPedestSubADCSample_R1(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPEDESTSUBADCVSSAMPLE_R2" )
  {
    iret += DrawTPCPedestSubADCSample_R2(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPEDESTSUBADCVSSAMPLE_R3" )
  {
    iret += DrawTPCPedestSubADCSample_R3(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCLASERCLUSTERSXYWEIGTHED")
  {
    iret += DrawTPCXYlaserclusters(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCLUSTERS5EXYUNWEIGTHED")
  {
    iret +=  DrawTPCXYclusters5event(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCHANSINPACKETNS")
  {
    iret +=  DrawTPCChansinPacketNS(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCHANSINPACKETSS")
  {
    iret +=  DrawTPCChansinPacketSS(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCHANSPERLVL1NS")
  {
    iret +=  DrawTPCChansperLVL1_NS(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCCHANSPERLVL1SS")
  {
    iret +=  DrawTPCChansperLVL1_SS(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCNONZSCHANNELS")
  {
    iret += DrawTPCNonZSChannels(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCZSTRIGGERADCVSSAMPLE")
  {
    iret += DrawTPCZSTriggerADCSample(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCFIRSTNONZSADCVSFIRSTNONZSSAMPLE")
  {
    iret +=  DrawTPCFirstnonZSADCFirstnonZSSample(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCDRIFTWINDOW")
  {
    iret +=  DrawTPCDriftWindow(what);  
    idraw++;
  }
  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    idraw++;
  }
  if (what == "ALL" || what == "TPCNSTREAKERSVSEVENTNO")
  {
    iret += DrawTPCNStreaksvsEventNo(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCPACKETYPEFRACTION")
  {
    iret +=  DrawTPCPacketTypes(what);
    idraw++;
  }
  if (what == "ALL" || what == "SHIFTER_DRIFT_PLOT")
  {
    iret += DrawShifterTPCDriftWindow(what);
    idraw++;
  }
  if (what == "ALL" || what == "TPCNOISECHANNELPLOTS")
  {
    iret += DrawTPCNoiseChannelPlots(what);
    idraw++;
  }
  if (what == "ALL" || what == "SHIFTER_TRANSMISSION_PLOT")
  {
    iret += DrawShifterTransmissionDist(what); 
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int TpcMonDraw::DrawTPCModules(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *tpcmon_NSIDEADC_u[48] = {nullptr};
  TH2 *tpcmon_SSIDEADC_u[48] = {nullptr};

  TH2 *tpcmon_NSIDEADC[24] = {nullptr};
  TH2 *tpcmon_SSIDEADC[24] = {nullptr};
  
  char TPCMON_STR[100];
  // TPC ADC pie chart
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSIDEADC_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC");
    tpcmon_SSIDEADC_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC");
  }

  add_TH2(tpcmon_NSIDEADC_u, tpcmon_NSIDEADC);
  add_TH2(tpcmon_SSIDEADC_u, tpcmon_SSIDEADC);
  
  //TH2 *tpcmon_NSIDEADC1 = (TH2*) cl->getHisto("TPCMON_0","NorthSideADC");
  //TH2 *tpcmon_SSIDEADC1 = (TH2*) cl->getHisto("TPCMON_0","SouthSideADC");

  if (!gROOT->FindObject("TPCModules"))
  {
    MakeCanvas("TPCModules");
  }

  TCanvas *MyTC = TC[0];
  TPad *TransparentTPad = transparent[0];

  dummy_his1 = new TH2F("dummy_his1", "ADC Counts North Side", 100, -1.5, 1.5, 100, -1.5, 1.5); //dummy histos for titles
  dummy_his2 = new TH2F("dummy_his2", "ADC Counts South Side", 100, -1.5, 1.5, 100, -1.5, 1.5);

  //labels
  NS18 = new TPaveLabel( 1.046586,-0.1938999,1.407997,0.2144871, "18" );
  NS17 = new TPaveLabel( 0.962076,0.4382608,1.323487,0.8466479 , "17" );
  NS16 = new TPaveLabel( 0.4801947,0.8802139,0.8416056,1.288601 , "16" );
  NS15 = new TPaveLabel( -0.1823921,1.011681,0.1790189,1.425662, "15" );
  NS14 = new TPaveLabel( -0.8449788,0.8690253,-0.4835679,1.288601 , "14" );
  NS13 = new TPaveLabel( -1.30879,0.441058,-0.9473786,0.8550394 , "13" );
  NS12 = new TPaveLabel( -1.411009,-0.2050886,-1.049598,0.2144871, "12" );
  NS23 = new TPaveLabel( -1.302585,-0.7757116,-0.9471979,-0.3561359 , "23" );
  NS22 = new TPaveLabel( -0.8449788,-1.309971,-0.4835679,-0.8848013 , "22" );
  NS21 = new TPaveLabel( -0.1823921,-1.426557,0.1790189,-1.006982 , "21" );
  NS20 = new TPaveLabel( 0.4801947,-1.309076,0.8416056,-0.8839062 , "20" );
  NS19 = new TPaveLabel( 0.9622567,-0.7785088,1.323668,-0.3533387 , "19" );

  SS00 = new TPaveLabel( 1.046586,-0.1938999,1.407997,0.2144871, "00" );
  SS01 = new TPaveLabel( 0.962076,0.4382608,1.323487,0.8466479 , "01" );
  SS02 = new TPaveLabel( 0.4801947,0.8802139,0.8416056,1.288601 , "02" );
  SS03 = new TPaveLabel( -0.1823921,1.011681,0.1790189,1.425662, "03" );
  SS04 = new TPaveLabel( -0.8449788,0.8690253,-0.4835679,1.288601 , "04" );
  SS05 = new TPaveLabel( -1.30879,0.441058,-0.9473786,0.8550394 , "05" );
  SS06 = new TPaveLabel( -1.411009,-0.2050886,-1.049598,0.2144871, "06" );
  SS07 = new TPaveLabel( -1.302585,-0.7757116,-0.9471979,-0.3561359 , "07" );
  SS08 = new TPaveLabel( -0.8449788,-1.309971,-0.4835679,-0.8848013 , "08" );
  SS09 = new TPaveLabel( -0.1823921,-1.426557,0.1790189,-1.006982 , "09" );
  SS10 = new TPaveLabel( 0.4801947,-1.309076,0.8416056,-0.8839062 , "10" );
  SS11 = new TPaveLabel( 0.9622567,-0.7785088,1.323668,-0.3533387 , "11" );

  NS18->SetFillColor(0);
  NS17->SetFillColor(0);
  NS16->SetFillColor(0);
  NS15->SetFillColor(0);
  NS14->SetFillColor(0);
  NS13->SetFillColor(0);
  NS12->SetFillColor(0);
  NS23->SetFillColor(0);
  NS22->SetFillColor(0);
  NS21->SetFillColor(0);
  NS20->SetFillColor(0);
  NS19->SetFillColor(0);

  SS00->SetFillColor(0);
  SS01->SetFillColor(0);
  SS02->SetFillColor(0);
  SS03->SetFillColor(0);
  SS04->SetFillColor(0);
  SS05->SetFillColor(0);
  SS06->SetFillColor(0);
  SS07->SetFillColor(0);
  SS08->SetFillColor(0);
  SS09->SetFillColor(0);
  SS10->SetFillColor(0);
  SS11->SetFillColor(0);

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  MyTC->cd(1);
  gPad->SetTopMargin(0.15);
  gStyle->SetOptStat(0);
  dummy_his1->Draw("colpolzsame");
 
  float NS_max = 0;
  for( int i=0; i<12; i++ )
  {
    if( tpcmon_NSIDEADC[i] ){
    MyTC->cd(1);
    tpcmon_NSIDEADC[i] -> DrawCopy("colpolzsame");
    if( tpcmon_NSIDEADC[i]->GetBinContent(tpcmon_NSIDEADC[i]->GetMaximumBin()) > NS_max)
    {
      NS_max = tpcmon_NSIDEADC[i]->GetBinContent(tpcmon_NSIDEADC[i]->GetMaximumBin());
      dummy_his1->SetMaximum( NS_max );
    }
    gStyle->SetPalette(57); //kBird CVD friendly
    }

  }
  MyTC->Update();
  MyTC->cd(1);
  SS00->Draw("same");
  SS01->Draw("same");
  SS02->Draw("same");
  SS03->Draw("same");
  SS04->Draw("same");
  SS05->Draw("same");
  SS06->Draw("same");
  SS07->Draw("same");
  SS08->Draw("same");
  SS09->Draw("same");
  SS10->Draw("same");
  SS11->Draw("same");

  MyTC->cd(2);
  gPad->SetTopMargin(0.15);
  gStyle->SetOptStat(0);
  dummy_his2->Draw("colpolzsame");

  float SS_max = 0;
  for( int i=0; i<12; i++ )
  {
    if( tpcmon_SSIDEADC[i+12] ){
    //std::cout<<"tpcmon_SSIDEADC i: "<< i+12 <<std::endl;
    MyTC->cd(2);
    tpcmon_SSIDEADC[i+12] -> DrawCopy("colpolzsame");

    if ( tpcmon_SSIDEADC[i+12]->GetBinContent(tpcmon_SSIDEADC[i+12]->GetMaximumBin()) > SS_max)
    {
      SS_max = tpcmon_SSIDEADC[i+12]->GetBinContent(tpcmon_SSIDEADC[i+12]->GetMaximumBin());
      dummy_his2->SetMaximum( SS_max );
    }

    gStyle->SetPalette(57); //kBird CVD friendly
    }
  }
  MyTC->Update();

  MyTC->cd(2);
  NS18->Draw("same");
  NS17->Draw("same");
  NS16->Draw("same");
  NS15->Draw("same");
  NS14->Draw("same");
  NS13->Draw("same");
  NS12->Draw("same");
  NS23->Draw("same");
  NS22->Draw("same");
  NS21->Draw("same");
  NS20->Draw("same");
  NS19->Draw("same");


  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_SUMADC Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  //turn off stats box
  //dummy_his1->SetStats(0);
  //dummy_his2->SetStats(0);
  
  MyTC->Update();



  MyTC->Show();
  MyTC->SetEditable(false);
  
  return 0;
}

int TpcMonDraw::DrawTPCSampleSize(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_samplesizedist_u[48] = {nullptr};
  TH1 *tpcmon_samplesizedist[24] = {nullptr};

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_samplesizedist_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"sample_size_hist");
  }

  add_TH1(tpcmon_samplesizedist_u, tpcmon_samplesizedist);
  
  if (!gROOT->FindObject("TPCSampleSize"))
  {
    MakeCanvas("TPCSampleSize");
  }
  TCanvas *MyTC = TC[1];
  TPad *TransparentTPad = transparent[1];
  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_samplesizedist[i] )
    {
      MyTC->cd(i+5);
      tpcmon_samplesizedist[i]->DrawCopy("");
      gPad->SetLogx(kTRUE);
      gPad->SetLogy(kTRUE);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_SampleSize Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->SetLogx();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCCheckSum(const std::string & /* what */)
{

  OnlMonClient *cl = OnlMonClient::instance();
  
  TH1 *tpcmon_checksumerror[24] = {nullptr};
  TH1 *tpcmon_checksums[24] = {nullptr};

  TH1 *tpcmon_checksumerror_u[48] = {nullptr};
  TH1 *tpcmon_checksums_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_checksumerror_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Check_Sum_Error");
    tpcmon_checksums_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Check_Sums");
  }

  add_TH1(tpcmon_checksumerror_u, tpcmon_checksumerror);
  add_TH1(tpcmon_checksums_u, tpcmon_checksums);
  
  if (!gROOT->FindObject("TPCCheckSumError"))
  {
    MakeCanvas("TPCCheckSumError");
  }
  TCanvas *MyTC = TC[2];
  TPad *TransparentTPad = transparent[2];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  MyTC->cd(1);

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_checksumerror[i] && tpcmon_checksums[i] )
    {
      MyTC->cd(i+5);

      tpcmon_checksumerror[i]->Divide(tpcmon_checksums[i]);
      tpcmon_checksumerror[i]->GetYaxis()->SetRangeUser(0.0001,1.5);
      tpcmon_checksumerror[i]->DrawCopy("HIST");
    
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*8,-0.01,(j+1)*8,1.5);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*8+4,1.2,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(25,1.4,"R1");
      tt1->DrawText(77,1.4,"R2");
      tt1->DrawText(163,1.4,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(48.5,-0.01,48.5,1.5);
      t1->DrawLine(112.5,-0.01,112.5,1.5);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_CheckSumError Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  //MyTC->SetLogy();
  MyTC->Show();
  MyTC->SetEditable(false);


  return 0;
}

int TpcMonDraw::DrawTPCADCSample(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_ADCSAMPLE[24] = {nullptr};
  TH2 *tpcmon_ADCSAMPLE_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_ADCSAMPLE_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"ADC_vs_SAMPLE");
  }

  add_TH2(tpcmon_ADCSAMPLE_u, tpcmon_ADCSAMPLE);

  if (!gROOT->FindObject("TPCADCSample"))
  {
    MakeCanvas("TPCADCSample");
  }  

  TCanvas *MyTC = TC[3];
  TPad *TransparentTPad = transparent[3];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_ADCSAMPLE[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_ADCSAMPLE[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC_vs_SAMPLE Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCMaxADCModule(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_MAXADC_MODULE[24] = {nullptr};
  TH2 *tpcmon_MAXADC_MODULE_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_MAXADC_MODULE_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"MAXADC");
  }

  add_TH2(tpcmon_MAXADC_MODULE_u, tpcmon_MAXADC_MODULE);
  
  if (!gROOT->FindObject("TPCMaxADCModule"))
  {
    MakeCanvas("TPCMaxADCModule");
  }  

  TCanvas *MyTC = TC[4];
  TPad *TransparentTPad = transparent[4];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_MAXADC_MODULE[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_MAXADC_MODULE[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_MAXADC2D Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());



  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCRawADC1D(const std::string & /* what */)
{
  //std::cout<<"Made it inside DrawTPCRawADC1D"<<std::endl;
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_RAWADC1D[24][3] = {nullptr};
  TH1 *tpcmon_RAWADC1D_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_RAWADC1D_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"RAWADC_1D_R1");
    tpcmon_RAWADC1D_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"RAWADC_1D_R2");
    tpcmon_RAWADC1D_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"RAWADC_1D_R3");
  }

  add_TH1_modules( tpcmon_RAWADC1D_u, tpcmon_RAWADC1D );


  if (!gROOT->FindObject("TPCRawADC1D"))
  {
    MakeCanvas("TPCRawADC1D");
  }  

  TCanvas *MyTC = TC[5];
  TPad *TransparentTPad = transparent[5];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  auto legend = new TLegend(0.7,0.65,0.98,0.95);
  bool draw_leg = 0;

  for( int i=0; i<24; i++ )
  {
    MyTC->cd(i+5);
    for( int j = 2; j>-1; j-- )
    {
      if( tpcmon_RAWADC1D[i][j] )
      {
        if(j == 2){tpcmon_RAWADC1D[i][j] -> DrawCopy("HIST");}
        else      {tpcmon_RAWADC1D[i][j] -> DrawCopy("HISTsame");} //assumes that R3 will always exist and is most entries
      }
    }
    gPad->Update();
    gPad->SetLogy(kTRUE);  

    if(draw_leg == 0 && tpcmon_RAWADC1D[i][0] && tpcmon_RAWADC1D[i][1] && tpcmon_RAWADC1D[i][2]) //if you have not drawn the legend yet, draw it BUT ONLY ONCE
    {
      legend->AddEntry(tpcmon_RAWADC1D[i][0], "R1");
      legend->AddEntry(tpcmon_RAWADC1D[i][1], "R2");
      legend->AddEntry(tpcmon_RAWADC1D[i][2], "R3");
      MyTC->cd(i+5);
      legend->Draw();
      draw_leg = 1; //don't draw it again
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_RAWADC Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCMaxADC1D(const std::string & /* what */)
{
  //std::cout<<"Made it inside DrawTPCMaxADC1D"<<std::endl;
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_MAXADC1D[24][3] = {nullptr};
  TH1 *tpcmon_MAXADC1D_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_MAXADC1D_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"MAXADC_1D_R1");
    tpcmon_MAXADC1D_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"MAXADC_1D_R2");
    tpcmon_MAXADC1D_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"MAXADC_1D_R3");
  }

  add_TH1_modules(tpcmon_MAXADC1D_u, tpcmon_MAXADC1D);

  if (!gROOT->FindObject("TPCMaxADC1D"))
  {
    MakeCanvas("TPCMaxADC1D");
  }  

  TCanvas *MyTC = TC[6];
  TPad *TransparentTPad = transparent[6];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  auto legend = new TLegend(0.7,0.65,0.98,0.95);
  bool draw_leg = 0;

  for( int i=0; i<24; i++ )
  {
    MyTC->cd(i+5);
    for( int j = 2; j>-1; j-- )
    {
      if( tpcmon_MAXADC1D[i][j] )
      {
        if(j == 2){tpcmon_MAXADC1D[i][j] -> DrawCopy("HIST");}
        else      {tpcmon_MAXADC1D[i][j] -> DrawCopy("HISTsame");} //assumes that R3 will always exist and is max
      }
    }
    gPad->Update();
    gPad->SetLogy(kTRUE);
     
    if(draw_leg == 0 && tpcmon_MAXADC1D[i][0] && tpcmon_MAXADC1D[i][1] && tpcmon_MAXADC1D[i][2]) //if you have not drawn the legend yet, draw it BUT ONLY ONCE
    {
      MyTC->cd(i+5);
      legend->AddEntry(tpcmon_MAXADC1D[i][0], "R1");
      legend->AddEntry(tpcmon_MAXADC1D[i][1], "R2");
      legend->AddEntry(tpcmon_MAXADC1D[i][2], "R3");
      legend->Draw();
      draw_leg = 1; //don't draw it again
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_LOCALMAXADC-PEDESTAL Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);
  
  return 0;
}
int TpcMonDraw::DrawTPCXYclusters(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_clusXY[24][3] = {nullptr};
  TH2 *tpcmon_SSTPC_clusXY[24][3] = {nullptr};

  TH2 *tpcmon_NSTPC_clusXY_u[48][3] = {nullptr};
  TH2 *tpcmon_SSTPC_clusXY_u[48][3] = {nullptr};  
  
  dummy_his1_XY = new TH2F("dummy_his1_XY", "(ADC-Pedestal) > (5#sigma||20ADC) North Side, WEIGHTED", 400, -800, 800, 400, -800, 800); //dummy histos for titles
  dummy_his2_XY = new TH2F("dummy_his2_XY", "(ADC-Pedestal) > (5#sigma||20ADC) South Side, WEIGHTED", 400, -800, 800, 400, -800, 800);

  dummy_his1_XY->SetXTitle("X [mm]");
  dummy_his1_XY->SetYTitle("Y [mm]");
  dummy_his1_XY->GetYaxis()->SetTitleSize(0.02);

  dummy_his2_XY->SetXTitle("-X [mm]"); //SS x is flipped from global coordinates
  dummy_his2_XY->SetYTitle("Y [mm]");
  dummy_his2_XY->GetYaxis()->SetTitleSize(0.02);

  //the lines are for the sector boundaries
  Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;

  Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;

  Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;

  Double_t sec_phi = (0.5024 + 0.5097)/2.0;

  TLine *lines[12];

  for(int ln=0;ln<12;ln++)
  {
    lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
  }

  TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
  TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
  TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
  TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
  //__________________

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSTPC_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R1");
    tpcmon_NSTPC_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R2");
    tpcmon_NSTPC_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R3");

    tpcmon_SSTPC_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R1");
    tpcmon_SSTPC_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R2");
    tpcmon_SSTPC_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R3");
  }

  add_TH2_modules(tpcmon_NSTPC_clusXY_u, tpcmon_NSTPC_clusXY);
  add_TH2_modules(tpcmon_SSTPC_clusXY_u, tpcmon_SSTPC_clusXY);
  
  if (!gROOT->FindObject("TPCClusterXY"))
  {
    MakeCanvas("TPCClusterXY");
  }

  TCanvas *MyTC = TC[7];
  TPad *TransparentTPad = transparent[7];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC-Pedestal>(5sigma||20ADC) WEIGHTED, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his1_XY->Draw("colzsame");

  float NS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_NSTPC_clusXY[i][j] )
      {
        MyTC->cd(1);
        tpcmon_NSTPC_clusXY[i][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_NSTPC_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_clusXY[i][j]->GetMaximumBin()) > NS_max)
        {
          NS_max = tpcmon_NSTPC_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_clusXY[i][j]->GetMaximumBin());
          dummy_his1_XY->SetMaximum( NS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }

    }
  }
  MyTC->cd(1);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }
  MyTC->Update();

  MyTC->cd(2);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his2_XY->Draw("colzsame");

  float SS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_SSTPC_clusXY[i+12][j] )
      {
        //std::cout<<"South Side Custer XY i: "<< i+12 <<", j: "<<j<<std::endl;
        MyTC->cd(2);
        tpcmon_SSTPC_clusXY[i+12][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_SSTPC_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_clusXY[i+12][j]->GetMaximumBin()) > SS_max)
        {
          SS_max = tpcmon_SSTPC_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_clusXY[i+12][j]->GetMaximumBin());
          dummy_his2_XY->SetMaximum( SS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }
    }

  }
  MyTC->cd(2);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCXYclusters_unweighted(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_clusXY[24][3] = {nullptr};
  TH2 *tpcmon_SSTPC_clusXY[24][3] = {nullptr};

  TH2 *tpcmon_NSTPC_clusXY_u[48][3] = {nullptr};
  TH2 *tpcmon_SSTPC_clusXY_u[48][3] = {nullptr};
  
  dummy_his1_XY_unw = new TH2F("dummy_his1_XY_unw", "(ADC-Pedestal) > (5#sigma||20ADC) North Side, UNWEIGHTED", 400, -800, 800, 400, -800, 800); //dummy histos for titles
  dummy_his2_XY_unw = new TH2F("dummy_his2_XY_unw", "(ADC-Pedestal) > (5#sigma||20ADC) South Side, UNWEIGHTED", 400, -800, 800, 400, -800, 800);

  dummy_his1_XY_unw->SetXTitle("X [mm]");
  dummy_his1_XY_unw->SetYTitle("Y [mm]");
  dummy_his1_XY_unw->GetYaxis()->SetTitleSize(0.02);

  dummy_his2_XY_unw->SetXTitle("-X [mm]"); //SS x is flipped from global coordinates
  dummy_his2_XY_unw->SetYTitle("Y [mm]");
  dummy_his2_XY_unw->GetYaxis()->SetTitleSize(0.02);

  //the lines are for the sector boundaries
  Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;

  Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;

  Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;

  Double_t sec_phi = (0.5024 + 0.5097)/2.0;

  TLine *lines[12];

  for(int ln=0;ln<12;ln++)
  {
    lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
  }

  TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
  TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
  TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
  TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
  //__________________

  char TPCMON_STR[100];

  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSTPC_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R1_unw");
    tpcmon_NSTPC_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R2_unw");
    tpcmon_NSTPC_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R3_unw");

    tpcmon_SSTPC_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R1_unw");
    tpcmon_SSTPC_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R2_unw");
    tpcmon_SSTPC_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R3_unw");
  }

  add_TH2_modules(tpcmon_NSTPC_clusXY_u, tpcmon_NSTPC_clusXY);
  add_TH2_modules(tpcmon_SSTPC_clusXY_u, tpcmon_SSTPC_clusXY);
  
  if (!gROOT->FindObject("TPCClusterXY_unw"))
  {
    MakeCanvas("TPCClusterXY_unw");
  }  

  TCanvas *MyTC = TC[8];
  TPad *TransparentTPad = transparent[8];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC-Pedestal>(5sigma||20ADC), UNWEIGHTED, Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  //gPad->SetLogz(kTRUE);
  dummy_his1_XY_unw->Draw("colzsame");

  float NS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_NSTPC_clusXY[i][j] )
      {
        MyTC->cd(1);
        tpcmon_NSTPC_clusXY[i][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_NSTPC_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_clusXY[i][j]->GetMaximumBin()) > NS_max)
        {
          NS_max = tpcmon_NSTPC_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_clusXY[i][j]->GetMaximumBin());
          dummy_his1_XY_unw->SetMaximum( NS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }
    }

  }
  MyTC->cd(1);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }
  MyTC->Update();

  MyTC->cd(2);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  dummy_his2_XY_unw->Draw("colzsame");
  //gPad->SetLogz(kTRUE);

  float SS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_SSTPC_clusXY[i+12][j] )
      {
        //std::cout<<"South Side Custer XY i: "<< i+12 <<", j: "<<j<<std::endl;
        MyTC->cd(2);
        tpcmon_SSTPC_clusXY[i+12][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_SSTPC_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_clusXY[i+12][j]->GetMaximumBin()) > SS_max)
        {
          SS_max = tpcmon_SSTPC_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_clusXY[i+12][j]->GetMaximumBin());
          dummy_his2_XY_unw->SetMaximum( SS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }
    }

  }
  MyTC->cd(2);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }
  MyTC->Update();

  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCADCSampleLarge(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_ADCSAMPLE_large[24] = {nullptr};
  TH2 *tpcmon_ADCSAMPLE_large_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_ADCSAMPLE_large_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"ADC_vs_SAMPLE_large");
  }

  add_TH2(tpcmon_ADCSAMPLE_large_u, tpcmon_ADCSAMPLE_large);  

  if (!gROOT->FindObject("TPCADCSamplelarge"))
  {
    MakeCanvas("TPCADCSamplelarge");
  }  

  TCanvas *MyTC = TC[9];
  TPad *TransparentTPad = transparent[9];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_ADCSAMPLE_large[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_ADCSAMPLE_large[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC_vs_SAMPLE_large Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}



int TpcMonDraw::DrawTPCZYclusters(const std::string & /* what */)
{

  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_clusZY[24] = {nullptr};
  TH2 *tpcmon_SSTPC_clusZY[24] = {nullptr};

  TH2 *tpcmon_NSTPC_clusZY_u[48] = {nullptr};
  TH2 *tpcmon_SSTPC_clusZY_u[48] = {nullptr};
  
  dummy_his1_ZY = new TH2F("dummy_his1_ZY", "(ADC-Pedestal) > ((5#sigma||20ADC), WEIGHTED", 515, -1030, 1030, 400, -800, 800); //dummy histos for titles
  dummy_his1_ZY->SetXTitle("Z [mm]");
  dummy_his1_ZY->SetYTitle("Y [mm]");

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSTPC_clusZY_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterZY");

    tpcmon_SSTPC_clusZY_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterZY");
  }

  add_TH2(tpcmon_NSTPC_clusZY_u, tpcmon_NSTPC_clusZY);
  add_TH2(tpcmon_SSTPC_clusZY_u, tpcmon_SSTPC_clusZY);
  
  if (!gROOT->FindObject("TPCClusterZY"))
  {
    MakeCanvas("TPCClusterZY");
  }  

  TCanvas *MyTC = TC[10];
  TPad *TransparentTPad = transparent[10];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC-Pedestal>(5sigma||20ADC), WEIGHTED, Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his1_ZY->Draw("colzsame");


  float max = 0;
  for( int i=0; i<12; i++ )
  {
    if( tpcmon_NSTPC_clusZY[i] )
    {
      MyTC->cd(1);
      tpcmon_NSTPC_clusZY[i] -> Draw("colzsame");
      if ( tpcmon_NSTPC_clusZY[i]->GetBinContent(tpcmon_NSTPC_clusZY[i]->GetMaximumBin()) > max ) 
      {
        max = tpcmon_NSTPC_clusZY[i]->GetBinContent(tpcmon_NSTPC_clusZY[i]->GetMaximumBin());
        dummy_his1_ZY->SetMaximum( max );
      }
      gStyle->SetPalette(57); //kBird CVD friendly
    }

  }
  MyTC->Update();

  for( int i=0; i<12; i++ )
  {
    if( tpcmon_SSTPC_clusZY[i+12] )
    {
      MyTC->cd(1);
      tpcmon_SSTPC_clusZY[i+12] -> Draw("colzsame");
      if ( tpcmon_SSTPC_clusZY[i+12]->GetBinContent(tpcmon_SSTPC_clusZY[i+12]->GetMaximumBin()) > max ) 
      {
        max = tpcmon_SSTPC_clusZY[i+12]->GetBinContent(tpcmon_SSTPC_clusZY[i+12]->GetMaximumBin());
        dummy_his1_ZY->SetMaximum( max );
      }
      //gStyle->SetLogz(kTRUE);
      gStyle->SetPalette(57); //kBird CVD friendly
    }

  }

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);



  return 0;
}


int TpcMonDraw::DrawTPCZYclusters_unweighted(const std::string & /* what */)
{


  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_clusZY_unw[24] = {nullptr};
  TH2 *tpcmon_SSTPC_clusZY_unw[24] = {nullptr};

  TH2 *tpcmon_NSTPC_clusZY_unw_u[48] = {nullptr};
  TH2 *tpcmon_SSTPC_clusZY_unw_u[48] = {nullptr};
  
  dummy_his1_ZY_unw = new TH2F("dummy_his1_ZY_unw", "(ADC-Pedestal) > (5#sigma||20ADC), UNWEIGHTED", 515, -1030, 1030, 400, -800, 800); //dummy histos for titles
  dummy_his1_ZY_unw->SetXTitle("Z [mm]");
  dummy_his1_ZY_unw->SetYTitle("Y [mm]");

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSTPC_clusZY_unw_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterZY_unw");

    tpcmon_SSTPC_clusZY_unw_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterZY_unw");
  }

  add_TH2(tpcmon_NSTPC_clusZY_unw_u, tpcmon_NSTPC_clusZY_unw);
  add_TH2(tpcmon_SSTPC_clusZY_unw_u, tpcmon_SSTPC_clusZY_unw);
  
  if (!gROOT->FindObject("TPCClusterZY_unw"))
  {
    MakeCanvas("TPCClusterZY_unw");
  }  

  TCanvas *MyTC = TC[11];
  TPad *TransparentTPad = transparent[11];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC-Pedestal>(5sigma||20ADC), UNWEIGHTED, Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his1_ZY_unw->Draw("colzsame");

  float max = 0;
  for( int i=0; i<12; i++ )
  {
    if( tpcmon_NSTPC_clusZY_unw[i] )
    {
      MyTC->cd(1);
      tpcmon_NSTPC_clusZY_unw[i] -> Draw("colzsame");
      if ( tpcmon_NSTPC_clusZY_unw[i]->GetBinContent(tpcmon_NSTPC_clusZY_unw[i]->GetMaximumBin()) > max ) 
      {
        max = tpcmon_NSTPC_clusZY_unw[i]->GetBinContent(tpcmon_NSTPC_clusZY_unw[i]->GetMaximumBin());
        dummy_his1_ZY->SetMaximum( max );
      }
      //gStyle->SetLogz(kTRUE);
      gStyle->SetPalette(57); //kBird CVD friendly
    }

  }
  MyTC->Update();

  for( int i=0; i<12; i++ )
  {
    if( tpcmon_SSTPC_clusZY_unw[i+12] )
    {
      MyTC->cd(1);
      tpcmon_SSTPC_clusZY_unw[i+12] -> Draw("colzsame");
      if ( tpcmon_SSTPC_clusZY_unw[i+12]->GetBinContent(tpcmon_SSTPC_clusZY_unw[i+12]->GetMaximumBin()) > max ) 
      {
        max = tpcmon_SSTPC_clusZY_unw[i+12]->GetBinContent(tpcmon_SSTPC_clusZY_unw[i+12]->GetMaximumBin());
        dummy_his1_ZY->SetMaximum( max );
      }
      //gStyle->SetLogz(kTRUE);
      gStyle->SetPalette(57); //kBird CVD friendly
    }

  }
  MyTC->Update();

  MyTC->Show();
  MyTC->SetEditable(false);



  return 0;
}
int TpcMonDraw::DrawTPCchannelphi_layer_weighted(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmonth2channelphi_layer[24] = {nullptr};
  TH2 *tpcmonth2channelphi_layer_u[48] = {nullptr};
  
  char TPCMON_STR[100];

  dummy_his1_channelphi_layer_w = new TH2F("dummy_his1_channelphi_layer_", "(ADC-Pedestal) > 20 ADC, UNWEIGHTED", 515, -1030, 1030, 400, -800, 800); //dummy histos for titles
  dummy_his1_channelphi_layer_w->SetXTitle("#phi chan. #");
  dummy_his1_channelphi_layer_w->SetYTitle("layer ");
  dummy_his1_channelphi_layer_w->SetZTitle("#Sigma(ADC-ped.)");

  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmonth2channelphi_layer_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"Layer_ChannelPhi_ADC_weighted");
  }

  add_TH2(tpcmonth2channelphi_layer_u, tpcmonth2channelphi_layer);

  if (!gROOT->FindObject("TPCLayerPhi"))
  {
    MakeCanvas("TPCLayerPhi");
  }  

  TCanvas *MyTC = TC[12];
  TPad *TransparentTPad = transparent[12];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ChannelPhi_vs_Layer_ADC_weighted " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his1_channelphi_layer_w->Draw("lego2zsame");

  float max = 0;
  for( int i=0; i<24; i++ )
  {
    if(  tpcmonth2channelphi_layer[i] )
    {
      MyTC->cd(1);
       tpcmonth2channelphi_layer[i] -> Draw("lego2zsame");
      if (   tpcmonth2channelphi_layer[i]->GetBinContent(  tpcmonth2channelphi_layer[i]->GetMaximumBin()) > max ) 
      {
        max =  tpcmonth2channelphi_layer[i]->GetBinContent( tpcmonth2channelphi_layer[i]->GetMaximumBin());
        dummy_his1_channelphi_layer_w->SetMaximum( max );
      }
      //gStyle->SetLogz(kTRUE);
      gStyle->SetPalette(57); //kBird CVD friendly
    }

  }
  MyTC->Update();

  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPedestSubADC1D(const std::string & /* what */)
{
  //std::cout<<"Made it inside DrawTPCRawADC1D"<<std::endl;
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_PEDESTSUBADC1D[24][3] = {nullptr};
  TH1 *tpcmon_PEDESTSUBADC1D_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PEDESTSUBADC1D_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_1D_R1");
    tpcmon_PEDESTSUBADC1D_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_1D_R2");
    tpcmon_PEDESTSUBADC1D_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_1D_R3");
  }

  add_TH1_modules(tpcmon_PEDESTSUBADC1D_u, tpcmon_PEDESTSUBADC1D);
  
  if (!gROOT->FindObject("TPCPedestSubADC1D"))
  {
    MakeCanvas("TPCPedestSubADC1D");
  }  

  TCanvas *MyTC = TC[13];
  TPad *TransparentTPad = transparent[13];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  auto legend = new TLegend(0.7,0.65,0.98,0.95);
  bool draw_leg = 0;

  for( int i=0; i<24; i++ )
  {
    MyTC->cd(i+5);
    for( int j = 2; j>-1; j-- )
    {
      if( tpcmon_PEDESTSUBADC1D[i][j] )
      {
        if(j == 2){tpcmon_PEDESTSUBADC1D[i][j] -> DrawCopy("HIST");}
        else      {tpcmon_PEDESTSUBADC1D[i][j] -> DrawCopy("HISTsame");} //assumes that R3 will always exist and is most entries
      }
    }
    gPad->Update();
    gPad->SetLogy(kTRUE);  

    if(draw_leg == 0 && tpcmon_PEDESTSUBADC1D[i][0] && tpcmon_PEDESTSUBADC1D[i][1] && tpcmon_PEDESTSUBADC1D[i][2]) //if you have not drawn the legend yet, draw it BUT ONLY ONCE
    {
      legend->AddEntry(tpcmon_PEDESTSUBADC1D[i][0], "R1");
      legend->AddEntry(tpcmon_PEDESTSUBADC1D[i][1], "R2");
      legend->AddEntry(tpcmon_PEDESTSUBADC1D[i][2], "R3");
      MyTC->cd(i+5);
      legend->Draw();
      draw_leg = 1; //don't draw it again
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_PEDESTSUBADC Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCNEventsvsEBDC(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmoneventsebdc[24] = {nullptr};
  TH1 *tpcmoneventsebdc_u[48] = {nullptr};
  
  char TPCMON_STR[100];

  dummy_his1_NEvents_EBDC = new TH1F("dummy_his1_Nevents_EBDC", "N_{Events} vs EBDC", 24, -0.5, 23.5); //dummy histos for titles
  dummy_his1_NEvents_EBDC->SetXTitle("EBDC #");
  dummy_his1_NEvents_EBDC->SetYTitle("N_{Events}");

  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmoneventsebdc_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"NEvents_vs_EBDC");
  }

  add_TH1( tpcmoneventsebdc_u,  tpcmoneventsebdc);
  
  if (!gROOT->FindObject("TPCNEventsEBDC"))
  {
    MakeCanvas("TPCNEventsEBDC");
  }  

  TCanvas *MyTC = TC[14];
  TPad *TransparentTPad = transparent[14];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_N_Events_vs_EBDC " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  dummy_his1_NEvents_EBDC->Draw("HISTsame");

  float max = 0;
  for( int i=0; i<24; i++ )
  {
    if( tpcmoneventsebdc[i] )
    {
      MyTC->cd(1);
      tpcmoneventsebdc[i] -> Draw("HISTsame");
      if( tpcmoneventsebdc[i]->GetBinContent(  tpcmoneventsebdc[i]->GetMaximumBin()) > max ) 
      {
        max =  tpcmoneventsebdc[i]->GetBinContent( tpcmoneventsebdc[i]->GetMaximumBin());
        dummy_his1_NEvents_EBDC->SetMaximum( max*(1.3) );
      }

    }
  }
  MyTC->Update();

  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPedestSubADCSample(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_PEDESTSUBADCSAMPLE[24] = {nullptr};
  TH2 *tpcmon_PEDESTSUBADCSAMPLE_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PEDESTSUBADCSAMPLE_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_ADC_vs_SAMPLE");
  }

  add_TH2(tpcmon_PEDESTSUBADCSAMPLE_u, tpcmon_PEDESTSUBADCSAMPLE);  

  if (!gROOT->FindObject("TPCPedestSubADCSample"))
  {
    MakeCanvas("TPCPedestSubADCSample");
  }  

  TCanvas *MyTC = TC[15];
  TPad *TransparentTPad = transparent[15];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_PEDESTSUBADCSAMPLE[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_PEDESTSUBADCSAMPLE[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_PEDEST_SUB_ADC_vs_SAMPLE Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPedestSubADCSample_R1(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R1[24] = {nullptr};
  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R1_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PEDESTSUBADCSAMPLE_R1_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_ADC_vs_SAMPLE_R1");
  }

  add_TH2(tpcmon_PEDESTSUBADCSAMPLE_R1_u, tpcmon_PEDESTSUBADCSAMPLE_R1);

  if (!gROOT->FindObject("TPCPedestSubADCSample_R1"))
  {
    MakeCanvas("TPCPedestSubADCSample_R1");
  }  

  TCanvas *MyTC = TC[16];
  TPad *TransparentTPad = transparent[16];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_PEDESTSUBADCSAMPLE_R1[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_PEDESTSUBADCSAMPLE_R1[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_PEDEST_SUB_ADC_vs_SAMPLE R1 ONLY Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPedestSubADCSample_R2(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R2[24] = {nullptr};
  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R2_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PEDESTSUBADCSAMPLE_R2_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_ADC_vs_SAMPLE_R2");
  }

  add_TH2(tpcmon_PEDESTSUBADCSAMPLE_R2_u, tpcmon_PEDESTSUBADCSAMPLE_R2);
  
  if (!gROOT->FindObject("TPCPedestSubADCSample_R2"))
  {
    MakeCanvas("TPCPedestSubADCSample_R2");
  }  

  TCanvas *MyTC = TC[17];
  TPad *TransparentTPad = transparent[17];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_PEDESTSUBADCSAMPLE_R2[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_PEDESTSUBADCSAMPLE_R2[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_PEDEST_SUB_ADC_vs_SAMPLE R2 ONLY Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPedestSubADCSample_R3(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R3[24] = {nullptr};
  TH2 *tpcmon_PEDESTSUBADCSAMPLE_R3_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PEDESTSUBADCSAMPLE_R3_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"PEDEST_SUB_ADC_vs_SAMPLE_R3");
  }

  add_TH2(tpcmon_PEDESTSUBADCSAMPLE_R3_u, tpcmon_PEDESTSUBADCSAMPLE_R3);
  
  if (!gROOT->FindObject("TPCPedestSubADCSample_R3"))
  {
    MakeCanvas("TPCPedestSubADCSample_R3");
  }  

  TCanvas *MyTC = TC[18];
  TPad *TransparentTPad = transparent[18];

  MyTC->SetEditable(true);
  MyTC->Clear("D");

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_PEDESTSUBADCSAMPLE_R3[i] )
    {
      MyTC->cd(i+5);
      gStyle->SetPalette(57); //kBird CVD friendly
      gPad->SetLogz(kTRUE);
      tpcmon_PEDESTSUBADCSAMPLE_R3[i] -> DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_PEDEST_SUB_ADC_vs_SAMPLE R3 ONLY Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());


  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCXYlaserclusters(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_laser_clusXY[24][3] = {nullptr};
  TH2 *tpcmon_SSTPC_laser_clusXY[24][3] = {nullptr};

  TH2 *tpcmon_NSTPC_laser_clusXY_u[48][3] = {nullptr};
  TH2 *tpcmon_SSTPC_laser_clusXY_u[48][3] = {nullptr};
  
  dummy_his1_laser_XY = new TH2F("dummy_his1_laser_XY", "(ADC-Pedestal) > (5#sigma||20ADC) North Side, WEIGHTED", 400, -800, 800, 400, -800, 800); //dummy histos for titles
  dummy_his2_laser_XY = new TH2F("dummy_his2_laser_XY", "(ADC-Pedestal) > (5#sigma||20ADC) South Side, WEIGHTED", 400, -800, 800, 400, -800, 800);

  dummy_his1_laser_XY->SetXTitle("X [mm]");
  dummy_his1_laser_XY->SetYTitle("Y [mm]");
  dummy_his1_laser_XY->GetYaxis()->SetTitleSize(0.02);

  dummy_his2_laser_XY->SetXTitle("-X [mm]"); //SS x is flipped from global coordinates
  dummy_his2_laser_XY->SetYTitle("Y [mm]");
  dummy_his2_laser_XY->GetYaxis()->SetTitleSize(0.02);

  //the lines are for the sector boundaries
  Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;

  Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;

  Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;

  Double_t sec_phi = (0.5024 + 0.5097)/2.0;

  TLine *lines[12];

  for(int ln=0;ln<12;ln++)
  {
    lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
  }

  TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
  TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
  TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
  TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
  //__________________

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NSTPC_laser_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R1_LASER");
    tpcmon_NSTPC_laser_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R2_LASER");
    tpcmon_NSTPC_laser_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R3_LASER");

    tpcmon_SSTPC_laser_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R1_LASER");
    tpcmon_SSTPC_laser_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R2_LASER");
    tpcmon_SSTPC_laser_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R3_LASER");
  }

  add_TH2_modules( tpcmon_NSTPC_laser_clusXY_u,  tpcmon_NSTPC_laser_clusXY);
  add_TH2_modules( tpcmon_SSTPC_laser_clusXY_u,  tpcmon_SSTPC_laser_clusXY);
  
  if (!gROOT->FindObject("TPCClusterXY_laser"))
  {
    MakeCanvas("TPCClusterXY_laser");
  }  

  TCanvas *MyTC = TC[19];
  TPad *TransparentTPad = transparent[19];

  MyTC->SetEditable(true); //after Chris' change need to skip 22
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_LASER_ADC-Pedestal>(5sigma||20ADC) WEIGHTED, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his1_laser_XY->Draw("colzsame");

  float NS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_NSTPC_laser_clusXY[i][j] )
      {
        MyTC->cd(1);
        tpcmon_NSTPC_laser_clusXY[i][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_NSTPC_laser_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_laser_clusXY[i][j]->GetMaximumBin()) > NS_max)
        {
          NS_max = tpcmon_NSTPC_laser_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_laser_clusXY[i][j]->GetMaximumBin());
          dummy_his1_laser_XY->SetMaximum( NS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }

    }
  }
  MyTC->cd(1);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }
  MyTC->Update();

  MyTC->cd(2);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  gPad->SetLogz(kTRUE);
  dummy_his2_laser_XY->Draw("colzsame");

  float SS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_SSTPC_laser_clusXY[i+12][j] )
      {
        //std::cout<<"South Side Custer XY i: "<< i+12 <<", j: "<<j<<std::endl;
        MyTC->cd(2);
        tpcmon_SSTPC_laser_clusXY[i+12][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_SSTPC_laser_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_laser_clusXY[i+12][j]->GetMaximumBin()) > SS_max)
        {
          SS_max = tpcmon_SSTPC_laser_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_laser_clusXY[i+12][j]->GetMaximumBin());
          dummy_his2_laser_XY->SetMaximum( SS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }
    }

  }
  MyTC->cd(2);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCStuckChannels(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  TH1 *tpcmon_stuckchannels[24] = {nullptr};
  TH1 *tpcmon_stuckchannels_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_stuckchannels_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Stuck_Channels");
    //if( !tpcmon_stuckchannels[i] ){std::cout<<"Not able to get stuck channel histo # "<<i<<std::endl;}
  }

  add_TH1(tpcmon_stuckchannels_u, tpcmon_stuckchannels);
  
  if (!gROOT->FindObject("TPCStuckChannels"))
  {
    MakeCanvas("TPCStuckChannels");
  }

  TCanvas *MyTC = TC[21];
  TPad *TransparentTPad = transparent[21];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  MyTC->cd(1);

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t11 = new TLine(); t11->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_stuckchannels[i] )
    {
      MyTC->cd(i+5);

      tpcmon_stuckchannels[i]->GetYaxis()->SetRangeUser(0.01,10000);
      tpcmon_stuckchannels[i]->DrawCopy("HIST");

      gPad->SetLogy(kTRUE);

      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+0.5),0.01,(j+0.5),1500);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k,400,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(3.1,2000,"R1");
      tt1->DrawText(9.6,2000,"R2");
      tt1->DrawText(20.4,2000,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(5.5,0.01,5.5,5000);
      t1->DrawLine(13.5,0.01,13.5,5000);

      t11->DrawLine(-0.5,256,25.5,256);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_StuckChannel Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  //MyTC->SetLogy();
  MyTC->Show();
  MyTC->SetEditable(false);


  return 0;
}
int TpcMonDraw::DrawTPCXYclusters5event(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_NSTPC_5e_clusXY[24][3] = {nullptr};
  TH2 *tpcmon_SSTPC_5e_clusXY[24][3] = {nullptr};

  TH2 *tpcmon_NSTPC_5e_clusXY_u[48][3] = {nullptr};
  TH2 *tpcmon_SSTPC_5e_clusXY_u[48][3] = {nullptr}; 
  
  dummy_his1_u5_XY = new TH2F("dummy_his1_u5_XY", "(ADC-Pedestal) > (5#sigma||20ADC) North Side, <= 5 E , UNWEIGHTED", 400, -800, 800, 400, -800, 800); //dummy histos for titles
  dummy_his2_u5_XY = new TH2F("dummy_his2_u5_XY", "(ADC-Pedestal) > (5#sigma||20ADC) South Side, <= 5 E , UNWEIGHTED", 400, -800, 800, 400, -800, 800);

  dummy_his1_u5_XY->SetXTitle("X [mm]");
  dummy_his1_u5_XY->SetYTitle("Y [mm]");
  dummy_his1_u5_XY->GetYaxis()->SetTitleSize(0.02);

  dummy_his2_u5_XY->SetXTitle("-X [mm]"); //SS x is flipped from global coordinates
  dummy_his2_u5_XY->SetYTitle("Y [mm]");
  dummy_his2_u5_XY->GetYaxis()->SetTitleSize(0.02);

  //the lines are for the sector boundaries
  Double_t sec_gap_inner = (2*M_PI - 0.5024*12.0)/12.0;

  Double_t sec_gap_outer = (2*M_PI - 0.5097*12.0)/12.0;

  Double_t sec_gap = (sec_gap_inner + sec_gap_outer)/2.0;

  Double_t sec_phi = (0.5024 + 0.5097)/2.0;

  TLine *lines[12];

  for(int ln=0;ln<12;ln++)
  {
    lines[ln] = new TLine(311.05*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),311.05*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*cos((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)),759.11*sin((sec_phi+sec_gap)/2.0+ln*(sec_phi+sec_gap)));
  }

  TEllipse *e1 = new TEllipse(0.0,0.0,311.05,311.05);
  TEllipse *e2 = new TEllipse(0.0,0.0,(402.49+411.53)/2.0,(402.49+411.53)/2.0);
  TEllipse *e3 = new TEllipse(0.0,0.0,(583.67+574.75)/2.0,(583.67+574.75)/2.0);
  TEllipse *e4 = new TEllipse(0.0,0.0,759.11,759.11);
  //__________________

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);                             
    tpcmon_NSTPC_5e_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R1_u5");
    tpcmon_NSTPC_5e_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R2_u5");
    tpcmon_NSTPC_5e_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"NorthSideADC_clusterXY_R3_u5");

    tpcmon_SSTPC_5e_clusXY_u[i][0] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R1_u5");
    tpcmon_SSTPC_5e_clusXY_u[i][1] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R2_u5");
    tpcmon_SSTPC_5e_clusXY_u[i][2] = (TH2*) cl->getHisto(TPCMON_STR,"SouthSideADC_clusterXY_R3_u5");
  }

  add_TH2_modules( tpcmon_NSTPC_5e_clusXY_u,  tpcmon_NSTPC_5e_clusXY);  
  add_TH2_modules( tpcmon_SSTPC_5e_clusXY_u,  tpcmon_SSTPC_5e_clusXY);
  
  if (!gROOT->FindObject("TPCClusterXY_u5"))
  {
    MakeCanvas("TPCClusterXY_u5");
  }  

  TCanvas *MyTC = TC[22];
  TPad *TransparentTPad = transparent[22];

  MyTC->SetEditable(true); 
  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ADC-Pedestal>(5sigma||20ADC) UNWEIGHTED, <= 5E, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->cd(1);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  //gPad->SetLogz(kTRUE);
  dummy_his1_u5_XY->Draw("colzsame");

  float NS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_NSTPC_5e_clusXY[i][j] )
      {
        MyTC->cd(1);
        tpcmon_NSTPC_5e_clusXY[i][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_NSTPC_5e_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_5e_clusXY[i][j]->GetMaximumBin()) > NS_max)
        {
          NS_max = tpcmon_NSTPC_5e_clusXY[i][j]->GetBinContent(tpcmon_NSTPC_5e_clusXY[i][j]->GetMaximumBin());
          dummy_his1_u5_XY->SetMaximum( NS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }

    }
  }
  MyTC->cd(1);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }
  MyTC->Update();

  MyTC->cd(2);
  gStyle->SetOptStat(kFALSE);
  gPad->SetTopMargin(0.15);
  //gPad->SetLogz(kTRUE);
  dummy_his2_u5_XY->Draw("colzsame");

  float SS_max = 0;
  for( int i=0; i<12; i++ )
  {
    for( int j=0; j<3; j++ )
    {
      if( tpcmon_SSTPC_5e_clusXY[i+12][j] )
      {
        //std::cout<<"South Side Custer XY i: "<< i+12 <<", j: "<<j<<std::endl;
        MyTC->cd(2);
        tpcmon_SSTPC_5e_clusXY[i+12][j] -> Draw("colzsame");
        //gStyle->SetLogz(kTRUE);
        if ( tpcmon_SSTPC_5e_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_5e_clusXY[i+12][j]->GetMaximumBin()) > SS_max)
        {
          SS_max = tpcmon_SSTPC_5e_clusXY[i+12][j]->GetBinContent(tpcmon_SSTPC_5e_clusXY[i+12][j]->GetMaximumBin());
          dummy_his2_u5_XY->SetMaximum( SS_max );
        }
        gStyle->SetPalette(57); //kBird CVD friendly
      }
    }

  }
  MyTC->cd(2);
  e1->SetFillStyle(0);
  e2->SetFillStyle(0);
  e3->SetFillStyle(0);
  e4->SetFillStyle(0);

  e1->Draw("same");
  e2->Draw("same");
  e3->Draw("same");
  e4->Draw("same");
  for(int ln2=0;ln2<12;ln2++)
  {
    lines[ln2]->Draw("same"); 
  }

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCChansinPacketNS(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_chanpacketNS[24] = {nullptr};
  TH1 *tpcmon_chanpacketalwaysNS[24] = {nullptr};

  TH1 *tpcmon_chanpacketNS_u[48] = {nullptr};
  TH1 *tpcmon_chanpacketalwaysNS_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<12; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanpacketNS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    tpcmon_chanpacketalwaysNS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_Always");
  }

  for( int i=24; i<36; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanpacketNS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    tpcmon_chanpacketalwaysNS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_Always");
  }

  add_TH1(tpcmon_chanpacketNS_u, tpcmon_chanpacketNS);
  add_TH1(tpcmon_chanpacketalwaysNS_u, tpcmon_chanpacketalwaysNS);
  
  if (!gROOT->FindObject("TPCChan_in_Packets_NS"))
  {
    MakeCanvas("TPCChan_in_Packets_NS");
  }

  TCanvas *MyTC = TC[23];
  TPad *TransparentTPad = transparent[23];

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=0; i<12; i++ ) 
  {
    if( tpcmon_chanpacketNS[i] && tpcmon_chanpacketalwaysNS[i] )
    {
      MyTC->cd(i+3);
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);
      tpcmon_chanpacketNS[i]->Divide(tpcmon_chanpacketalwaysNS[i]);

      double Yrange_upper = 1.32*tpcmon_chanpacketNS[i]->GetMaximum();

      tpcmon_chanpacketNS[i]->GetYaxis()->SetRangeUser(0, Yrange_upper);

      tpcmon_chanpacketNS[i]->SetMarkerColor(4);
      tpcmon_chanpacketNS[i]->SetLineColor(4);
      tpcmon_chanpacketNS[i]->DrawCopy("HIST");
      
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*256,-0.01,(j+1)*256,Yrange_upper);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*256+128,0.84*Yrange_upper,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(800,0.92*Yrange_upper,"R1");
      tt1->DrawText(2450,0.92*Yrange_upper,"R2");
      tt1->DrawText(5200,0.92*Yrange_upper,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(1536,0,1536,Yrange_upper);
      t1->DrawLine(3584,0,3584,Yrange_upper);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_NS_Channels per Packet per RCDAQ Event Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCChansinPacketSS(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_chanpacketSS[24] = {nullptr};
  TH1 *tpcmon_chanpacketalwaysSS[24] = {nullptr};

  TH1 *tpcmon_chanpacketSS_u[48] = {nullptr};
  TH1 *tpcmon_chanpacketalwaysSS_u[48] = {nullptr};  
  
  char TPCMON_STR[100];
  for( int i=12; i<24; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanpacketSS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    tpcmon_chanpacketalwaysSS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_Always");
  }

  for( int i=36; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanpacketSS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    tpcmon_chanpacketalwaysSS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_Always");
  }

  add_TH1(tpcmon_chanpacketSS_u, tpcmon_chanpacketSS);
  add_TH1(tpcmon_chanpacketalwaysSS_u, tpcmon_chanpacketalwaysSS);

  if (!gROOT->FindObject("TPCChan_in_Packets_SS"))
  {
    MakeCanvas("TPCChan_in_Packets_SS");
  }

  TCanvas *MyTC = TC[24];
  TPad *TransparentTPad = transparent[24];

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=12; i<24; i++ ) 
  {
    if( tpcmon_chanpacketSS[i] && tpcmon_chanpacketalwaysSS[i] )
    {
      MyTC->cd(i+3);
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);
      tpcmon_chanpacketSS[i]->Divide(tpcmon_chanpacketalwaysSS[i]);
      double Yrange_upper = 1.32*tpcmon_chanpacketSS[i]->GetMaximum();
      tpcmon_chanpacketSS[i]->GetYaxis()->SetRangeUser(0, Yrange_upper);

      tpcmon_chanpacketSS[i]->SetMarkerColor(4);
      tpcmon_chanpacketSS[i]->SetLineColor(4);
      tpcmon_chanpacketSS[i]->DrawCopy("HIST");
      
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*256,-0.01,(j+1)*256,Yrange_upper);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*256+128,0.84*Yrange_upper,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(800,0.92*Yrange_upper,"R1");
      tt1->DrawText(2450,0.92*Yrange_upper,"R2");
      tt1->DrawText(5200,0.92*Yrange_upper,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(1536,0,1536,Yrange_upper);
      t1->DrawLine(3584,0,3584,Yrange_upper);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_SS_Channels per Packet per RCDAQ Event Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCNonZSChannels(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_nonZSchannels_u[48] = {nullptr};
  TH2 *tpcmon_nonZSchannels[24] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_nonZSchannels_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"Num_non_ZS_channels_vs_SAMPA");
  }

  add_TH2(tpcmon_nonZSchannels_u, tpcmon_nonZSchannels);

  if (!gROOT->FindObject("TPCNonZSChannels"))
  {
    MakeCanvas("TPCNonZSChannels");
  }
  TCanvas *MyTC = TC[25];
  TPad *TransparentTPad = transparent[25];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_nonZSchannels[i] )
    {
      MyTC->cd(i+5);
      tpcmon_nonZSchannels[i]->DrawCopy("colz");
      gPad->SetLogz(kTRUE);

      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*8,-300.01,(j+1)*8,1024);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*8+4,-100,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(25,-200,"R1");
      tt1->DrawText(77,-200,"R2");
      tt1->DrawText(163,-200,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(48,-300.01,48,1024);
      t1->DrawLine(112,-300.01,112,1024);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_Non-ZS Channels per SAMPA Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCZSTriggerADCSample(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_ZSTriggerADCSampledist[24] = {nullptr};
  TH2 *tpcmon_ZSTriggerADCSampledist_u[48] = {nullptr};

  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_ZSTriggerADCSampledist_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"ZS_Trigger_ADC_vs_Sample");
  }

  add_TH2( tpcmon_ZSTriggerADCSampledist_u,  tpcmon_ZSTriggerADCSampledist);

  if (!gROOT->FindObject("TPCNonZSTriggerADCvsSample"))
  {
    MakeCanvas("TPCNonZSTriggerADCvsSample");
  }

  TCanvas *MyTC = TC[26];
  TPad *TransparentTPad = transparent[26];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_ZSTriggerADCSampledist[i] )
    {
      MyTC->cd(i+5);
      tpcmon_ZSTriggerADCSampledist[i]->GetXaxis()->SetRangeUser(0, 20);
      tpcmon_ZSTriggerADCSampledist[i]->DrawCopy("colz");
      gPad->SetLogz(kTRUE);
      gPad->SetLogy(kTRUE);
      gPad->SetGridy(kTRUE);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_Trigger ADC vs Sample Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCFirstnonZSADCFirstnonZSSample(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *tpcmon_FirstNZSADCvsFirstNZSSample[24] = {nullptr};
  TH2 *tpcmon_FirstNZSADCvsFirstNZSSample_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_FirstNZSADCvsFirstNZSSample_u[i] = (TH2*) cl->getHisto(TPCMON_STR,"First_ADC_vs_First_Time_Bin");
  }

  add_TH2(tpcmon_FirstNZSADCvsFirstNZSSample_u, tpcmon_FirstNZSADCvsFirstNZSSample);
  
  if (!gROOT->FindObject("TPCFirstADCvsFirstSample"))
  {
    MakeCanvas("TPCFirstADCvsFirstSample");
  }

  TCanvas *MyTC = TC[27];
  TPad *TransparentTPad = transparent[27];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_FirstNZSADCvsFirstNZSSample[i] )
    {
      MyTC->cd(i+5);
      tpcmon_FirstNZSADCvsFirstNZSSample[i]->RebinX(5);
      tpcmon_FirstNZSADCvsFirstNZSSample[i]->DrawCopy("colz");
      gPad->SetLogz(kTRUE);
      gPad->SetLogy(kTRUE);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1st non-ZS ADC vs 1st non-ZS Sample Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCDriftWindow(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_DriftWindow[24][3] = {nullptr};
  TH1 *tpcmon_DriftWindow_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    for( int j=0; j<3; j++ )
    {
      //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
      sprintf(TPCMON_STR,"TPCMON_%i",i);
      tpcmon_DriftWindow_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R1");//tpcmon_DriftWindow[i][0]->GetXaxis()->SetRangeUser(0,500);
      tpcmon_DriftWindow_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R2");//tpcmon_DriftWindow[i][1]->GetXaxis()->SetRangeUser(0,500);
      tpcmon_DriftWindow_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R3");//tpcmon_DriftWindow[i][2]->GetXaxis()->SetRangeUser(0,500);
    }
  }

  add_TH1_modules( tpcmon_DriftWindow_u, tpcmon_DriftWindow);

  if (!gROOT->FindObject("TPCDriftWindow"))
  {
    MakeCanvas("TPCDriftWindow");
  }

  TCanvas *MyTC = TC[28];
  TPad *TransparentTPad = transparent[28];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  auto legend = new TLegend(0.7,0.65,0.98,0.95);
  bool draw_leg = 0;

  for( int i=0; i<24; i++ )
  {
    MyTC->cd(i+5);

    int min = std::numeric_limits<int>::max(); // start wih the largest possible value
    int max = std::numeric_limits<int>::min(); // start with smalles possible value
    int no_laser_max =  std::numeric_limits<int>::min(); // start with smalles possible value

    bool no_laser_window = 0;
    
    for( int j = 2; j>-1; j-- )
    {
      if( tpcmon_DriftWindow[i][j] )
      {
        for( int k = 1; k < tpcmon_DriftWindow[i][j]->GetNbinsX(); k++ ) // old laser peak 416, new laser peak 403, new laser peak w/ 57.14 ns clock 368
	{
          if( (k>=0 && k<=350) || (k>380) ){ no_laser_window = 1;}
          if( k>350 && k<=380){ no_laser_window = 0;}
          if( (tpcmon_DriftWindow[i][j]->GetBinContent(k) > no_laser_max && tpcmon_DriftWindow[i][j]->GetBinContent(k) > 0) && (no_laser_window==1) ){no_laser_max = tpcmon_DriftWindow[i][j]->GetBinContent(k);}
          if( tpcmon_DriftWindow[i][j]->GetBinContent(k) > max && tpcmon_DriftWindow[i][j]->GetBinContent(k) > 0 ){max = tpcmon_DriftWindow[i][j]->GetBinContent(k);}
          if( tpcmon_DriftWindow[i][j]->GetBinContent(k) < min && tpcmon_DriftWindow[i][j]->GetBinContent(k) > 0 ){min = tpcmon_DriftWindow[i][j]->GetBinContent(k);}
	}
      }
    }


    for( int l = 2; l>-1; l-- )
    {
      if( tpcmon_DriftWindow[i][l] )
      {
        tpcmon_DriftWindow[i][l]->GetXaxis()->SetRangeUser(0,500);
        if(l == 2){tpcmon_DriftWindow[i][l]->GetYaxis()->SetRangeUser(0.9*min,1.1*max);tpcmon_DriftWindow[i][l] -> DrawCopy("HIST");} //used to be 
        else      {tpcmon_DriftWindow[i][l]->GetYaxis()->SetRangeUser(0.9*min,1.1*max);tpcmon_DriftWindow[i][l] -> DrawCopy("HISTsame");} //assumes that R3 will always exist and is most entries
      }
    }
    
    gPad->Update();  

    if(draw_leg == 0 && tpcmon_DriftWindow[i][0] && tpcmon_DriftWindow[i][1] && tpcmon_DriftWindow[i][2]) //if you have not drawn the legend yet, draw it BUT ONLY ONCE
    {
      legend->AddEntry(tpcmon_DriftWindow[i][0], "R1");
      legend->AddEntry(tpcmon_DriftWindow[i][1], "R2");
      legend->AddEntry(tpcmon_DriftWindow[i][2], "R3");
      MyTC->cd(i+5);
      legend->Draw();
      draw_leg = 1; //don't draw it again
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_Drift Window, ADC-Pedestal>(5sigma||20ADC) Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}


int TpcMonDraw::DrawTPCNStreaksvsEventNo(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_NStreak_vsEventNo[24] = {nullptr};
  TH1 *tpcmoneventsebdc[24] = {nullptr};

  TH1 *tpcmon_NStreak_vsEventNo_u[48] = {nullptr};
  TH1 *tpcmoneventsebdc_u[48] = {nullptr};
  
  char TPCMON_STR[100];

  for( int i=0; i<48; i++) //get the streakers vs event nos from each event histo 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_NStreak_vsEventNo_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"NStreaks_vs_EventNo");
  }

  for( int i=0; i<48; i++) //get the num of events from each event histo 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmoneventsebdc_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"NEvents_vs_EBDC");
  }

  add_TH1( tpcmon_NStreak_vsEventNo_u,  tpcmon_NStreak_vsEventNo);
  add_TH1( tpcmoneventsebdc_u,  tpcmoneventsebdc  );
  
  if (!gROOT->FindObject("TPCNStreakersvsEventNo"))
  {
    MakeCanvas("TPCNStreakersvsEventNo");
  }    

  int event_max = 0;
  int horiz_max = 0; 

  for ( int i=0; i< 24; i++ )
  {
    if( tpcmoneventsebdc[i] )
    {
      if(tpcmoneventsebdc[i]->GetBinContent(i+1) > event_max){event_max = tpcmoneventsebdc[i]->GetBinContent(i+1);}
      if(tpcmoneventsebdc[i]->GetBinContent(tpcmoneventsebdc[i]->GetMaximumBin()) > horiz_max){horiz_max = tpcmoneventsebdc[i]->GetBinContent(tpcmoneventsebdc[i]->GetMaximumBin());}
    }
  }


  int line_colors[24] = { 3, 8, 2, 6, 46, 14, 1, 39, 38, 4, 7, 30, 3, 8, 6, 2, 46, 4, 1, 39, 38, 4, 7, 30 }; // assumed filling down and across
  //int line_colors_leg[24] = {2, 3, 38, 14, 6, 8, 4, 1, 46, 30, 7, 39, 6, 3, 38, 4, 2, 8, 4, 1, 46, 30, 7, 39 }; // assumed filling across and down

  TCanvas *MyTC = TC[29];
  TPad *TransparentTPad = transparent[29];

  auto leg1 = new TLegend(0.6,0.65,0.98,0.95);
  leg1->SetNColumns(4);

  leg1->AddEntry((TObject*)0,"North Top","");
  leg1->AddEntry((TObject*)0,"North West","");
  leg1->AddEntry((TObject*)0,"North Bottom","");
  leg1->AddEntry((TObject*)0,"North East","");

  auto leg2 = new TLegend(0.6,0.65,0.98,0.95);
  leg2->SetNColumns(4);
  leg2->AddEntry((TObject*)0,"South Top","");
  leg2->AddEntry((TObject*)0,"South West","");
  leg2->AddEntry((TObject*)0,"South Bottom","");
  leg2->AddEntry((TObject*)0,"South East","");

  // int order[24] = {2, 3, 4, 0, 1, 11, 8, 9, 10, 5, 6, 7, 14, 15, 16, 12, 13, 23, 20, 21, 22, 17, 18, 19}; // assumed filling down and across
  int order_leg[24] = {2, 0, 8, 5, 3, 1, 9, 6, 4, 11, 10, 7, 14, 12, 20, 17, 15, 13, 21, 18, 16, 23, 22, 19}; // assumed filling across and down

  char legend_str[100];

  for( int i=0; i<24; i++) // legend loop
  {

    if( tpcmon_NStreak_vsEventNo[order_leg[i]] && i <=11 )
    {
      sprintf(legend_str,"Sector %i",order_leg[i]);
      leg1->AddEntry(tpcmon_NStreak_vsEventNo[order_leg[i]],legend_str);
    }

    if( !tpcmon_NStreak_vsEventNo[order_leg[i]] && i <=11 )
    {
      leg1->AddEntry((TObject*)0,"","");    
    }

    if( tpcmon_NStreak_vsEventNo[order_leg[i]] && i > 11 )
    {
      sprintf(legend_str,"Sector %i",order_leg[i]);
      leg2->AddEntry(tpcmon_NStreak_vsEventNo[order_leg[i]],legend_str);
    }

    if( !tpcmon_NStreak_vsEventNo[order_leg[i]] && i > 11 )
    {
      leg2->AddEntry((TObject*)0,"","");    
    }    

  }

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_NStreak_vsEventNo[i] )
    {

      if( i <= 11){MyTC->cd(1);}
      else { MyTC->cd(2);}
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);

      tpcmon_NStreak_vsEventNo[i]->GetXaxis()->SetRangeUser(0, event_max);
      tpcmon_NStreak_vsEventNo[i]->GetYaxis()->SetRangeUser(0.01,1.1*horiz_max);
      tpcmon_NStreak_vsEventNo[i]->SetStats(kFALSE);
      tpcmon_NStreak_vsEventNo[i]->SetTitle("");    

      tpcmon_NStreak_vsEventNo[i]->SetMarkerColor(line_colors[i]);
      tpcmon_NStreak_vsEventNo[i]->SetLineColor(line_colors[i]);
      tpcmon_NStreak_vsEventNo[i]->SetLineWidth(3);   
      tpcmon_NStreak_vsEventNo[i]->Draw("LF2 same"); 

      gPad->SetLogy(kTRUE);
       
      MyTC->Update();

    }
  }

  MyTC->cd(1);
  leg1->Draw("same");
  MyTC->cd(2);
  leg2->Draw("same");


  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_NStreakers_vs_Event # Packet per Sector" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCPacketTypes(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_PacketType[24][3] = {nullptr};
  TH1 *tpcmon_PacketType_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_PacketType_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"Packet_Type_Fraction_HB");
    tpcmon_PacketType_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"Packet_Type_Fraction_NORM");
    tpcmon_PacketType_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"Packet_Type_Fraction_ELSE");
  }

  add_TH1_modules(tpcmon_PacketType_u, tpcmon_PacketType);
  
  if (!gROOT->FindObject("TPCPacketType"))
  {
    MakeCanvas("TPCPacketType");
  }

  TCanvas *MyTC = TC[30];
  TPad *TransparentTPad = transparent[30];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  double normfactor = 0;

  int index_arr[3] = {1, 0, 2} ;

  double norm_highest = 0.;
  int index_highest = 1;

  for( int i=0; i<24; i++ ) 
  {

    for( int k=0; k<3; k++)
    { 
      if(tpcmon_PacketType[i][k])
      { 
        normfactor += (tpcmon_PacketType[i][k]->GetEntries()); 
        if(tpcmon_PacketType[i][k]->GetEntries()>norm_highest){ norm_highest = (tpcmon_PacketType[i][k]->GetEntries());index_highest=k; } 
      }  
    }

    if(index_highest == 0){ index_arr[0] = 0; index_arr[1] = 1; index_arr[2] = 2; }
    if(index_highest == 2){ index_arr[0] = 2; index_arr[1] = 1; index_arr[2] = 0; }

    for( int j=0; j< 3; j++)
    {
      if( tpcmon_PacketType[i][index_arr[j]] )
      {
        MyTC->cd(i+5);
        if(normfactor > 0){tpcmon_PacketType[i][index_arr[j]]->Scale(1/(normfactor),"width");}
        gPad->SetLogy(kTRUE);
        if(j == 0){ tpcmon_PacketType[i][index_arr[j]]->DrawCopy("HIST"); }
        else{  tpcmon_PacketType[i][index_arr[j]]->DrawCopy("HISTsame");}  
      } 
    }

    gPad->Update();
 
    normfactor = 0; //reset this after scaling each histogram at end of iteration of outer for loop (i loop)

  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_WF PACKET FRACTION Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}


int TpcMonDraw::DrawTPCChansperLVL1_NS(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_chanlvl1NS[24] = {nullptr};
  TH1 *lvl1_per_EBDC[24] = {nullptr};

  TH1 *tpcmon_chanlvl1NS_u[48] = {nullptr};
  TH1 *lvl1_per_EBDC_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<12; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanlvl1NS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    lvl1_per_EBDC_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"LVL_1_TAGGER_per_EBDC");
  }

  for( int i=24; i<36; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanlvl1NS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    lvl1_per_EBDC_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"LVL_1_TAGGER_per_EBDC");
  }  

  add_TH1(tpcmon_chanlvl1NS_u, tpcmon_chanlvl1NS);
  add_TH1(lvl1_per_EBDC_u, lvl1_per_EBDC);
  
  if (!gROOT->FindObject("TPCChan_per_LVL1_NS"))
  {
    MakeCanvas("TPCChan_per_LVL1_NS");
  }

  TCanvas *MyTC = TC[31];
  TPad *TransparentTPad = transparent[31];

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=0; i<12; i++ ) 
  {
    if( tpcmon_chanlvl1NS[i] && lvl1_per_EBDC[i] )
    {
      MyTC->cd(i+3);
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);
      tpcmon_chanlvl1NS[i]->Scale(1/(lvl1_per_EBDC[i]->GetBinContent(i+1)),"width");
      double Yrange_upper = 1.32*tpcmon_chanlvl1NS[i]->GetMaximum();
      tpcmon_chanlvl1NS[i]->GetYaxis()->SetRangeUser(0, Yrange_upper);

      tpcmon_chanlvl1NS[i]->SetMarkerColor(4);
      tpcmon_chanlvl1NS[i]->SetLineColor(4);
      tpcmon_chanlvl1NS[i]->SetFillColor(5);
      tpcmon_chanlvl1NS[i]->DrawCopy("HIST");
      
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*256,-0.01,(j+1)*256,Yrange_upper);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*256+128,0.84*Yrange_upper,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(800,0.92*Yrange_upper,"R1");
      tt1->DrawText(2450,0.92*Yrange_upper,"R2");
      tt1->DrawText(5200,0.92*Yrange_upper,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(1536,0,1536,Yrange_upper);
      t1->DrawLine(3584,0,3584,Yrange_upper);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_NS_Channels per LEVEL_1 TAGGER Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCChansperLVL1_SS(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_chanlvl1SS[24] = {nullptr};
  TH1 *lvl1_per_EBDC[24] = {nullptr};

  TH1 *tpcmon_chanlvl1SS_u[48] = {nullptr};
  TH1 *lvl1_per_EBDC_u[48] = {nullptr};

  char TPCMON_STR[100];
  for( int i=12; i<24; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanlvl1SS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    lvl1_per_EBDC_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"LVL_1_TAGGER_per_EBDC");
  }

  for( int i=36; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_chanlvl1SS_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Channels_in_Packet");
    lvl1_per_EBDC_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"LVL_1_TAGGER_per_EBDC");
  }  

  add_TH1(tpcmon_chanlvl1SS_u, tpcmon_chanlvl1SS);
  add_TH1(lvl1_per_EBDC_u, lvl1_per_EBDC);

  if (!gROOT->FindObject("TPCChan_per_LVL1_SS"))
  {
    MakeCanvas("TPCChan_per_LVL1_SS");
  }

  TCanvas *MyTC = TC[32];
  TPad *TransparentTPad = transparent[32];

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=12; i<24; i++ ) 
  {
    if( tpcmon_chanlvl1SS[i] && lvl1_per_EBDC[i] )
    {
      MyTC->cd(i+3);
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);
      tpcmon_chanlvl1SS[i]->Scale(1/(lvl1_per_EBDC[i]->GetBinContent(i+13)),"width");
      double Yrange_upper = 1.32*tpcmon_chanlvl1SS[i]->GetMaximum();
      tpcmon_chanlvl1SS[i]->GetYaxis()->SetRangeUser(0, Yrange_upper);

      tpcmon_chanlvl1SS[i]->SetMarkerColor(4);
      tpcmon_chanlvl1SS[i]->SetLineColor(4);
      tpcmon_chanlvl1SS[i]->SetFillColor(5);
      tpcmon_chanlvl1SS[i]->DrawCopy("HIST");
      
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*256,-0.01,(j+1)*256,Yrange_upper);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*256+128,0.84*Yrange_upper,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(800,0.92*Yrange_upper,"R1");
      tt1->DrawText(2450,0.92*Yrange_upper,"R2");
      tt1->DrawText(5200,0.92*Yrange_upper,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(1536,0,1536,Yrange_upper);
      t1->DrawLine(3584,0,3584,Yrange_upper);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_SS_Channels per LEVEL_1 TAGGER Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCParity(const std::string & /* what */)
{

  OnlMonClient *cl = OnlMonClient::instance();
  
  TH1 *tpcmon_parityerror[24] = {nullptr};
  TH1 *tpcmon_parities[24] = {nullptr};

  TH1 *tpcmon_parityerror_u[48] = {nullptr};
  TH1 *tpcmon_parities_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_parityerror_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Parity_Error");
    tpcmon_parities_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Check_Sums");
  }

  add_TH1(tpcmon_parityerror_u, tpcmon_parityerror);
  add_TH1(tpcmon_parities_u, tpcmon_parities);
  
  if (!gROOT->FindObject("TPCParityError"))
  {
    MakeCanvas("TPCParityError");
  }
  TCanvas *MyTC = TC[33];
  TPad *TransparentTPad = transparent[33];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  MyTC->cd(1);

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  for( int i=0; i<24; i++ )
  {
    if( tpcmon_parityerror[i] && tpcmon_parities[i] )
    {
      MyTC->cd(i+5);

      tpcmon_parityerror[i]->Divide(tpcmon_parities[i]);
      tpcmon_parityerror[i]->GetYaxis()->SetRangeUser(0.0001,1.5);
      tpcmon_parityerror[i]->DrawCopy("HIST");
    
      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*8,-0.01,(j+1)*8,1.5);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*8+4,1.2,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(25,1.4,"R1");
      tt1->DrawText(77,1.4,"R2");
      tt1->DrawText(163,1.4,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(48.5,-0.01,48.5,1.5);
      t1->DrawLine(112.5,-0.01,112.5,1.5);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_ParityError Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  //MyTC->SetLogy();
  MyTC->Show();
  MyTC->SetEditable(false);


  return 0;
}
                
int TpcMonDraw::DrawShifterTPCDriftWindow(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_DriftWindow_shifter[24][3] = {nullptr};
  TH1 *tpcmon_DriftWindow_shifter_u[48][3] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    for( int j=0; j<3; j++ )
    {
      //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
      sprintf(TPCMON_STR,"TPCMON_%i",i);
      tpcmon_DriftWindow_shifter_u[i][0] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R1");
      tpcmon_DriftWindow_shifter_u[i][1] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R2");
      tpcmon_DriftWindow_shifter_u[i][2] = (TH1*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R3");
    }
  }

  add_TH1_modules( tpcmon_DriftWindow_shifter_u, tpcmon_DriftWindow_shifter);

  // also get # of events that made it into each OnlMon Server
  TH1 *tpcmoneventsebdc[24] = {nullptr}; 

  char TPCMON_STR2[100];
  for( int i=0; i<24; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR2,"TPCMON_%i",i);
    tpcmoneventsebdc[i] = (TH1*) cl->getHisto(TPCMON_STR2,"NEvents_vs_EBDC");
  }
  
  if (!gROOT->FindObject("ShifterTPCDriftWindow"))
  {
    MakeCanvas("ShifterTPCDriftWindow");
  }

  TCanvas *MyTC = TC[34];
  TPad *TransparentTPad = transparent[34];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57); //kBird CVD friendly

  auto legend = new TLegend(0.7,0.65,0.98,0.95);
  bool draw_leg = 0;

  char bad_message[128];

  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TPaveText *messages[24];  

  for( int i=0; i<24; i++ )
  {
    MyTC->cd(i+5);

    int min = std::numeric_limits<int>::max(); // start wih the largest possible value
    int max = std::numeric_limits<int>::min(); // start with smalles possible value
    
    int R1_max = std::numeric_limits<int>::min(); // start with smalles possible value
    int R2_max = std::numeric_limits<int>::min(); // start with smalles possible value
    int R3_max = std::numeric_limits<int>::min(); // start with smalles possible value

    bool R1_bad = 0;
    bool R2_bad = 0;
    bool R3_bad = 0;

    for( int j = 2; j>-1; j-- )
    {
      if( tpcmon_DriftWindow_shifter[i][j] )
      {
        for( int k = 1; k < tpcmon_DriftWindow_shifter[i][j]->GetNbinsX(); k++ )
	{
          //if(j == 2 &&  (tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > R3_max && tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > 0) ){ R3_max = k; }
          //if(j == 1 &&  (tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > R2_max && tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > 0) ){ R2_max = k; }
          //if(j == 0 &&  (tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > R1_max && tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > 0) ){ R1_max = k; }
          //std::cout<<"In Progress: "<<"R1_max = "<<R1_max<<" R2_max = "<<R2_max<<" R3_max = "<<R3_max<<std::endl;
          if( tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > max && tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > 0 ){max = tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k);}
          if( tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) < min && tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k) > 0 ){min = tpcmon_DriftWindow_shifter[i][j]->GetBinContent(k);}
	}
      }
    }


    if( tpcmon_DriftWindow_shifter[i][0] ){  R1_max = tpcmon_DriftWindow_shifter[i][0]->GetBinCenter(tpcmon_DriftWindow_shifter[i][0]->GetMaximumBin());}
    if( tpcmon_DriftWindow_shifter[i][1] ){  R2_max = tpcmon_DriftWindow_shifter[i][1]->GetBinCenter(tpcmon_DriftWindow_shifter[i][1]->GetMaximumBin());}
    if( tpcmon_DriftWindow_shifter[i][2] ){  R3_max = tpcmon_DriftWindow_shifter[i][2]->GetBinCenter(tpcmon_DriftWindow_shifter[i][2]->GetMaximumBin());}

    if( (R1_max>std::numeric_limits<int>::min() && (R1_max < 365)) || R1_max > 374 ){ R1_bad = 1;} // old peak - 416, new peak 403 50 ns clock, new peak 368 57.14 ns clock 
    if( (R2_max>std::numeric_limits<int>::min() && (R2_max < 365)) || R2_max > 374 ){ R2_bad = 1;} 
    if( (R3_max>std::numeric_limits<int>::min() && (R3_max < 365)) || R3_max > 374 ){ R3_bad = 1;}

    for( int l = 2; l>-1; l-- )
    {
      if( tpcmon_DriftWindow_shifter[i][l] )
      {
        tpcmon_DriftWindow_shifter[i][l]->GetXaxis()->SetRangeUser(346,396); // old peak - 416, new peak 403, new peak 368 57.14 ns clock
        if(l == 2)
        { 
          tpcmon_DriftWindow_shifter[i][l]->GetYaxis()->SetRangeUser(0.9*min,1.3*max);tpcmon_DriftWindow_shifter[i][l] -> DrawCopy("HIST");
        } //used to be 
        else      
        { 
          tpcmon_DriftWindow_shifter[i][l]->GetYaxis()->SetRangeUser(0.9*min,1.3*max); tpcmon_DriftWindow_shifter[i][l] -> DrawCopy("HISTsame");
        } //assumes that R3 will always exist and is most entries
      }
    }

    t2->DrawLine(365,0.9*min,365,1.3*max);
    t2->DrawLine(375,0.9*min,375,1.3*max);
    
    gPad->Update();  

    if(draw_leg == 0 && tpcmon_DriftWindow_shifter[i][0] && tpcmon_DriftWindow_shifter[i][1] && tpcmon_DriftWindow_shifter[i][2]) //if you have not drawn the legend yet, draw it BUT ONLY ONCE
    {
      legend->AddEntry(tpcmon_DriftWindow_shifter[i][0], "R1");
      legend->AddEntry(tpcmon_DriftWindow_shifter[i][1], "R2");
      legend->AddEntry(tpcmon_DriftWindow_shifter[i][2], "R3");
      MyTC->cd(i+5);
      legend->Draw();
      draw_leg = 1; //draw legend only once
    }

    //std::cout<<"R1_max = "<<R1_max<<" R2_max = "<<R2_max<<" R3_max = "<<R3_max<<std::endl;
    //messages->Clear();
    messages[i] = new TPaveText(0.1,0.5,0.4,0.9,"brNDC");  
    
    /*
    if( (i == 21 || i == 22) || (i == 23) )
    {
      messages[i]->AddText("SECTOR NOT MONITORED"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlack);
      MyTC->cd(i+5);
      messages[i]->Draw("same");      
    }
    */
    if( tpcmoneventsebdc[i] )
    {
      if(tpcmoneventsebdc[i]->GetEntries() <  8000)
      {
        messages[i]->AddText("NOT ENOUGH STATS"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlue);
        messages[i]->AddText("CHECK AGAIN"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlue);
        MyTC->cd(i+5);
        messages[i]->Draw("same");
      }
    }
    else if(  (R1_bad==1 || R2_bad==1) || R3_bad==1 )
    {
      //std::cout<<"made it into the if statement for bad timing"<<std::endl;
      sprintf(bad_message,"Sector %i BAD",i);
      messages[i]->SetFillColor(kRed);
      messages[i]->AddText(bad_message); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlack);
      messages[i]->AddText("REFRESH. IF PERSISTS, CALL EXPERT"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlack);
      MyTC->cd(i+5);
      messages[i]->Draw("same");
    }
    else if( tpcmon_DriftWindow_shifter[i][0] && (tpcmon_DriftWindow_shifter[i][1] && tpcmon_DriftWindow_shifter[i][2]) )
    {
      messages[i]->SetFillColor(kGreen);
      messages[i]->AddText("ALL GOOD"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlack);
      MyTC->cd(i+5);
      messages[i]->Draw("same");
    }
    else
    {
      messages[i]->AddText("MISSING DATA"); ((TText*)messages[i]->GetListOfLines()->Last())->SetTextColor(kBlack);
      MyTC->cd(i+5);
      messages[i]->Draw("same");
    }
    

  }// finish looping over 24 sectors

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_Shifter_Drift Window, ADC-Pedestal>(5sigma||20ADC) Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawTPCNoiseChannelPlots(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *tpcmon_noise_channel_plots[24] = {nullptr};
  TH1 *tpcmon_lvl1_per_EBDC[24] = {nullptr};

  TH1 *tpcmon_noise_channel_plots_u[48] = {nullptr};
  TH1 *tpcmon_lvl1_per_EBDC_u[48] = {nullptr};
  
  char TPCMON_STR[100];
  for( int i=0; i<48; i++ ) 
  {
    //const TString TPCMON_STR( Form( "TPCMON_%i", i ) );
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    tpcmon_noise_channel_plots_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"Noise_Channel_Plots");
    tpcmon_lvl1_per_EBDC_u[i] = (TH1*) cl->getHisto(TPCMON_STR,"LVL_1_TAGGER_per_EBDC");
  }

  add_TH1(tpcmon_noise_channel_plots_u, tpcmon_noise_channel_plots);
  add_TH1(tpcmon_lvl1_per_EBDC_u, tpcmon_lvl1_per_EBDC);
  
  if (!gROOT->FindObject("TPCNoiseChannelsPlots"))
  {
    MakeCanvas("TPCNoiseChannelsPlots");
  }

  TCanvas *MyTC = TC[35];
  TPad *TransparentTPad = transparent[35];

  TLine *t1 = new TLine(); t1->SetLineWidth(2);
  TLine *t2 = new TLine(); t2->SetLineStyle(2);
  TText *tt1= new TText(); tt1->SetTextSize(0.05);

  int FEEid[26]={2,4,3,13,17,16, // R1
                 11,12,19,18,0,1,15,14, // R2
                 20,22,21,23,25,24,10,9,8,6,7,5 // R3
                };

  char title[50];

  MyTC->SetEditable(true);
  MyTC->Clear("D");
  for( int i=0; i<24; i++ ) 
  {
    if( tpcmon_noise_channel_plots[i] && tpcmon_lvl1_per_EBDC[i]  )
    {
      MyTC->cd(i+5);
      gStyle->SetPadLeftMargin(0.05);
      gStyle->SetPadRightMargin(0.02);
      tpcmon_noise_channel_plots[i]->Scale(1./tpcmon_lvl1_per_EBDC[i]->GetEntries());
      
      double Yrange_upper = 2.0*tpcmon_noise_channel_plots[i]->GetMaximum();
      tpcmon_noise_channel_plots[i]->GetYaxis()->SetRangeUser(0,Yrange_upper);
      tpcmon_noise_channel_plots[i]->DrawCopy("HIST");      

      MyTC->Update();

      for(int j=0;j<25;j++)
      {
        t2->DrawLine((j+1)*256,-0.01,(j+1)*256,Yrange_upper);
      }
      for(int k=0;k<26;k++)
      {
        sprintf(title,"%d",FEEid[k]);
        tt1->DrawText(k*256+128,0.84*Yrange_upper,title);
      }
      tt1->SetTextSize(0.06);
      tt1->DrawText(800,0.92*Yrange_upper,"R1");
      tt1->DrawText(2450,0.92*Yrange_upper,"R2");
      tt1->DrawText(5200,0.92*Yrange_upper,"R3");
      tt1->SetTextSize(0.05); 

      t1->DrawLine(1536,0,1536,Yrange_upper);
      t1->DrawLine(3584,0,3584,Yrange_upper);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_Counts of ADC-Ped. > 300, t < 360 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

int TpcMonDraw::DrawShifterTransmissionDist(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  //TH1F *tpcmon_back[24][3] = {nullptr};
  // TH1F *tpcmon_drift[24][3] = {nullptr};

  TH1 *tpcmon_Drift[24][3] = {nullptr};
  TH1 *tpcmon_Drift_u[48][3] = {nullptr};
  //TH1 *tpcmoneventsebdc[24] = {nullptr};

  TH1F *PERCENT = new TH1F("PERCENT","TPC PERCENT TRANSMISSION; % Transmission; Counts",75,-1.5,151.5);

  TF1 *exxy = new TF1("exxy","[0]*exp(x*[1])",0,500);

  char TPCMON_STR[100];
 
  for( int i=0; i<48; i++ ) 
  {
    sprintf(TPCMON_STR,"TPCMON_%i",i);
    //tpcmoneventsebdc[i] = (TH1*) cl->getHisto(TPCMON_STR,"NEvents_vs_EBDC");
    for( int j=0; j<3; j++ )
    {

      tpcmon_Drift_u[i][0] = (TH1F*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R1");
      tpcmon_Drift_u[i][1] = (TH1F*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R2");
      tpcmon_Drift_u[i][2] = (TH1F*) cl->getHisto(TPCMON_STR,"COUNTS_vs_SAMPLE_1D_R3");

      /*
      sprintf(DRIFT_CLONE_STR,"drift_sec_%i_R%i",i,j+1);

      tpcmon_DriftBackground[i][j] = (TH1F*)tpcmon_back[i][j]->Clone(BACK_CLONE_STR);
      tpcmon_DriftNorm[i][j] = (TH1F*)tpcmon_back[i][j]->Clone(DRIFT_CLONE_STR);
      */
    }
  }

  add_TH1_modules(tpcmon_Drift_u, tpcmon_Drift);

  if (!gROOT->FindObject("ShifterTPCTransmissionDist"))
  {
    MakeCanvas("ShifterTPCTransmissionDist");
  }

  TCanvas *MyTC = TC[36];
  TPad *TransparentTPad = transparent[36];
  MyTC->SetEditable(true);
  MyTC->Clear("D");

  //left = 34, right = 350 for new firmware with laser around 380 samples
  int left=35; int right=350;

  //gStyle->SetOptStat(0);
  //gStyle->SetPalette(57); //kBird CVD friendly

  char norm_str[100];
  char back_str[100];

  for(int sector=0; sector<24; sector++)
  {
    for(int rad=0; rad<3; rad++)
    {
      if( (!tpcmon_Drift[sector][rad] || tpcmon_Drift[sector][rad]->GetEntries()<20000) ){continue;} //if doesn't exist move on
      //first zero the drift signal out of the background spectra
      sprintf(norm_str,"norm_sec_%i_r%i",sector,rad+1);
      sprintf(back_str,"back_sec_%i_r%i",sector,rad+1);
      
      TH1F *current_norm = (TH1F*) tpcmon_Drift[sector][rad]->Clone(norm_str);
      current_norm->GetXaxis()->SetRangeUser(0,500);
      TH1F *back_norm = (TH1F*) tpcmon_Drift[sector][rad]->Clone(back_str);
      back_norm->GetXaxis()->SetRangeUser(0,500);

      for(int bin=left; bin<=right; bin++)
      {
        back_norm->SetBinContent(bin,0.0);
      }
      //  second zero the laser signal out of the background spectra.
      for (int bin=367; bin<=376; bin++)
      {
        back_norm->SetBinContent(bin,0.0);
      }

      //Now Fit a function to each background and divide...
      exxy->SetParameter(0,back_norm->GetBinContent(2));
      exxy->SetParameter(1,0);
      back_norm->Fit(exxy,"QW");
      current_norm->Divide( back_norm->GetFunction("exxy")  );
      //  Subtract the "1" offset and set the minimum to -0.2...
      for (int bin=1; bin<=current_norm->GetNbinsX(); bin++)
      {
	current_norm->SetBinContent(bin, std::max(-0.2,  current_norm->GetBinContent(bin)-1.0 ));
      }

      if ( current_norm->GetBinContent(75) > 0)
      {
	exxy->SetParameter(0,current_norm->GetBinContent(75));
	exxy->SetParameter(1,0);
	current_norm->Fit(exxy,"QW","",75,275);
	double one  =  current_norm->GetFunction("exxy")->GetParameter(1);
	double percent = 100.0*exp(one*(275-75));
	PERCENT->Fill(percent);
      }
      
    }
  } 

  MyTC->Clear("D");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << " Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TransparentTPad->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.91, runstring.c_str());

  TLine *t1 = new TLine(); t1->SetLineWidth(4); t1->SetLineColor(kBlack);
  TLine *t2 = new TLine(); t2->SetLineWidth(2); t2->SetLineStyle(10); t2->SetLineColor(kRed);

  MyTC->cd(1);
  gPad->SetTopMargin(0.15);
  PERCENT->DrawCopy("hist");
  if(PERCENT->GetEntries()>36){PERCENT->Fit("gaus");t2->DrawLine( PERCENT->GetFunction("gaus")->GetParameter(1),0, PERCENT->GetFunction("gaus")->GetParameter(1),1.05*PERCENT->GetMaximum());}
  t1->DrawLine(70,0,70,1.05*PERCENT->GetMaximum());

  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);
  
  return 0;
}

int TpcMonDraw::SavePlot(const std::string &what, const std::string &type)
{

  OnlMonClient *cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
      return iret;
  }
  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" +
      std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}

int TpcMonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient *cl = OnlMonClient::instance();

  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    // Register the canvas png file to the menu and produces the png file.
    std::string pngfile = cl->htmlRegisterPage(*this, canvas->GetTitle(), std::to_string(icnt), "png");
    cl->CanvasToPng(canvas, pngfile);
  }
  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  // std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
  // std::ofstream out(logfile.c_str());
  // out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
  //     << "</TITLE></HEAD>" << std::endl;
  // out << "<P>Some log file output would go here." << std::endl;
  // out.close();

  // std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
  // std::ofstream out2(status.c_str());
  // out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
  //      << "</TITLE></HEAD>" << std::endl;
  // out2 << "<P>Some status output would go here." << std::endl;
  // out2.close();
  // cl->SaveLogFile(*this);
  return 0;
}

time_t TpcMonDraw::getTime()
{
  OnlMonClient *cl = OnlMonClient::instance();
  time_t currtime = 0;
  int i = 0;
  while (currtime == 0 && i <= 23)
  {
    std::string servername = "TPCMON_" + std::to_string(i);
    currtime = cl->EventTime(servername,"CURRENT");
    i++;
  }
  return currtime;
}

int TpcMonDraw::DrawServerStats()
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("TpcMonServerStats"))
  {
    MakeCanvas("TpcMonServerStats");
  }
  TCanvas *MyTC = TC[20];
  TPad *TransparentTPad = transparent[20];

  MyTC->Clear("D");
  MyTC->SetEditable(true);
  TransparentTPad->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  PrintRun.SetTextSize(0.04);
  PrintRun.SetTextColor(1);
  PrintRun.DrawText(0.5, 0.99, "Server Statistics");

  PrintRun.SetTextAlign(13);  // center/top alignment
  PrintRun.SetTextSize(0.02);
  double vdist = 0.03;
  double vstart = 0.9;
  double vpos = vstart;
  double hpos = 0.01;
  int i = 0;
  int numservers = m_ServerSet.size();
  std::vector<std::string> ServerVector;
  for (const auto &server : m_ServerSet)
    {
      if (server.size() <= 8)
	{
	  ServerVector.push_back(server);
	}
    }
  for (const auto &server : m_ServerSet)
    {
      if (server.size() > 8)
	{
	  ServerVector.push_back(server);
	}
    }
  for (const auto &server : ServerVector)
  {
    std::ostringstream txt;
    auto servermapiter = cl->GetServerMap(server);
    if (servermapiter == cl->GetServerMapEnd())
    {
      txt << "Server " << server
          << " is dead ";
      PrintRun.SetTextColor(kRed);
    }
    else
    {
      txt << "Server " << server
          << ", run number " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second)
          << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
      if (std::get<0>(servermapiter->second))
      {
        PrintRun.SetTextColor(kGray+2);
      }
      else
      {
        PrintRun.SetTextColor(kRed);
      }
    }
    if (i >= numservers/2)
      {
	hpos = 0.5;
	vpos = vstart;
	i = 0;
      }
	PrintRun.DrawText(hpos, vpos, txt.str().c_str());
    vpos -= vdist;
    i++;
  }
  MyTC->Update();
  MyTC->Show();
  MyTC->SetEditable(false);

  return 0;
}

void TpcMonDraw::add_TH1(TH1* hist[48], TH1* histadd[24]) {
    for (int i = 0; i < 24; ++i) {
      // Clone hist[i] to preserve binning, axis labels, etc.
      //char name_str[256];
      //sprintf(name_str,"%s",hist[i]->GetName());
      if(!hist[i] && !hist[i+24]){continue;}
      if(hist[i] && !hist[i+24]){histadd[i] = (TH1*)hist[i]->Clone(Form("histadd_%d",i));}
      if(!hist[i] && hist[i+24]){histadd[i] = (TH1*)hist[i+24]->Clone(Form("histadd_%d",i));}
      if(hist[i] && hist[i+24]){
	histadd[i] = (TH1*)hist[i]->Clone(Form("histadd_%d",i));
        histadd[i]->Add(hist[i+24]);
      }
    }
}

void TpcMonDraw::add_TH2(TH2* hist[48], TH2* histadd[24]) {
    for (int i = 0; i < 24; ++i) {
      // Clone hist[i] to preserve binning, axis labels, etc.
      //char name_str[256];
      //sprintf(name_str,"%s",hist[i]->GetName());
      if(!hist[i] && !hist[i+24]){continue;}
      if(hist[i] && !hist[i+24]){histadd[i] = (TH2*)hist[i]->Clone(Form("histadd_%d",i));}
      if(!hist[i] && hist[i+24]){histadd[i] = (TH2*)hist[i+24]->Clone(Form("histadd_%d",i));}
      if(hist[i] && hist[i+24]){
	histadd[i] = (TH2*)hist[i]->Clone(Form("histadd_%d",i));
        histadd[i]->Add(hist[i+24]);
      }
    }
}

void TpcMonDraw::add_TH1_modules(TH1* hist[48][3], TH1* histadd[24][3]) {
    for (int i = 0; i < 24; ++i) {
      for (int j = 0; j < 3; ++j) {
        // Clone hist[i] to preserve binning, axis labels, etc.
        //char name_str[256];
        //sprintf(name_str,"%s",hist[i]->GetName());
        if(!hist[i][j] && !hist[i+24][j]){continue;}
        if(hist[i][j] && !hist[i+24][j]){histadd[i][j] = (TH1*)hist[i][j]->Clone(Form("histadd_%d_%d",i,j));}
        if(!hist[i][j] && hist[i+24][j]){histadd[i][j] = (TH1*)hist[i+24][j]->Clone(Form("histadd_%d_%d",i,j));}
        if(hist[i][j] && hist[i+24][j]){
   	  histadd[i][j] = (TH1*)hist[i][j]->Clone(Form("histadd_%d_%d",i,j));
          histadd[i][j]->Add(hist[i+24][j]);
        }
      }	
    }
}

void TpcMonDraw::add_TH2_modules(TH2* hist[48][3], TH2* histadd[24][3]) {
    for (int i = 0; i < 24; ++i) {
      for (int j = 0; j < 3; ++j) {
        // Clone hist[i] to preserve binning, axis labels, etc.
        //char name_str[256];
        //sprintf(name_str,"%s",hist[i]->GetName());
        if(!hist[i][j] && !hist[i+24][j]){continue;}
        if(hist[i][j] && !hist[i+24][j]){histadd[i][j] = (TH2*)hist[i][j]->Clone(Form("histadd_%d_%d",i,j));}
        if(!hist[i][j] && hist[i+24][j]){histadd[i][j] = (TH2*)hist[i+24][j]->Clone(Form("histadd_%d_%d",i,j));}
        if(hist[i][j] && hist[i+24][j]){
   	  histadd[i][j] = (TH2*)hist[i][j]->Clone(Form("histadd_%d_%d",i,j));
          histadd[i][j]->Add(hist[i+24][j]);
        }
      }	
    }
}

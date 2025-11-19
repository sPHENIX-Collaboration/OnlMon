#ifndef TPC_TPCMON_H
#define TPC_TPCMON_H

#include <onlmon/OnlMon.h>

#include <tpc/TpcMap.h>  //this needs to be pointed to coresoftware - not sure how to do that on EBDCXX...
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Event;
class TH1;
class TH2;
class TTree;

class TpcMon : public OnlMon
{
 public:
  TpcMon(const std::string &name);
  virtual ~TpcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int evtcnt = 0;
  int evtcnt5 = 0;
  int idummy = 0;
  int weird_counter = 0;

  static const int N_rBins = 4;                                             //(one inner bin not filled, 2nd bin is R1, 3rd bin is R2, 4th bin is R3)
  const int N_thBins = 12;                                                  //(12 theta bins of uniform angle (360/12 = 30 degrees = TMath::Pi()/6 ~= 0.523 rad)
  const double rBin_edges[N_rBins + 1] = {0.0, 0.256, 0.504, 0.752, 1.00};  // variable edges for radial dims

  const int FEE_transform[26] = {10, 11, 0, 2, 1, 25, 23, 24, 22, 21, 20, 6, 7, 3, 13, 12, 5, 4, 9, 8, 14, 16, 15, 17, 19, 18};

  // for X-Y channel plot
  static const int N_rBins_XY = 66;  // From Evgeny code - global to all modules
  const double r_bins[N_rBins_XY + 1] = {217.83,
                                         223.83, 229.83, 235.83, 241.83, 247.83, 253.83, 259.83, 265.83, 271.83, 277.83, 283.83, 289.83, 295.83, 301.83, 306.83,
                                         311.05, 317.92, 323.31, 329.27, 334.63, 340.59, 345.95, 351.91, 357.27, 363.23, 368.59, 374.55, 379.91, 385.87, 391.23, 397.19, 402.49,
                                         411.53, 421.70, 431.90, 442.11, 452.32, 462.52, 472.73, 482.94, 493.14, 503.35, 513.56, 523.76, 533.97, 544.18, 554.39, 564.59, 574.76,
                                         583.67, 594.59, 605.57, 616.54, 627.51, 638.48, 649.45, 660.42, 671.39, 682.36, 693.33, 704.30, 715.27, 726.24, 737.21, 748.18, 759.11};  // From Evgeny code - global to all modules

  static const int N_phi_binx_XY_R1 = 1152;  // from Evgeny code (0 to 2pi)
  static const int N_phi_binx_XY_R2 = 1536;  // from Evgeny code (0 to 2pi)
  static const int N_phi_binx_XY_R3 = 2304;  // from Evgeny code (0 to 2pi)

  static const int N_z_bins = 3350;  // (50 ns time bins * ~12.3 micron/ns drift time = 615 microns). Drift Length = 1.03 * 10^6 microns. 2 *1.03 *10^6 microns / 615 microns = 3350 bins

  TH2 *NorthSideADC_clusterXY_R1 = nullptr;
  TH2 *NorthSideADC_clusterXY_R2 = nullptr;
  TH2 *NorthSideADC_clusterXY_R3 = nullptr;

  TH2 *SouthSideADC_clusterXY_R1 = nullptr;
  TH2 *SouthSideADC_clusterXY_R2 = nullptr;
  TH2 *SouthSideADC_clusterXY_R3 = nullptr;

  TH2 *NorthSideADC_clusterXY_R1_LASER = nullptr;
  TH2 *NorthSideADC_clusterXY_R2_LASER = nullptr;
  TH2 *NorthSideADC_clusterXY_R3_LASER = nullptr;

  TH2 *SouthSideADC_clusterXY_R1_LASER = nullptr;
  TH2 *SouthSideADC_clusterXY_R2_LASER = nullptr;
  TH2 *SouthSideADC_clusterXY_R3_LASER = nullptr;

  TH2 *NorthSideADC_clusterXY_R1_u5 = nullptr;
  TH2 *NorthSideADC_clusterXY_R2_u5 = nullptr;
  TH2 *NorthSideADC_clusterXY_R3_u5 = nullptr;

  TH2 *SouthSideADC_clusterXY_R1_u5 = nullptr;
  TH2 *SouthSideADC_clusterXY_R2_u5 = nullptr;
  TH2 *SouthSideADC_clusterXY_R3_u5 = nullptr;

  TH2 *NorthSideADC_clusterXY_R1_unw = nullptr;
  TH2 *NorthSideADC_clusterXY_R2_unw = nullptr;
  TH2 *NorthSideADC_clusterXY_R3_unw = nullptr;

  TH2 *SouthSideADC_clusterXY_R1_unw = nullptr;
  TH2 *SouthSideADC_clusterXY_R2_unw = nullptr;
  TH2 *SouthSideADC_clusterXY_R3_unw = nullptr;

  TH2 *NorthSideADC_clusterZY = nullptr;
  TH2 *SouthSideADC_clusterZY = nullptr;

  TH2 *NorthSideADC_clusterZY_unw = nullptr;
  TH2 *SouthSideADC_clusterZY_unw = nullptr;

  TH2 *NorthSideADC = nullptr;
  TH2 *SouthSideADC = nullptr;

  TH2 *ADC_vs_SAMPLE = nullptr;
  TH2 *PEDEST_SUB_ADC_vs_SAMPLE = nullptr;
  TH2 *ADC_vs_SAMPLE_large = nullptr;
  TH2 *PEDEST_SUB_ADC_vs_SAMPLE_R1 = nullptr;
  TH2 *PEDEST_SUB_ADC_vs_SAMPLE_R2 = nullptr;
  TH2 *PEDEST_SUB_ADC_vs_SAMPLE_R3 = nullptr;

  TH1 *sample_size_hist = nullptr;
  TH1 *Check_Sum_Error = nullptr;
  TH1 *Parity_Error = nullptr;
  TH1 *Check_Sums = nullptr;
  TH1 *Stuck_Channels = nullptr;
  TH1 *Channels_in_Packet = nullptr;
  TH1 *Channels_Always = nullptr;
  TH1 *LVL_1_TAGGER_per_EBDC = nullptr;

  TH2 *Num_non_ZS_channels_vs_SAMPA = nullptr;
  TH2 *ZS_Trigger_ADC_vs_Sample = nullptr;
  TH2 *First_ADC_vs_First_Time_Bin = nullptr;

  TH2 *MAXADC = nullptr;

  TH1 *RAWADC_1D_R1 = nullptr;
  TH1 *MAXADC_1D_R1 = nullptr;
  TH1 *PEDEST_SUB_1D_R1 = nullptr;
  TH1 *COUNTS_vs_SAMPLE_1D_R1 = nullptr;

  TH1 *RAWADC_1D_R2 = nullptr;
  TH1 *MAXADC_1D_R2 = nullptr;
  TH1 *PEDEST_SUB_1D_R2 = nullptr;
  TH1 *COUNTS_vs_SAMPLE_1D_R2 = nullptr;

  TH1 *RAWADC_1D_R3 = nullptr;
  TH1 *MAXADC_1D_R3 = nullptr;
  TH1 *PEDEST_SUB_1D_R3 = nullptr;
  TH1 *COUNTS_vs_SAMPLE_1D_R3 = nullptr;

  TH2 *Layer_ChannelPhi_ADC_weighted = nullptr;
  TH1 *NEvents_vs_EBDC = nullptr;

  TH1 *NStreaks_vs_EventNo = nullptr;

  TH1 *Packet_Type_Fraction_HB = nullptr;
  TH1 *Packet_Type_Fraction_NORM = nullptr;
  TH1 *Packet_Type_Fraction_ELSE = nullptr;
  TH2 *Packet_Type_vs_sample_ADC = nullptr;

  TH1 *Noise_Channel_Plots = nullptr;
  TH2 *DC_vs_SAMPA = nullptr;
  TH2 *DC_SAMPA_vs_TIME = nullptr;

  TpcMap M;  // declare Martin's map

  int starting_BCO;
  int starting_BCO_DC;
  int rollover_value;
  int rollover_value_DC;
  int current_BCOBIN;

  int serverid;

  int stuck_channel_count[256][26] = {0};  // array for counting # of times a unique channel get stuck

  void Locate(int id, float *rbin, float *thbin);
  int ebdc_from_serverid(int server_id);
  int Index_from_Module(int sec_id, int fee_id);
  int Module_ID(int fee_id);
  int Max_Nine(int one, int two, int three, int four, int five, int six, int seven, int eight, int nine);
  bool side(int server_id);
  std::pair<float, float> calculateMedianAndStdDev(const std::vector<int> &values);
  float calculateRawStdDev(const std::vector<int> &values);
};

#endif /* TPC_TPCMON_H */

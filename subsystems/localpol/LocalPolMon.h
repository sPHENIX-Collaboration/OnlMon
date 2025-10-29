#ifndef LOCALPOL_LOCALPOLMON_H
#define LOCALPOL_LOCALPOLMON_H

#include <onlmon/OnlMon.h>

#include <cmath>
#include <map>
#include <vector>

class CaloWaveformFitting;
class Event;
class TH1D;
class TH2I;
class TH2D;
class TProfile;
class Packet;
class TRandom;
class eventReceiverClient;

class LocalPolMon : public OnlMon
{
 public:
  explicit LocalPolMon(const std::string &name = "LOCALPOLMON");
  virtual ~LocalPolMon() = default;

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 private:
  double *ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U);
  float anaWaveformFast(Packet *p, const int channel, const int low, const int high, const int ihisto);
  CaloWaveformFitting *WaveformProcessingFast = nullptr;
  void RetrieveSpinPattern(int r);
  void RetrieveTriggerDistribution(Event* e);
  int RetrieveAbortGapData();
  int RetrieveBunchNumber(Event* e, long long int z);
  bool GoodSelection(int i);

  const int packetid_gl1 = 14001;  // could be ported to config
  const int packetid_smd = 12001;  // could be ported to config

  const int UP = 1;
  const int DN = -1;

  const int BLUE = 0;
  const int YELLOW = 1;

  const int ZDCN1 = 8;
  const int ZDCS1 = 0;
  const int ZDCN2 = 10;
  const int ZDCS2 = 2;


  const int ivetoNF=48;
  const int ivetoNB=49;
  const int ivetoSF=50;
  const int ivetoSB=51;




  const float nchannelsY = 8.0;
  const float nchannelsX = 7.0;
  const float pitchY = 2.0 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/ * sin(M_PI / 4) /*(correct for the tilt)*/;
  const float pitchX = 1.5 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/;

  bool goodtrigger[16] = {false};        // selection from config
  bool fake = false;                     // config
  bool verbosity = false;                // config
  bool Initfirstbunch=false;
  bool Integrated=false;
  int ExpectedsPhenixGapPosition = 111;  // from config
  int EventCountThresholdGap = 6000;     // from config
  int EventsAsymmetryNewPoint = 10000;   // from config
  int nEmptyAbort=0;
  int MultiLow=1;
  int MultiHigh=7;
  float ZDC1Cut=60.;
  float ZDC2Cut=10.;
  float VetoCut=0.;
  float SMDthr=5.;
  long long int Prevgl1_clock {0};
  long long int Prevzdc_clock {0};
  int lowSample[52]={0};
  int highSample[52]={0};
  int evtcnt = 0;
  int evtcntA=0;
  int EvtShift=0;
  int EvtShiftValid=0;
  int failuredepth=0;
  int StartAbortGapPattern = 111;
  int StartAbortGapData = 111;
  int CrossingShift = 0;
  int iPoint = 0;
  int ConversionSign[4] = {1, -1, 1, -1};  // NorthY (bottom to top), NorthX (left to righ from IR->ZDC, while x points outside the ring), SouthY (bottom to top), SouthX (left to right from IR->ZDC and x points outside ring but z is going from south to north, need extra minus one when considering yellow direction)
  float smd_north_relatgain[16]={0.};
  float smd_south_relatgain[16]={0.}; 
  float smd_adc[32]{};
  float signalZDCN1 = 0;
  float signalZDCS1 = 0;
  float signalZDCN2 = 0;
  float signalZDCS2 = 0;
  float vetoNF=0;
  float vetoNB=0;
  float vetoSF=0;
  float vetoSB=0;

  float ZeroPosition[4] = {0., 0., 0., 0.};  // from config

  std::map<int, int> SpinPatterns[2];
  std::map<int, long long> gl1_counter[16];
  std::map<int, int> Chmapping;

  TH1D *h_trigger[16]={nullptr};
  TH1D *h_events=nullptr;
  TH1D **h_Counts = nullptr;
  TH1D **h_CountsScramble = nullptr;

  TH1D *hmultiplicity[4]={nullptr};
  TH1D *hposition[8]={nullptr};
  TH1D *hadcsum[4]={nullptr};

  TProfile *h_time = nullptr;
  TH1D ****h_Asym = nullptr;
  TH1D ****h_AsymScramble = nullptr;

  TH2I* hspinpattern=nullptr;
  //TH2I* htimesync=nullptr;
  TH1D* hsyncfrac=nullptr;
  TH2D* Bluespace=nullptr;
  TH2D* Yellowspace=nullptr;
  TH2D* hwaveform[6]={nullptr};
  TH2D* hclocks=nullptr;
  TH2D* hevolsync=nullptr;
  TH2D* hshiftevol=nullptr;
  
  TRandom *myRandomBunch = nullptr;
  //std::map<int, int> stored_gl1p_files;
  eventReceiverClient *erc = nullptr;
};

#endif /* LOCALPOL_LOCALPOLMON_H */

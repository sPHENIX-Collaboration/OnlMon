#ifndef MVTX_MVTXMON_H
#define MVTX_MVTXMON_H

#include <onlmon/OnlMon.h>

#include <TString.h>
#include <TPaveText.h>

#include <map>
#include <cmath>

//#include "/sphenix/u/jkvapil/onlmon/src/subsystems/mvtx/PixelData.h"
//#include "PixelData.h"

class Event;
class TH1;
class TH2;
class TH1I;
class TH2I;
class TH3I;
class TH1D;
class TH2D;
class TH2Poly;
class map;
class pair;
//class ChipPixelData;



class MvtxMon : public OnlMon
{
 public:
  MvtxMon(const std::string &name);
  virtual ~MvtxMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();


 protected:
  int evtcnt = 0;
  int idummy = 0;

  const static int NSTAVE = 48;
  const static int NCHIP = 9;
  const static int NLAYERS = 3;
  const static int IDMVTXV1_MAXRUID = 48;
  const static int IDMVTXV1_MAXRUCHN = 28;
  static constexpr int NFees = 48 * 3;
  static constexpr int NTrigger = 13;
  static constexpr int NLanesMax = 9;
  const int NRowMax = 512;
  const int NColMax = 1024;
  int mHitPerChip[NLAYERS][NSTAVE][NCHIP] = {};
  int mHitPerChip_evt[NLAYERS][NSTAVE][NCHIP] = {};
  float OccPerChip[NSTAVE][NCHIP] = {};
  const int NBins = 30;
  static constexpr int NFlags = 3;

  int mMaxGeneralAxisRange = -3;  // the range of TH2Poly plots z axis range, pow(10, mMinGeneralAxisRange) ~ pow(10, mMaxGeneralAxisRange)
  int mMinGeneralAxisRange = -12;
  std::string mLaneStatusFlag[NFlags] = { "WARNING", "ERROR", "FAULT" };

  const int StaveBoundary[4] = { 0, 12, 28, 48};
  const int LayerBoundaryFEE[NLAYERS - 1] = { 35, 83};
  const int chipmapoffset[3] = { 0, 12, 28};
  //double** mOccupancyLane;

  TH2	*mvtxmon_ChipStaveOcc= nullptr;
  TH1	*mvtxmon_ChipStave1D= nullptr;
  TH1	*mvtxmon_ChipFiredHis= nullptr;
  TH1	*mvtxmon_EvtHitChip= nullptr;
  TH1 *mvtxmon_EvtHitDis= nullptr;
  TH2 *mvtxmon_HitMap[NSTAVE][NCHIP] = {nullptr}; 
  TH2Poly* mvtxmon_GeneralOccupancy = nullptr;
  TH2Poly* mvtxmon_LaneStatusOverview[NFlags] = {nullptr}; 
 //fee
  TH2I* mTriggerVsFeeId= nullptr;
  TH1I* mTrigger= nullptr;
  TH2I* mLaneInfo= nullptr;
  TH2I* mLaneStatus[NFlags] = {nullptr};
  TH2I* mLaneStatusCumulative[NFlags] = {nullptr};
  TH1I* mLaneStatusSummary[NLAYERS] = {nullptr};
  TH1I* mLaneStatusSummaryIB= nullptr;
  TString mTriggerType[NTrigger] = { "ORBIT", "HB", "HBr", "HC", "PHYSICS", "PP", "CAL", "SOT", "EOT", "SOC", "EOC", "TF", "INT" };

  //raw task
  TH1D* hErrorPlots= nullptr;
  TH2D* hErrorFile= nullptr;
  TH1D* hOccupancyPlot[NLAYERS] = {nullptr};
  TH2I* hEtaPhiHitmap[NLAYERS] = {nullptr};
  TH2D* hChipStaveOccupancy[NLAYERS] = {nullptr};
  TH3I* hChipHitmap = nullptr;
  TH3I* hChipHitmap_evt = nullptr;

  //fhr
  TH2I* mErrorVsFeeid= nullptr;
  TH2Poly* mGeneralOccupancy= nullptr;
  TH2Poly* mGeneralNoisyPixel= nullptr;
  TH2D* mDeadChipPos[NLAYERS] = {nullptr};
  TH2D* mAliveChipPos[NLAYERS] = {nullptr};
  TH2D* mTotalDeadChipPos= nullptr;
  TH2D* mTotalAliveChipPos= nullptr;
  TH2D* mChipStaveOccupancy[NLAYERS] = {nullptr};
  TH1D* mOccupancyPlot[NLAYERS] = {nullptr};

  static constexpr int NCols = 1024;
  static constexpr int NRows = 512;
  static constexpr int NPixels = NRows * NCols;
  float mOccupancyCutForNoisyPixel = 0.1;
  int mNoisyPixelNumber[3][20][9] = { { 0 } };
  static constexpr int nbinsetaIB = 9;
  static constexpr int nbinsphiIB = 16;
  int mEtabins = 130;
  int mPhibins = 240;
  float etabinsIB[3][nbinsetaIB + 1] = { { -2.34442, -2.06375, -1.67756, -1.0732, 0., 1.0732, 1.67756, 2.06375, 2.34442, 2.6 }, { -2.05399, -1.77866, -1.40621, -0.851972, 0., 0.851972, 1.40621, 1.77866, 2.05399, 2.4 }, { -1.8412, -1.57209, -1.21477, -0.707629, 0., 0.707629, 1.21477, 1.57209, 1.8412, 2.1 } };
  float phibinsIB[3][nbinsphiIB + 1] = { { -2.86662, -2.34302, -1.81942, -1.29582, -0.772222, -0.248623, 0.203977, 0.274976, 0.596676, 0.798574, 0.989375, 1.32217, 1.38207, 1.84577, 2.36937, 2.89297, 3.2 }, { -2.93762, -2.54492, -2.15222, -1.75952, -1.36682, -0.974121, -0.581422, -0.188722, 0.159761, 0.47392, 0.78808, 1.10224, 1.77477, 2.16747, 2.56017, 2.95287, 3.2 }, { -2.98183, -2.66767, -2.35351, -2.03935, -1.72519, -1.41104, -1.09688, -0.782717, -0.468558, -0.154398, 1.4164, 1.73056, 2.04472, 2.35888, 2.67304, 2.98719, 3.2 } };


  static constexpr int NError = 11;
  static constexpr int NErrorExtended = 19;
 
 /* TString ErrorType[NError] = { "Error ID 1: ErrPageCounterDiscontinuity", "Error ID 2: ErrRDHvsGBTHPageCnt",
                                "Error ID 3: ErrMissingGBTHeader", "Error ID 4: ErrMissingGBTTrailer", "Error ID 5: ErrNonZeroPageAfterStop",
                                "Error ID 6: ErrUnstoppedLanes", "Error ID 7: ErrDataForStoppedLane", "Error ID 8: ErrNoDataForActiveLane",
                                "Error ID 9: ErrIBChipLaneMismatch", "Error ID 10: ErrCableDataHeadWrong",
                                "Error ID 11: Jump in RDH_packetCounter" };*/

  TString ErrorType[NError] = { "Error ID 1", "Error ID 2",
                                "Error ID 3", "Error ID 4", "Error ID 5",
                                "Error ID 6", "Error ID 7", "Error ID 8",
                                "Error ID 9", "Error ID 10",
                                "Error ID 11" };

  const float etaCoverage[NLAYERS] = { 2.5, 2.3, 2.0};
  const double PhiMin = 0;
  const double PhiMax = 3.284;
  const int NFiles = 6;
  int ntriggers = 0;


  const int NStaves[3] = { 12, 16, 20 };
  const float StartAngle[3] = { 16.997 / 360 * (M_PI * 2.), 17.504 / 360 * (M_PI * 2.), 17.337 / 360 * (M_PI * 2.)}; // start angle of first stave in each layer
  const float MidPointRad[3] = { 23.49, 31.586, 39.341}; 
  //TH1	*InfoCanvas= nullptr;

  const int mapstave[3][20] = {{10,11,12,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0},{25,26,27,28,13,14,15,16,17,18,19,20,21,22,23,24,0,0,0,0},{44,45,46,47,48,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43}};

  std::map<std::pair<int,int>,std::pair<int,int>> chipmap = {
	{{1,1}, {0,0}},
	{{1,2}, {0,1}},
	{{1,3}, {0,2}},
	{{1,4}, {0,3}},
	{{1,5}, {0,4}},
	{{1,6}, {0,5}},
	{{1,7}, {0,6}},
	{{1,8}, {0,7}},
	{{1,9}, {3,8}},
	{{1,10}, {3,7}},
	{{1,11}, {3,6}},
	{{1,12}, {3,5}},
	{{1,13}, {3,4}},
	{{1,14}, {3,3}},
	{{1,15}, {3,2}},
	{{1,16}, {3,1}},
	{{1,17}, {3,0}},
	{{1,18}, {2,8}},
	{{1,19}, {2,7}},
	{{1,20}, {2,6}},
	{{1,21}, {2,5}},
	{{1,22}, {2,4}},
	{{1,23}, {2,3}},
	{{1,24}, {2,2}},
	{{1,25}, {2,1}},
	{{1,26}, {2,0}},
	{{1,27}, {0,8}},
	{{2,1}, {1,2}},
	{{2,2}, {1,1}},
	{{2,3}, {1,0}},
	{{2,4}, {1,3}},
	{{2,5}, {1,4}},
	{{2,6}, {1,5}},
	{{2,7}, {1,6}},
	{{2,8}, {1,7}},
	{{2,9}, {1,8}}
  }; //<ruid, ruchn> to <stave, chipID>

  private:
  unsigned short decode_row(int hit){	return hit >> 16;}
  unsigned short decode_col(int hit){	return hit & 0xffff;}
  void getStavePoint(int layer, int stave, double* px, double* py);
  void drawLayerName(TH2* histo2D);
  void createPoly(TH2Poly *h);


};


#endif /* MVTX_MVTXMON_H */

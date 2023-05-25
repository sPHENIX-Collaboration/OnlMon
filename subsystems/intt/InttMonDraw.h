#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttMonConstants.h"
#include "InttFelixMap.h"
#include "InttMon.h"

#include <onlmon/OnlMonDraw.h>
#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TStyle.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
#include <TROOT.h>

#include <TLine.h>
#include <TText.h>

#include <TH2D.h>
#include <TH1D.h>

#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

class InttMonDraw : public OnlMonDraw
{
public:
	InttMonDraw(const std::string &name);
	~InttMonDraw() override;

	int Init() override;
	int Draw(const std::string &what = "ALL") override;
	int MakePS(const std::string &what = "ALL") override;
	int MakeHtml(const std::string &what = "ALL") override;

	typedef std::map<std::string, void(*)()> Options_t;
	static Options_t OPTIONS;

	//actual options
	static void GlobalChipChannelHitmap();
	static void GlobalLadderChipHitmap();


	//Main-Spawned idiom
	typedef void(*HistPrepFunc_t)(TH2D*, struct INTT::Indexes_s&);
	typedef std::map<std::string, std::pair<HistPrepFunc_t, HistPrepFunc_t>> ExecOptions_t;
	static ExecOptions_t EXEC_OPTIONS;

	//GlobalChip-Channel idiom
	static void DrawGlobalChipMap(std::string const&);
	static void DrawChannelMap(std::string const&, struct INTT::Indexes_s);
	static void InttGlobalChipExec(const std::string&, int);
	static void InttChannelExec(const std::string&, int, int, int, int);

	//GlobalLadder-Chip idiom
	static void DrawGlobalLadderMap(std::string const&);
	static void DrawChipMap(std::string const&, struct INTT::Indexes_s);
	static void InttGlobalLadderExec(const std::string&, int);
	static void InttChipExec(const std::string&, int, int, int);

	//GlobalChip-Channel methods
	static void PrepHitmapGlobalChipHist(TH2D*, struct INTT::Indexes_s&);
	static void PrepHitmapChannelHist(TH2D*, struct INTT::Indexes_s&);

	//GlobalLadder-Chip methods
	static void PrepHitmapGlobalLadderHist(TH2D*, struct INTT::Indexes_s&);
	static void PrepHitmapChipHist(TH2D*, struct INTT::Indexes_s&);

private:
	OnlMonDB *dbvars = nullptr;

	//===	Constants for Drawing	===//
	static constexpr double NUM_SIG = 2.0;

	static constexpr int CNVS_WIDTH = 1280;
	static constexpr int CNVS_HEIGHT = 720;

	static constexpr double T_MARGIN = 0.05;
	static constexpr double B_MARGIN = 0.05;
	static constexpr double L_MARGIN = 0.05;
	static constexpr double R_MARGIN = 0.05;

	static constexpr double DISP_FRAC = 0.1;
	static constexpr double DISP_TEXT_SIZE = 0.20;

	static constexpr double KEY_FRAC = 0.1;
	static constexpr double LABEL_FRAC = 0.1;
	static constexpr double TITLE_FRAC = 0.1;
	//===	~Constants for Drawing	===//

	//===	Drawing Methods		===//
	static void DrawPad(TPad*, TPad*);
	//===	~Drawing Methods	===//
};

#endif

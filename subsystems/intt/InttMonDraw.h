#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttMon.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <TLine.h>
#include <TPolyLine.h>
#include <TText.h>

#include <TH1D.h>
#include <TH2D.h>

#include <cctype>
#include <cmath>
#include <ctime>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class InttMonDraw : public OnlMonDraw
{
 public:
  // InttMonDraw.cc
  InttMonDraw(std::string const&);
  ~InttMonDraw() override;

  int Init() override;
  int Draw(std::string const& = "ALL") override;
  int MakeHtml(std::string const& = "ALL") override;
  int SavePlot(std::string const& = "ALL", std::string const& = "png") override;

 private:
  // InttMonDraw.cc
  int MakeCanvas(const std::string& name);
  int DrawServerStats();

  // InttMonDraw_o_FelixBcoFphxBco.cc
  struct FelixBcoFphxBco_s
  {
	double cnvs_width, cnvs_height;
	double disp_frac, lgnd_frac;
	double disp_text_size;
	double lgnd_box_width, lgnd_box_height, lgnd_text_size;
	std::string name;
  } static const m_FelixBcoFphxBco;
  int DrawFelixBcoFphxBco(int);
  int DrawFelixBcoFphxBco_DispPad();
  int DrawFelixBcoFphxBco_LgndPad();
  int DrawFelixBcoFphxBco_SubPads();
  int DrawFelixBcoFphxBco_SubPad(int);

  // InttMonDraw_o_HitMap.cc
  struct HitMap_s
  {
	double cnvs_width, cnvs_height;
	double disp_frac, lgnd_frac;
	double disp_text_size;
	double lgnd_box_width, lgnd_box_height, lgnd_text_size;
	double lower, upper;
	std::string name;
  } static const m_HitMap;
  int DrawHitMap(int);
  int DrawHitMap_DispPad();
  int DrawHitMap_LgndPad();
  int DrawHitMap_SubPads();
  int DrawHitMap_SubPad(int);

  // InttMonDraw_o_HitRates.cc
  struct HitRates_s
  {
	double cnvs_width, cnvs_height;
	double disp_frac, lgnd_frac;
	double disp_text_size;
	std::string name;
  } static const m_HitRates;
  int DrawHitRates(int);
  int DrawHitRates_DispPad();
  int DrawHitRates_SubPads();
  int DrawHitRates_SubPad(int);

  // InttMonDraw_o_Peaks.cc
  struct Peaks_s
  {
	double cnvs_width, cnvs_height;
	double disp_frac;
	double disp_text_size;
	double frac;
	double max_width;
	std::string name;
  } static const m_Peaks;
  int DrawPeaks(int);
  int DrawPeaks_DispPad();
  int DrawPeaks_SubPads();
  int DrawPeaks_SubPad(int);
  int DrawPeaks_GetFeePeakAndWidth(int, double*, double*, double*);
  // ...

  // InttMonDraw.cc (Helper functions I want to declare last)
  void static DrawPad(TPad*, TPad*);
  void static CdPad(TPad*);
  Color_t static GetFeeColor(int const&);

  // Member Variables
  TCanvas* TC[999] = {nullptr}; // Make it large--code compiles if I forget to change it, but isn't safe
  TPad* transparent[1] = {nullptr};
};

#endif

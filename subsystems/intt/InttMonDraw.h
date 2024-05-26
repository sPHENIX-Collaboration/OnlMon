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

  // All methods can use these "Generic" ones
  // the way they are used for HitRates
  int MakeCanvas_Generic(int);
  int DrawDispPad_Generic(int);

  // InttMonDraw_o_HitRates.cc
  int Draw_HitRates();
  int DrawHistPad_HitRates(int);
  TH1D* m_hist_hitrates[8] = {nullptr};

  // InttMonDraw_o_FelixBcoFphxBco.cc
  int Draw_FelixBcoFphxBco();
  int DrawLgndPad_FelixBcoFphxBco();
  int DrawHistPad_FelixBcoFphxBco(int);
  TH1D* m_hist_felixbcofphxbco[8][14] = {nullptr};
  Color_t static GetFeeColor(int const&);

  // InttMonDraw_o_HitMap.cc
  int Draw_HitMap();
  int DrawLgndPad_HitMap();
  int DrawHistPad_HitMap(int);
  TH2D* m_hist_hitmap[8] = {nullptr};

  // // InttMonDraw_o_Peaks.cc
  // int Draw_Peaks();
  // int DrawHistPad_Peaks(int);
  // int DrawPeaks_GetFeePeakAndWidth(int, double*, double*, double*);
  // TMultiGraph* m_hist_hitrates[8] = {nullptr};
  // ...

  enum Method_e {
	  k_server_stats = 0, // Reserved for Chris
	  // I don't use it, it just offsets the enum

	  k_hitmap,
	  k_hitrates,
	  k_felixbcofphxbco,
	  k_peaks,

	  k_end = 10,
  };

  // Member Variables
  TCanvas* TC[k_end] = {nullptr};
  TPad* transparent[1] = {nullptr};

  TPad* m_disp_pad[k_end]     = {nullptr};
  TPad* m_lgnd_pad[k_end]     = {nullptr};
  TPad* m_hist_pad[k_end][10] = {nullptr};

  // Some things are universal
  int const static m_cnvs_width = 1280;
  int const static m_cnvs_height = 720;

  double constexpr static m_disp_frac = 0.2;
  double constexpr static m_disp_text_size = 0.25;
  double constexpr static m_warn_text_size = 0.2;
  double constexpr static m_min_events = 50000;

  // Each Draw...() sets these to what it needs them to be
  std::string m_name;
  double m_lgnd_frac = 0.2;
};

#endif

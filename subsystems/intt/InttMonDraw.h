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

  int MakeCanvas_Generic(int);
  int DrawDispPad_Generic(int);

  void static DrawPad(TPad*, TPad*);
  void static CdPad(TPad*);
  Color_t static GetFeeColor(int const&);

  // InttMonDraw_o_HitRates.cc
  int Draw_HitRates(int);
  int DrawSubPad_HitRates(int, int);

  // InttMonDraw_o_FelixBcoFphxBco.cc
  // struct FelixBcoFphxBco_s
  // {
  //   double lgnd_frac;
  //   double lgnd_box_width, lgnd_box_height, lgnd_text_size;
  //   std::string name;
  // } static const m_FelixBcoFphxBco;
  // int DrawFelixBcoFphxBco(int);
  // int DrawFelixBcoFphxBco_LgndPad(int);
  // int DrawFelixBcoFphxBco_SubPads(int);
  // int DrawFelixBcoFphxBco_SubPad(int);

  // InttMonDraw_o_HitMap.cc
  // struct HitMap_s
  // {
  //   double lgnd_frac;
  //   double lgnd_box_width, lgnd_box_height, lgnd_text_size;
  //   double lower, upper;
  //   std::string name;
  // } static const m_HitMap;
  // int DrawHitMap(int);
  // int DrawHitMap_LgndPad(int);
  // int DrawHitMap_SubPads(int);
  // int DrawHitMap_SubPad(int);


  // InttMonDraw_o_Peaks.cc
  // struct Peaks_s
  // {
  //   double peak_frac;
  //   double max_width;
  //   std::string name;
  // } static const m_Peaks;
  // int DrawPeaks(int);
  // int DrawPeaks_SubPads(int);
  // int DrawPeaks_SubPad(int);
  // int DrawPeaks_GetFeePeakAndWidth(int, double*, double*, double*);
  // ...

  int const static ALOT = 99; // Make it large--code compiles if I forget to change it, but isn't safe
  enum Pad_e {
	  // big chunk of space for methods to use how they need
	  k_disp_pad = ALOT,
	  k_lgnd_pad,
      k_end,
  };

  // Member Variables
  TCanvas* TC[ALOT] = {nullptr};
  TPad* TP[ALOT][k_end] = {nullptr};
  TPad* transparent[1] = {nullptr};

  // Not constant so methods can set them as they need
  std::string m_name;
  int m_cnvs_width = 1280;
  int m_cnvs_height = 720;

  double m_disp_frac = 0.2;
  double m_disp_text_size = 0.25;
  double m_warn_text_size = 0.2;

  double m_lgnd_frac = 0.2;
  double m_lgnd_text_size = 0.2;
  double m_lgnd_box_size = 0.2;

  double m_min_events = 50000;
};

#endif

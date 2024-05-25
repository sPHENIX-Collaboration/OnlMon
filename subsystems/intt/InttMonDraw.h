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

  void static DrawPad(TPad*, TPad*);
  void static CdPad(TPad*);
  Color_t static GetFeeColor(int const&);

  // InttMonDraw_o_HitRates.cc
  int Draw_HitRates(int);
  int DrawSubPad_HitRates(int, int);
  TH1D* m_hist_hitrates[8] = {nullptr};

  // InttMonDraw_o_FelixBcoFphxBco.cc
  int Draw_FelixBcoFphxBco(int);
  int DrawLgndPad_FelixBcoFphxBco(int);
  int DrawSubPad_FelixBcoFphxBco(int, int);
  TH1D* m_hist_bco[8][14] = {nullptr};

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
  int DrawHitMap();
  int DrawHitMap_DispPad();
  int DrawHitMap_LgndPad();
  int DrawHitMap_SubPads();
  int DrawHitMap_SubPad(int);

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


  enum Pad_e {
      // big chunk of space for methods to use how they need
      k_disp_pad = 99,
      k_lgnd_pad,
      k_end,
  };

  // Member Variables
  TCanvas* TC[99] = {nullptr};
  TPad* TP[99][k_end] = {nullptr};

  TPad* transparent[4] = {nullptr};
  // Hit map
  TPad* Pad_hit_hist[8]{nullptr};
  TPad* Pad_felixbcofphxbco_hist[8]{nullptr};
  TPad* lgnd_pad[9]{nullptr};
  TPad* disp_pad[9]{nullptr};
  TH2D* hist[8]{nullptr};
  TH1D* hist_FB[8][14]{nullptr};

  // Not constant so methods can set them as they need
  std::string m_name;
  int m_cnvs_width = 1280;
  int m_cnvs_height = 720;

  double m_disp_frac = 0.2;
  double m_disp_text_size = 0.25;
  double m_warn_text_size = 0.2;

  double m_lgnd_frac = 0.2;
  double m_lgnd_text_size = 0.2;
  double m_lgnd_box_width = 0.2;
  double m_lgnd_box_height = 0.2;

  double m_min_events = 50000;
};

#endif

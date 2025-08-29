#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

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
#include <limits>
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

  void set_cold_threshold ( double const& lower ) { m_lower = lower; }
  void set_hot_threshold ( double const& upper ) { m_upper = upper; }

 private:
  static constexpr int NCHIPS = 26;
  static constexpr int NFEES = 14;
  static constexpr int NBCOS = 128;

  int MakeCanvas(const std::string& name);
  int DrawServerStats();

  int MakeDispPad(int icnvs, double lgnd_frac = std::numeric_limits<double>::quiet_NaN());
  int DrawDispPad_Generic(int icnvs, const std::string& title);

  int Draw_FelixBcoFphxBco();
  int DrawHistPad_FelixBcoFphxBco(int i, int icnvs);

  int Draw_JustFphxBco();
  int DrawHistPad_JustFphxBco(int i, int icnvs);

  int Draw_ZoomedFphxBco();
  int DrawHistPad_ZoomedFphxBco(int i, int icnvs);

  Color_t static GetFeeColor(int const&);

  int Draw_HitMap();
  int DrawLgndPad_HitMap();
  int DrawHistPad_HitMap(int i, int icnvs);

  int Draw_HitRates();
  int DrawHistPad_HitRates(int i, int icnvs);

  // int Draw_Peaks();
  // int DrawHistPad_Peaks(int);
  // int DrawPeaks_GetFeePeakAndWidth(int, double*, double*, double*);
  // TMultiGraph* m_hist_hitrates[8] = {nullptr};
  // ...

  int Draw_History();

  enum
  {
    k_server_stats = 0,  // Reserved for Chris
    // I don't use it, it just offsets the enum

    k_felixbcofphxbco,
    k_justfphxbco,
    k_zoomedfphxbco,
    k_hitmap,
    k_hitrates,
    k_peaks,
    k_history,
    k_end
  };

  // Member Variables
  TStyle* m_style{nullptr};  // delete

  TCanvas* TC[k_end]{nullptr};
  TPad* transparent[k_end]{nullptr};

  TPad* m_disp_pad[k_end]{nullptr};
  TPad* m_lgnd_pad[k_end]{nullptr};
  TPad* m_hist_pad[k_end][8]{{nullptr}};
  TPad* m_left_hist_pad[k_end][8]{{nullptr}};
  TPad* m_right_hist_pad[k_end][8]{{nullptr}};
  TPad* m_transparent_pad[k_end][8]{{nullptr}};
  TPad* m_single_hist_pad[k_end]{nullptr};
  TPad* m_single_transparent_pad[k_end]{nullptr};

  TH1* m_hist_felixbcofphxbco[8][14]{{nullptr}};
  TH1* m_hist_justfphxbco[8][14]{{nullptr}};
  TH1* m_left_hist_zoomedfphxbco[8][14]{{nullptr}};
  TH1* m_right_hist_zoomedfphxbco[8][14]{{nullptr}};
  TH1* m_hist_hitrates[8]{nullptr};
  TH2* m_hist_hitmap[8]{nullptr};
  TH1* m_hist_history[8]{nullptr};

  // Some things are universal
  int m_cnvs_width = 1280;
  int m_cnvs_height = 720;

  double constexpr static m_disp_frac = 0.15;
  double constexpr static m_disp_text_size = 0.2;
  double constexpr static m_warn_text_size = 0.15;
  double constexpr static m_min_events = 50000;

  // pp defaults
  // double m_lower = 0.015;
  // double m_upper = 0.650;

  // AuAu defaults
  double m_lower = 0.400;
  double m_upper = 2.500;
};

#endif

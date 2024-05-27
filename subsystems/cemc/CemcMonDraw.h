#ifndef CEMC_CEMCMONDRAW_H
#define CEMC_CEMCMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <Rtypes.h>
#include <TColor.h>

#include <iostream>  // for cout, endl
#include <ostream>   // for operator<<, basic_ostream, ostream
#include <string>    // for allocator, string
#include <vector>

class TCanvas;
class TGraphErrors;
class TH1;
class TH2;
class TPad;
class TProfile;
class TStyle;

class CemcMonDraw : public OnlMonDraw
{
 public:
  explicit CemcMonDraw(const std::string &name = "CEMCMON");  // same name as server!

  ~CemcMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;
  void setSave(int s) { save = s; }
//  void HandleEvent(int event, int x, int y, TObject *sel);

 private:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawThird(const std::string &what = "ALL");
  int DrawFourth(const std::string &what = "ALL");
  int DrawFifth(const std::string &what = "ALL");
  int DrawSixth(const std::string &what = "ALL");
  int DrawSeventh(const std::string &what = "ALL");
  int DrawHistory(const std::string &what = "ALL");
  int DrawServerStats();

  int FindHotTower(TPad *warn, TH2 *);
  time_t getTime();
  std::vector<int> getBadPackets(TH1 *hist, int what, float cutoff);

  const int nTowersEta{96};
  const int nTowersPhi{256};
  const int templateDepth{10000};
  const double SampleLowBoundary{4.5};
  const double SampleHighBoundary{7.5};
  int save{0};

  TCanvas *TC[19]{nullptr};
  // TCanvas *PopUpCanvas{nullptr};
  TPad *transparent[19]{nullptr};
  TPad *Pad[29]{nullptr};
  TPad *warning[29]{nullptr};

  // TPad *PopUpPad[8][8] {{nullptr}};
  // TPad *PopUpTransparent{nullptr};
  TGraphErrors *gr[2]{nullptr};
  // TProfile *summedProfile[8][8]{{nullptr}};
  // TProfile *AllProfiles[256][96]{{nullptr}};
  TH2 *h2_template_hit{nullptr};
  TH2 *h_cemc_datahits{nullptr};
  TStyle *cemcStyle{nullptr};

  TH1 *h1_zs {nullptr};
  TH1 *h1_zs_low {nullptr};
  TH1 *h1_zs_high {nullptr};
  Int_t ZSPalette[255] {0};

  void MakeZSPalette()
  {
    if(ZSPalette[0] > 0) return;
    Double_t red[9] = {1.0, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764};
    Double_t green[9] = {0.0, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832};
    Double_t blue[9] = {0.0, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539};
    Double_t stops[9] = {0.0, 0.04, 0.12, 0.15, 0.2, 0.25, 0.3, 0.35, 1.0000};
    Int_t nb = 255;
    Int_t FI = TColor::CreateGradientColorTable(9, stops, red, green, blue, nb, 1.);

    for (int i = 0; i < nb; i++)
    {
      ZSPalette[i] = FI + i;
    }
  }
};

#endif /* CEMC_CEMCMONDRAW_H */

#ifndef HCAL_HCALMONDRAW_H
#define HCAL_HCALMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <TH2.h>
#include <TStyle.h>

#include <string>  // for allocator, string

class TCanvas;
class TGraphErrors;
class TPad;

class HcalMonDraw : public OnlMonDraw
{
 public:
  explicit HcalMonDraw(const std::string& name);

  ~HcalMonDraw() override {}

  int Init() override;
  int Draw(const std::string& what = "ALL") override;
  int MakeHtml(const std::string& what = "ALL") override;
  int SavePlot(const std::string& what = "ALL", const std::string& type = "png") override;
  void HandleEvent(int, int, int, TObject*);

 private:
  int MakeCanvas(const std::string& name);
  int DrawFirst(const std::string& what = "ALL");
  int DrawSecond(const std::string& what = "ALL");
  int DrawThird(const std::string& what = "ALL");
  int DrawFourth(const std::string& what = "ALL");
  int DrawFifth(const std::string& what = "ALL");
  int DrawSixth(const std::string& what = "ALL");
  int DrawSeventh(const std::string& what = "ALL");
  int DrawServerStats();
  int FindHotTower(TPad* warn, TH2*);
  void DrawTowerAvg();
  void DrawHitMap();
  void DrawAvgTime();
  time_t getTime();

  void MakeZSPalette()
  {
    Double_t red[9] = {1.0, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764};
    Double_t green[9] = {0.0, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832};
    Double_t blue[9] = {0.0, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539};
    Double_t stops[9] = {0.0, 0.04, 0.12, 0.15, 0.2, 0.25, 0.3, 0.35, 1.0000};
    Int_t nb = 255;
    Int_t FI = TColor::CreateGradientColorTable(9, stops, red, green, blue, nb, false);

    for (int i = 0; i < nb; i++)
    {
      ZSPalette[i] = FI + i;
    }

    gStyle->SetNumberContours(nb);
  }


  // int DrawDeadServer(TPad *transparent);
  std::string prefix = "HCALMON";
  TCanvas* TC[10] = {nullptr};
  TPad* transparent[19] = {nullptr};
  TPad* Pad[29] = {nullptr};
  TPad* warning[28] = {nullptr};
  TH2F* h2_mean_template = nullptr;
  TH1F* h1_zs = nullptr;
  TH1F* h1_zs_low = nullptr;
  TH1F* h1_zs_high = nullptr;
  Int_t ZSPalette[255];
  
  //  TGraphErrors* gr[2] = {nullptr};
  TStyle* hcalStyle = nullptr;
};

#endif /* HCAL_HCALMONDRAW_H */

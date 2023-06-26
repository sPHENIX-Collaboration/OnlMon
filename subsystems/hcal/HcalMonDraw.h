#ifndef HCAL_HCALMONDRAW_H
#define HCAL_HCALMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <TH2.h>
#include <TStyle.h>

#include <string>  // for allocator, string

class OnlMonDB;
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
  int MakePS(const std::string& what = "ALL") override;
  int MakeHtml(const std::string& what = "ALL") override;
  void HandleEvent(int, int, int, TObject*);
  

 protected:
  int MakeCanvas(const std::string& name);
  int DrawFirst(const std::string& what = "ALL");
  int DrawSecond(const std::string& what = "ALL");
  int DrawThird(const std::string& what = "ALL");
  int DrawFourth(const std::string& what = "ALL");
  int DrawHistory(const std::string& what = "ALL");
  int FindHotTower(TPad* warn, TH2D*);
  void DrawTowerAvg();
  void DrawHitMap();
  

  // int DrawDeadServer(TPad *transparent);
  std::string prefix = "HCALMON";
  int TimeOffsetTicks = -1;
  TCanvas* TC[9] = {nullptr};
  TPad* transparent[9] = {nullptr};
  TPad* Pad[18] = {nullptr};
  TPad* warning[18] = {nullptr};
  TGraphErrors* gr[2] = {nullptr};
  OnlMonDB* dbvars = nullptr;
  TStyle* hcalStyle = nullptr;
};

#endif /* HCAL_HCALMONDRAW_H */

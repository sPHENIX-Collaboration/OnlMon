#ifndef GL1_GL1MONDRAW_H
#define GL1_GL1MONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <array>
#include <string>  // for allocator, string
#include <vector>

class RunDBodbc;
class TCanvas;
class TGraph;
class TPad;
class TStyle;

class GL1MonDraw : public OnlMonDraw
{
 public:
  GL1MonDraw(const std::string &name);
  ~GL1MonDraw() override;

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;
  int FetchTriggerNames();

 protected:
  int MakeCanvas(const std::string &name);
  int DrawScaled(const std::string &what = "ALL");
  int DrawLive(const std::string &what = "ALL");
  int DrawServerStats();
  int DrawRejection();
  std::vector<TGraph *> reject_graph_good;
  std::vector<TGraph *> reject_graph_bad;
  std::vector<std::pair<int, int>> rejection_limit;
  TStyle *gl1Style{nullptr};
  TCanvas *TC[4]{};
  TPad *transparent[4]{};
  TPad *ScalePad[28]{};
  TPad *LivePad[28]{};
  TPad *RejPad[2]{};
  RunDBodbc *m_RunDB{nullptr};
  int m_CurrentRunnumber{0};
  std::array<std::string, 64> m_TrignameArray;
};

#endif /*GL1_GL1MONDRAW_H */

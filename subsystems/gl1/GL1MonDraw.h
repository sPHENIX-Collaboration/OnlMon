#ifndef GL1_GL1MONDRAW_H
#define GL1_GL1MONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <array>
#include <string>  // for allocator, string

class RunDBodbc;
class TCanvas;
class TGraphErrors;
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
  int DrawRaw(const std::string &what = "ALL");
  TStyle *gl1Style{nullptr}; 
  TCanvas *TC[3] {};
  TPad *transparent[3] {};
  TPad *Pad[10] {};
  RunDBodbc *m_RunDB {nullptr};
  int m_CurrentRunnumber {0};
  std::array<std::string,64> m_TrignameArray;
};

#endif /*GL1_GL1MONDRAW_H */

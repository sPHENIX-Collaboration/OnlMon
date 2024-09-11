#ifndef SINGLE_INTT_DRAWER_H
#define SINGLE_INTT_DRAWER_H

#include <onlmon/SingleCanvasDrawer.h>

#include <string>

class TCanvas;
class TPad;

class SingleInttDrawer : public SingleCanvasDrawer
{
public:
  SingleInttDrawer(std::string const&);
  virtual ~SingleInttDrawer() = default;

  virtual int DrawCanvas() override;

protected:
  virtual int MakeCanvas() override;

  virtual int DrawDisp();
  virtual int DrawLgnd();
  virtual int DrawHist(int);

  int GetFchColor(int);

  using SingleCanvasDrawer::m_name;
  using SingleCanvasDrawer::m_canvas;

  TPad* m_transparent{nullptr};
  TPad* m_disp_pad{nullptr};
  TPad* m_lgnd_pad{nullptr};
  TPad* m_hist_pad[8]{nullptr};
  TPad* m_dead_pad[8]{nullptr};

  float m_disp_frac = 0.15;
  float m_lgnd_frac = 0.15;
  float m_disp_text_size = 0.2;
  float m_dead_text_size = 0.1;
  float m_warn_text_size = 0.15;
  float m_min_events = 50000;
};

#endif//SINGLE_INTT_DRAWER_H

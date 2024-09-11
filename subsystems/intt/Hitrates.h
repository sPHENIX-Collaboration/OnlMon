#ifndef HITRATES_H
#define HITRATES_H

#include "SingleInttDrawer.h"

class TH1;

class Hitrates : public SingleInttDrawer
{
public:
  Hitrates(std::string const& = "intt_hitrates");
  virtual ~Hitrates();

  virtual int DrawCanvas() override;

protected:
  virtual int DrawHist(int) override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_lgnd_frac;
  using SingleInttDrawer::m_hist_pad;

  TH1* m_hist[8]{nullptr};
};

#endif//HITRATES_H

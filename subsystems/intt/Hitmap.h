#ifndef HITMAP_H
#define HITMAP_H

#include "SingleInttDrawer.h"

class TH1;

class Hitmap : public SingleInttDrawer
{
public:
  Hitmap(std::string const& = "intt_hitmap");
  virtual ~Hitmap();

  virtual int DrawCanvas() override;

protected:
  virtual int DrawLgnd() override;
  virtual int DrawHist(int) override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_lgnd_pad;
  using SingleInttDrawer::m_hist_pad;

  TH1* m_hist[8]{nullptr};

};

#endif//HITMAP_H

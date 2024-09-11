#ifndef FPHX_BCO_H
#define FPHX_BCO_H

#include "SingleInttDrawer.h"

class TH1;

class FphxBco : public SingleInttDrawer
{
public:
  FphxBco(std::string const& = "intt_fphx_bco");
  virtual ~FphxBco();

  virtual int DrawCanvas() override;

protected:
  virtual int DrawHist(int) override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_hist_pad;

  TH1* m_hist[8][14]{nullptr};

};

#endif//FPHX_BCO_H


#ifndef ZOOMED_FPHX_BCO_H
#define ZOOMED_FPHX_BCO_H

#include "SingleInttDrawer.h"

class TH1;
class TPad;

class ZoomedFphxBco : public SingleInttDrawer
{
public:
  ZoomedFphxBco(std::string const& = "intt_zoomed_fphx_bco");
  virtual ~ZoomedFphxBco();

  virtual int DrawCanvas() override;

protected:
  virtual int MakeCanvas() override;

  virtual int DrawHist(int) override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_hist_pad;

  TH1* m_l_hist[8][14]{nullptr};
  TH1* m_r_hist[8][14]{nullptr};

  TPad* m_l_hist_pad[8]{nullptr};
  TPad* m_r_hist_pad[8]{nullptr};

};

#endif//ZOOMED_FPHX_BCO_H


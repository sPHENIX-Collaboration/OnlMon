#ifndef BCO_DIFF_H
#define BCO_DIFF_H

#include "SingleInttDrawer.h"

class TH1;

class BcoDiff : public SingleInttDrawer
{
public:
  BcoDiff(std::string const& = "intt_bco_diff");
  virtual ~BcoDiff();

  virtual int DrawCanvas() override;

protected:
  virtual int DrawHist(int) override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_hist_pad;

  TH1* m_hist[8][14]{nullptr};

};

#endif//BCO_DIFF_H

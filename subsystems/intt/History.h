#ifndef HISTORY_H
#define HISTORY_H

#include "SingleInttDrawer.h"

class TH1;
class TPad;

class History : public SingleInttDrawer
{
public:
  History(std::string const& = "intt_history");
  virtual ~History();

  virtual int DrawCanvas() override;

protected:
  virtual int MakeCanvas() override;
  virtual int DrawLgnd() override;

  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_lgnd_pad;

  TH1* m_hist[8]{nullptr};
  TPad* m_single{nullptr};
  TPad* m_dead{nullptr};

};

#endif//HISTORY_H

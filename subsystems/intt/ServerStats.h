#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include "SingleInttDrawer.h"

class TH1;
class TPad;
class OnlMonDraw;

class ServerStats : public SingleInttDrawer
{
public:
  ServerStats(std::string const& = "intt_server_stats", OnlMonDraw* = nullptr);
  virtual ~ServerStats();

  virtual int DrawCanvas() override;

protected:
  using SingleInttDrawer::m_name;
  using SingleInttDrawer::m_canvas;
  using SingleInttDrawer::m_transparent;

  OnlMonDraw* m_onlmondraw{nullptr};

};

#endif//SERVER_STATS_H

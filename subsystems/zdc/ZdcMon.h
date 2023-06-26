#ifndef ZDC_ZDCMON_H
#define ZDC_ZDCMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;

class ZdcMon : public OnlMon
{
 public:
  ZdcMon(const std::string &name = "ZDCMON");
  virtual ~ZdcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int evtcnt = 0;
  int idummy = 0;
  TH1 *zdchist1 = nullptr;
  TH2 *zdchist2 = nullptr;
};

#endif /* ZDC_ZDCMON_H */

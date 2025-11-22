#ifndef GL1_GL1MON_H
#define GL1_GL1MON_H

#include <onlmon/OnlMon.h>

#include <array>

class Event;
class eventReceiverClient;
class TH1;

class GL1Mon : public OnlMon
{
 public:
  GL1Mon(const std::string &name = "GL1MON");
  ~GL1Mon() override;

  int process_event(Event *evt) override;
  int Init() override;
  int BeginRun(const int runno) override;
  int Reset() override;

 private:
  eventReceiverClient* erc {nullptr};
  TH1 *gl1_stats {nullptr};
  std::array<TH1 *, 64> scaledtriggers;
  std::array<TH1 *, 64> livetriggers;
  std::array<TH1 *, 64> rawtriggers;
};

#endif /* GL1_GL1MON_H */

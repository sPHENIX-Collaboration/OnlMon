#ifndef GL1_GL1MON_H
#define GL1_GL1MON_H

#include <onlmon/OnlMon.h>

#include <array>
#include <cstdint>
#include <deque>
#include <map>
#include <vector>

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
  eventReceiverClient *erc{nullptr};
  TH1 *gl1_stats{nullptr};
  std::vector<TH1 *> gl1_reject;
  time_t starttime{0};
  time_t lastupdate{0};
  int64_t n_minbias{0};
  std::vector<int> triggernumber;
  std::vector<int> ntriggers;
  std::vector<std::string> triggername;
  std::map<int, std::string> triggernamemap;
  std::deque<std::pair<int, uint64_t>> eventticdeque;
  std::array<TH1 *, 64> scaledtriggers;
  std::array<TH1 *, 64> livetriggers;
  std::array<TH1 *, 64> rawtriggers;
  std::array<TH1 *,5> TimeToLastEvent;
};

#endif /* GL1_GL1MON_H */

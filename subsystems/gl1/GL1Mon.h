#ifndef GL1_GL1MON_H
#define GL1_GL1MON_H

#include <onlmon/OnlMon.h>

#include <array>

class Event;
class TH1;

class GL1Mon : public OnlMon
{
 public:
  GL1Mon(const std::string &name = "GL1MON");
  ~GL1Mon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  std::array<TH1 *, 64> scaledtriggers;
  std::array<TH1 *, 64> livetriggers;
  std::array<TH1 *, 64> rawtriggers;
};

#endif /* GL1_GL1MON_H */

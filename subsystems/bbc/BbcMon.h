#ifndef BBC_BBCMON_H
#define BBC_BBCMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;
class TRandom3;

class BbcMon : public OnlMon
{
 public:
  explicit BbcMon(const std::string &name = "BBCMON");
  virtual ~BbcMon();

  int process_event(Event *evt);
  int Init();                     // Done once at start of server
  int BeginRun(const int runno);  // Done at every start of run
  int Reset();

 protected:
  int evtcnt = 0;
  int idummy = 0;

  TH2 *bbc_adc = nullptr;
  TH2 *bbc_tdc = nullptr;
  TH2 *bbc_tdc_overflow = nullptr;
  TH1 *bbc_tdc_overflow_each[128] = {};  // should be [nPMT_BBC], need to fix
  // TH1 *bbc_nhit[nTRIGGER];

  TH2 *bbc_tdc_armhittime = nullptr;
  TH1 *bbc_nevent_counter = nullptr;

  TH1 *bbc_zvertex = nullptr;
  TH1 *bbc_zvertex_bbll1 = nullptr;
  // TH1 *bbc_zvertex_zdc = nullptr;
  // TH1 *bbc_zvertex_zdc_scale3 = nullptr;
  TH1 *bbc_zvertex_bbll1_novtx = nullptr;
  TH1 *bbc_zvertex_bbll1_narrowvtx = nullptr;  // Run11 pp
  // TH1 *bbc_zvertex_bbll1_zdc = nullptr;

  TH2 *bbc_tzero_zvtx = nullptr;
  TH1 *bbc_avr_hittime = nullptr;
  TH1 *bbc_south_hittime = nullptr;
  TH1 *bbc_north_hittime = nullptr;
  TH1 *bbc_south_chargesum = nullptr;
  TH1 *bbc_north_chargesum = nullptr;
  TH1 *bbc_prescale_hist = nullptr;

  TRandom3 *trand3 = nullptr;
};

#endif /* BBC_BBCMON_H */

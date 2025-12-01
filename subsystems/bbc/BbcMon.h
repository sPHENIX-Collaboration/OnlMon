#ifndef BBC_BBCMON_H
#define BBC_BBCMON_H

#include <onlmon/OnlMon.h>
#include "BbcMonDefs.h"
#include <onlmon/triggerEnum.h>

#include <string>
#include <ctime>

class Event;
class TH1;
class TH2;
class TF1;
class TH2Poly;
class MbdEvent;
class MbdGeom;
class MbdOut;
class MbdRawContainer;
class MbdPmtContainer;
//class GL1Manager;
class eventReceiverClient;
class Packet;
class RunDBodbc;
// class OnlMonDB;

class BbcMon : public OnlMon
{
 public:
  explicit BbcMon(const std::string &name = "BBCMON");
  virtual ~BbcMon();

  int process_event(Event *evt) override;
  int Init() override;                     // Done once at start of server
  int BeginRun(const int runno) override;  // Done at every start of run
  int EndRun(const int runno) override;    // Done at the end of every run
  int Reset() override;

  void set_GL1(const int g) { useGL1 = g; }
  //void set_skipto(const int s) { skipto = s; }
  void set_skipto(const int s);

 protected:
  int DBVarInit();

  MbdEvent *bevt{nullptr};

  int useGL1{1};    // whether to use the GL1 data
  uint64_t triggervec{0};
  uint64_t triginput{0};
  uint64_t trigraw{0};
  uint64_t triglive{0};
  uint64_t trigscaled{0};
  uint64_t gl1_bco{0};
  uint64_t gl1_offset{0};     // offset needed to get gl1 match
  uint64_t trigmask{0};       // accepted triggers
  uint64_t mbdtrig{0};        // main mbd trigger
  uint64_t mbdwidebest{0};    // best of mbdns n>=1or2, or |z|<150, or zdcns
  uint64_t mbdbest{0};        // best mbd trigger
  uint64_t mbdns{0};          // mbdns n>=1 or 2 bit
  uint64_t mbdnsvtx10{0};     // mbdns vtx<10 bit
  uint64_t mbdnsvtx30{0};     // mbdns vtx<30 bit
  uint64_t mbdnsvtx150{0};     // mbdns vtx<150 bit
  uint64_t zdcns{0};          // zdcns
  uint64_t emcal{0};          // all emcal triggers, no bbc
  uint64_t hcal{0};           // all hcal triggers, no bbc
  uint64_t emcalmbd{0};       // all emcal triggers, with bbc
  uint64_t hcalmbd{0};        // all hcal triggers, with bbc
  uint64_t orig_trigmask{0};  // store for recovering from runs with one trigger defined
  eventReceiverClient *erc{nullptr};
  int      skipto{0};
  static const int nPacketStatus{6};
  uint64_t dclock{0};         // clock diff between BCO and fem clock
  //GL1Manager *gl1mgr{nullptr};
  RunDBodbc *rdb{nullptr};

  uint64_t GetMinBiasTrigBit(uint64_t trigs_enabled);
  uint64_t GetMinBiasWideTrigBit(uint64_t trigs_enabled);

  int      f_evt{0};          // event number
  int      evtcnt{0};
  // OnlMonDB *dbvars = nullptr;

  MbdGeom *_mbdgeom{nullptr};  // contains positions of BBC PMTs
  MbdOut *m_mbdout{nullptr};
  MbdRawContainer *m_mbdraws{nullptr};
  MbdPmtContainer *m_mbdpmts{nullptr};

  // vtx to MCR variables
  int fillnumber{0};
  int sendflag{0};      // 0 = don't send, 1 = send
  std::string sendflagfname;
  int GetFillNumber();
  int GetSendFlag();
  int UpdateSendFlag(const int flag);
  std::time_t prev_send_time{0};
  std::time_t tstart{0};

  // vtx reset variables
  int zresetflag{0};      // 0 = don't reset, 1 = reset
  std::string zresetflagfname;
  int GetZResetFlag();
  int UpdateZResetFlag(const int flag);

  // kludge to work around situations when gl1 events are being received
  int gl1badflag{0};   // 0 = normal, 1 = gl1 bad, accept all events
  std::string gl1badflagfname;
  int GetGL1BadFlag();
  int UpdateGL1BadFlag(const int flag);
  int FindGoodGL1(Event*& g, Packet*& p, uint64_t femclk);

  TH1 *bbc_trigs{nullptr};
  TH2 *bbc_adc{nullptr};
  TH2 *bbc_tdc{nullptr};
  TH2 *bbc_tdc_overflow{nullptr};
  TH1 *bbc_tdc_overflow_each[128] = {};  // should be [nPMT_BBC], need to fix
  TH1 *bbc_south_nhit{nullptr};
  TH1 *bbc_north_nhit{nullptr};
  TH1 *bbc_nhit_emcal[2]{nullptr,nullptr};
  TH1 *bbc_nhit_hcal[2]{nullptr,nullptr};
  TH1 *bbc_nhit_emcalmbd[2]{nullptr,nullptr};
  TH1 *bbc_nhit_hcalmbd[2]{nullptr,nullptr};
  TH1 *h1_packet_status[nPacketStatus]{nullptr};

  TH1 * bbc_zvertex_autoupdate[TriggerEnum::NUM_MBD_TRIGGERS]{nullptr};  // BBCMON_NTRIGS = 16, for all triggers
  unsigned int bbc_last_update_ticker[TriggerEnum::NUM_MBD_TRIGGERS]{0};  // last update ticker for zvtx autoupdate

  TH2 *bbc_tdc_armhittime{nullptr};
  TH1 *bbc_nevent_counter{nullptr};

  TH1 *bbc_zvertex{nullptr};        // whatever primary trigger is
  TH1 *bbc_zvertex_alltrigger{nullptr};  // all triggers
  TH1 *bbc_zvertex_short{nullptr};  // Used for short time scales, exclusively wide trigs
  TH1 *bbc_zvertex_prime_short{nullptr};  // Used for short time scales, primary trig
  TH1 *bbc_zvertex_ns{nullptr};     // no vtx cut
  TH1 *bbc_zvertex_10{nullptr};     // 10 cm cut
  TH1 *bbc_zvertex_30{nullptr};
  TH1 *bbc_zvertex_60{nullptr};
  TH1 *bbc_zvertex_ns_chk{nullptr};     // for checking the vertex cut only
  TH1 *bbc_zvertex_10_chk{nullptr};     // for checking the vertex cut only
  TH1 *bbc_zvertex_30_chk{nullptr};
  TH1 *bbc_zvertex_60_chk{nullptr};
  TH1 *bbc_zvertex_zdcns{nullptr};  // ZDCNS triggers
  TH1 *bbc_zvertex_emcal{nullptr};  // EMCAL triggers
  TH1 *bbc_zvertex_hcal{nullptr};   // HCAL triggers
  TH1 *bbc_zvertex_emcalmbd{nullptr};  // EMCAL triggers, w/ BBC
  TH1 *bbc_zvertex_hcalmbd{nullptr};   // HCAL triggers, w/ BBC
  TF1 *f_zvtx{nullptr};             // Used for fitting central vertex peak
  // TH1 *bbc_zvertex_bbll1_novtx = nullptr;
  // TH1 *bbc_zvertex_bbll1_narrowvtx = nullptr;  // Run11 pp

  TH2 *bbc_tzero_zvtx{nullptr};
  TH1 *bbc_avr_hittime{nullptr};
  TH1 *bbc_south_hittime{nullptr};
  TH1 *bbc_north_hittime{nullptr};
  TH1 *bbc_south_chargesum{nullptr};
  TH1 *bbc_north_chargesum{nullptr};
  TH1 *bbc_prescale_hist{nullptr};

  TH1 *bbc_runvtx{nullptr};       // vtx, running vertex measurement
  TH1 *bbc_runvtxerr{nullptr};    // vtxerr, running vertex measurement
  TH1 *bbc_runvtxtime{nullptr};   // time, running vertex measurement

  // TH2 *bbc_tmax[2] = {};  // [0 == time ch, 1 == chg ch], max sample in evt vs ch
  TH2 *bbc_time_wave{nullptr};    // raw waveforms, time ch
  TH2 *bbc_charge_wave{nullptr};  // raw waveforms, charge ch
  // TH2 *bbc_trange = nullptr;       // subtracted tdc at maxsamp vs ch

  TH2Poly *bbc_south_hitmap{nullptr};  // hitmap
  TH2Poly *bbc_north_hitmap{nullptr};  // hitmap
  const unsigned int zvtx_autoupdate_ticker = 1000;

};

#endif /* BBC_BBCMON_H */

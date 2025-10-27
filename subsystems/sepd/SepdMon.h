#ifndef SEPD_SEPDMON_H
#define SEPD_SEPDMON_H

#include <onlmon/OnlMon.h>
#include <cmath>
#include <vector>

class CaloWaveformFitting;
class TowerInfoContainer;
class Event;
class TH1;
class TH2;
class Packet;
class TProfile;
class TProfile2D;
class runningMean;
class eventReceiverClient;

class SepdMon : public OnlMon
{
 public:
  SepdMon(const std::string &name);
  virtual ~SepdMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  std::vector<float> getSignal(Packet *p, const int channel);
  std::vector<float> anaWaveformFast(Packet *p, const int channel);
  std::vector<float> anaWaveformTemp(Packet *p, const int channel);
  int evtcnt = 0;
  int idummy = 0;

  // --- copied straight from the MBD
  uint64_t mbdtrig{0};        // main mbd trigger
  uint64_t mbdns{0};          // mbdns n>=1 or 2 bit
  uint64_t mbdnsvtx10{0};     // mbdns vtx<10 bit
  uint64_t mbdnsvtx30{0};     // mbdns vtx<30 bit
  uint64_t mbdnsvtx150{0};     // mbdns vtx<150 bit

  const int Nsector = 24;
  const int Nchannel = 192 * 4;
  const int packetlow = 9001;
  const int packethigh = 9006;
  const int m_nChannels = 192;

  const int packet_depth = 1000;
  const float hit_threshold = 10;
  const int n_samples_show = 31;

  const int nChannels = 744;
  int nPhi0 = 12;
  int nPhi = 24;
  int nRad = 24;
  double axislimit = M_PI;
  static const int nPacketStatus{6};


  TH2 *h_ADC_corr = nullptr;
  TH2 *h_hits_corr = nullptr;

  TH1 *h1_waveform_twrAvg = nullptr;
  TH1 *h1_waveform_time = nullptr;
  TH1 *h1_waveform_pedestal = nullptr;
  TH1 *h_event = nullptr;
  TH2 *h2_sepd_waveform = nullptr;

  TH1 *h1_sepd_fitting_sigDiff = nullptr;
  TH1 *h1_sepd_fitting_pedDiff = nullptr;
  TH1 *h1_sepd_fitting_timeDiff = nullptr;

  TH1 *h1_packet_chans = nullptr;
  TH1 *h1_packet_length = nullptr;
  TH1 *h1_packet_number = nullptr;
  TH1 *h1_packet_event = nullptr;

  TProfile *p_noiserms_all_channel{nullptr};
  TH1 *h_hits_all_channel = nullptr;
  TH1 *h_ADC_all_channel = nullptr;
  TH1 *h_ADC_channel[768] = {nullptr};
  TH1* h1_packet_status[nPacketStatus] = {nullptr};

  //TH1 *h_ADC_channel[744] = {nullptr};

  std::string runtypestr = "Unknown";
  std::string id_string;

  CaloWaveformFitting *WaveformProcessingFast = nullptr;
  CaloWaveformFitting *WaveformProcessingTemp = nullptr;
  eventReceiverClient *erc = nullptr;

  std::vector<runningMean *> rm_packet_number;
  std::vector<runningMean *> rm_packet_length;
  std::vector<runningMean *> rm_packet_chans;
  std::vector<runningMean *> rm_packet_event;
};

#endif /* SEPD_SEPDMON_H */

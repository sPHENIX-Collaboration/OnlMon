// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "SepdMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>
#include <onlmon/pseudoRunningMean.h>
#include <onlmon/triggerEnum.h> // for trigger selection

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/eventReceiverClient.h>
#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TRandom.h>

#include <cmath>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

SepdMon::SepdMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new EpdMon()
  return;
}

SepdMon::~SepdMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  std::vector<runningMean *>::iterator rm_it;
  for (auto iter : rm_packet_number)
  {
    delete iter;
  }
  for (auto iter : rm_packet_length)
  {
    delete iter;
  }
  for (auto iter : rm_packet_chans)
  {
    delete iter;
  }
  for (auto iter : rm_packet_event)
  {
    delete iter;
  }
  if (erc)
  {
    delete erc;
  }
  return;
}

int SepdMon::Init()
{
  gRandom->SetSeed(rand());
  // read our calibrations from EpdMonData.dat
  /*
  const char *sepdcalib = getenv("SEPDCALIB");
  if (!sepdcalib)
  {
    std::cout << "SEPDCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(sepdcalib) + "/" + "SepdMonData.dat";
  std::ifstream calib(fullfile);

  calib.close();
   */
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  // --- copied straight from the MBD code
  mbdns = (0x1UL << TriggerEnum::MBD_NS2) | (0x1UL << TriggerEnum::MBD_NS1); // mbd wide triggers
  mbdnsvtx10 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX10) | (0x1UL << TriggerEnum::MBD_NS1_ZVRTX10);
  mbdnsvtx30 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX30);
  mbdnsvtx150 = (0x1UL << TriggerEnum::MBD_NS2_ZVRTX150);
  mbdtrig = mbdns | mbdnsvtx10 | mbdnsvtx30 | mbdnsvtx150;


  h_ADC_all_channel = new TH1D("h_ADC_all_channel",";;",768,-0.5,767.5);
  h_hits_all_channel = new TH1D("h_hits_all_channel",";;",768,-0.5,767.5);

  p_noiserms_all_channel = new TProfile("p_noiserms_all_channel","",768,0,768,"S");

  int nADCcorr = 500;
  double ADCcorrmax = 6e6;
  int nhitscorr = 500;
  double hitscorrmax = 1000;
  h_ADC_corr = new TH2F("h_ADC_corr", ";ADC sum (south); ADC sum (north)", nADCcorr, 0, ADCcorrmax, nADCcorr, 0, ADCcorrmax);
  h_hits_corr = new TH2F("h_hits_corr", ";N hits sum (south); N hits sum (north)", nhitscorr, 0, hitscorrmax, nhitscorr, 0, hitscorrmax);

  h_event = new TH1D("h_event", "", 1, 0, 1);

  // waveform processing
  h1_waveform_twrAvg = new TH1D("h1_waveform_twrAvg", "", n_samples_show, 0.5, n_samples_show + 0.5);
  h1_waveform_time = new TH1D("h1_waveform_time", "", n_samples_show, 0.5, n_samples_show + 0.5);
  //h1_waveform_pedestal = new TH1D("h1_waveform_pedestal", "", 42, 1.0e3, 2.0e3);
  // --- wider range and more bins, copied from HCal
  h1_waveform_pedestal = new TH1F("h1_waveform_pedestal", "", 5e3, 0, 5e3);
  h2_sepd_waveform = new TH2F("h2_sepd_waveform", "", n_samples_show, 0.5, n_samples_show + 0.5, 1000, 0, 15000);

  // waveform processing, template vs. fast interpolation
  h1_sepd_fitting_sigDiff = new TH1D("h1_fitting_sigDiff", "", 50, 0, 2);
  h1_sepd_fitting_pedDiff = new TH1D("h1_fitting_pedDiff", "", 50, 0, 2);
  h1_sepd_fitting_timeDiff = new TH1D("h1_fitting_timeDiff", "", 50, -10, 10);

  // packet stuff
  h1_packet_number = new TH1D("h1_packet_number", "", 6, packetlow - 0.5, packethigh + 0.5);
  h1_packet_length = new TH1D("h1_packet_length", "", 6, packetlow - 0.5, packethigh + 0.5);
  h1_packet_chans = new TH1D("h1_packet_chans", "", 6, packetlow - 0.5, packethigh + 0.5);
  h1_packet_event = new TH1D("h1_packet_event", "", 6, packetlow - 0.5, packethigh + 0.5);
  for(int i = 0; i < nPacketStatus; i++) h1_packet_status[i] = new TH1F(Form("h1_packet_status_%d",i),"",5,packetlow-0.5, packethigh+0.5);


  for (int i = 0; i < 6; i++)
  {
    rm_packet_number.push_back(new pseudoRunningMean(1, packet_depth));
    rm_packet_length.push_back(new pseudoRunningMean(1, packet_depth));
    rm_packet_chans.push_back(new pseudoRunningMean(1, packet_depth));
    rm_packet_event.push_back(new pseudoRunningMean(1, packet_depth));
  }

  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  // first histogram uses the TH1->GetName() as key
  se->registerHisto(this, h_ADC_all_channel);
  se->registerHisto(this, h_hits_all_channel);
  se->registerHisto(this, h_ADC_corr);
  se->registerHisto(this, h_hits_corr);
  se->registerHisto(this, h_event);
  se->registerHisto(this, h1_waveform_twrAvg);
  se->registerHisto(this, h1_waveform_time);
  se->registerHisto(this, h1_waveform_pedestal);
  se->registerHisto(this, h2_sepd_waveform);
  se->registerHisto(this, h1_packet_number);
  se->registerHisto(this, h1_packet_length);
  se->registerHisto(this, h1_packet_chans);
  se->registerHisto(this, h1_packet_event);
  for(int i = 0; i < nPacketStatus; i++) se->registerHisto(this, h1_packet_status[i]);

  //  se->registerHisto(this, h1_sepd_fitting_sigDiff);
  //  se->registerHisto(this, h1_sepd_fitting_pedDiff);
  //  se->registerHisto(this, h1_sepd_fitting_timeDiff);

  // save inidividual channel ADC distribution
  //for (int ichannel = 0; ichannel < nChannels; ichannel++)
  for (int ichannel = 0; ichannel < 768; ichannel++)
  {
    h_ADC_channel[ichannel] = new TH1D(Form("h_ADC_channel_%d", ichannel), ";ADC;Counts", 1000, 0, 1000);
    se->registerHisto(this, h_ADC_channel[ichannel]);
  }
  se->registerHisto(this, p_noiserms_all_channel);

  // initialize waveform extraction tool
  WaveformProcessingFast = new CaloWaveformFitting();

  WaveformProcessingTemp = new CaloWaveformFitting();

  std::string sepdtemplate;
  if (getenv("SEPDCALIB"))
  {
    sepdtemplate = getenv("SEPDCALIB");
  }
  else
  {
    sepdtemplate = ".";
  }
  sepdtemplate += std::string("/testbeam_sepd_template.root");
  // WaveformProcessingTemp->initialize_processing(sepdtemplate);

  Reset();

  erc = new eventReceiverClient(eventReceiverClientHost);

  return 0;
}

int SepdMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  std::vector<runningMean *>::iterator rm_it;
  for (rm_it = rm_packet_number.begin(); rm_it != rm_packet_number.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  for (rm_it = rm_packet_length.begin(); rm_it != rm_packet_length.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  for (rm_it = rm_packet_chans.begin(); rm_it != rm_packet_chans.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  for (rm_it = rm_packet_event.begin(); rm_it != rm_packet_event.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  // if (anaGL1)
  // {
  OnlMonServer *se = OnlMonServer::instance();
  se->UseGl1();
  // }
  return 0;
}

// simple wavefrom analysis for possibe issues with the wavforProcessor
std::vector<float> SepdMon::getSignal(Packet *p, const int channel)
{
  double baseline = 0;
  for (int s = 0; s < 3; s++)
  {
    baseline += p->iValue(s, channel);
  }
  baseline /= 3.;

  double signal = 0;
  float x = 0;
  for (int s = 3; s < p->iValue(0, "SAMPLES"); s++)
  {
    x++;
    signal += p->iValue(s, channel) - baseline;
  }

  signal /= x;

  // simulate a failure  if ( evtcount > 450 && p->getIdentifier() ==6011) return 0;

  std::vector<float> result;
  result.push_back(signal);
  result.push_back(2);
  result.push_back(1);
  return result;
}

std::vector<float> SepdMon::anaWaveformFast(Packet *p, const int channel)
{
  std::vector<float> waveform;
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_sepd;
  fitresults_sepd = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_sepd.at(0);

  return result;
}

std::vector<float> SepdMon::anaWaveformTemp(Packet *p, const int channel)
{
  std::vector<float> waveform;
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_sepd;
  fitresults_sepd = WaveformProcessingTemp->process_waveform(multiple_wfs);

  std::vector<float> result;
  result = fitresults_sepd.at(0);

  return result;
}

int SepdMon::process_event(Event *e /* evt */)
{
  evtcnt++;
  //h1_packet_event->Reset(); // not sure about this...
  unsigned int ChannelNumber = 0;
  //  float sectorAvg[Nsector] = {0};
  // int phi_in = 0;
  // float phi;
  // float r;
  int sumhit_s = 0;
  int sumhit_n = 0;
  long double sumADC_s = 0;
  long double sumADC_n = 0;

  // --- get the trigger info...
  bool is_trigger_okay = false;
  int evtnr = e->getEvtSequence();
  Event* gl1Event = erc->getEvent(evtnr);
  // --- only do this insanity for diagnostic purposes and a small number of events
  // std::cout << "event number evtnr is " << evtnr << std::endl;
  // std::cout << "gl1Event pointer is " << gl1Event << std::endl;
  if ( gl1Event )
    {
      OnlMonServer *se = OnlMonServer::instance();
      se->IncrementGl1FoundCounter();
      Packet* pgl1 = gl1Event->getPacket(14001);
      // --- only do this insanity for diagnostic purposes and a small number of events
      // std::cout << "gl1 packet 14001 pointer is " << pgl1 << std::endl;
      if ( pgl1 )
        {
          uint64_t triggervec = pgl1->lValue(0, "ScaledVector");
          is_trigger_okay = triggervec & mbdtrig;
          // --- only do this insanity for diagnostic purposes and a small number of events
          // std::cout << "triggervec is " << triggervec << std::endl;
          // std::cout << "mbdtrig is " << mbdtrig << std::endl;
          // std::cout << "trigger decision is " << is_trigger_okay << std::endl;
          delete pgl1;
        }
      delete gl1Event;
    }

  uint64_t zdc_clock = 0;
  Packet* pzdc = e->getPacket(12001);
  if ( pzdc )
    {
      zdc_clock = pzdc->lValue(0,"CLOCK");
      // --- only do this insanity for diagnostic purposes and a small number of events
      // std::cout << "ZDC clock is " << zdc_clock << std::endl;
      delete pzdc;
    }
  // --- only do this insanity for diagnostic purposes and a small number of events
  // else std::cout << "Why no ZDC packet..." << std::endl;

  // loop over packets which contain a single sector
  for (int packet = packetlow; packet <= packethigh; packet++)
  {
    Packet *p = e->getPacket(packet);
    int packet_index = packet - packetlow;
    int packet_bin = packet - packetlow + 1;
    if (p)
    {
      int one[1] = {1};
      rm_packet_number[packet_index]->Add(one);
      int packet_length[1] = {p->getLength()};
      rm_packet_length[packet_index]->Add(packet_length);
      h1_packet_status[(int)p->getStatus()]->Fill(packet);
      h1_packet_length->SetBinContent(packet_bin, rm_packet_length[packet_index]->getMean(0));

      // ---
      uint64_t p_clock = p->lValue(0,"CLOCK");
      long long clock_diff = p_clock-zdc_clock;
      double cd = (double)clock_diff;
      // --- only do this insanity for diagnostic purposes and a small number of events
      // std::cout << "Packet clock is " << p_clock << " and clock diff is " << clock_diff << " " << cd << std::endl;
      // --- trying to improve clock diff...
      // --- currently we overwrite the histogram with the difference from the latest event
      // --- not sure if we want to do that or running mean, which is done for the number, length, channels
      // h1_packet_event->SetBinContent(packet - packetlow + 1, p->lValue(0, "CLOCK"));
      // h1_packet_event->SetBinContent(packet_bin, clock_diff);
      // h1_packet_event->SetBinContent(packet_bin, cd);
      rm_packet_event[packet_index]->Add(&cd);
      h1_packet_event->SetBinContent(packet_bin, rm_packet_event[packet_index]->getMean(0));
      // --- only do this insanity for diagnostic purposes and a small number of events
      // std::cout << "Histogram bin " << packet_bin << " center " << h1_packet_event->GetBinCenter(packet_bin) << " bin content " << h1_packet_event->GetBinContent(packet_bin) << std::endl;
      int nPacketChannels = p->iValue(0, "CHANNELS");
      if (nPacketChannels > m_nChannels)
      {
        return -1;  // packet is corrupted, reports too many channels
      }
      // else
      // {
      //   rm_packet_chans[packet - packetlow]->Add(&nChannels);
      //   h1_packet_chans->SetBinContent(packet_bin, rm_packet_chans[packet - packetlow]->getMean(0));
      // }
      int channel_counter = 0;
      for (int c = 0; c < p->iValue(0, "CHANNELS"); c++)
      {
        // msg << "Filling channel: " << c << " for packet: " << packet << std::endl;
        // se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), TRGMESSAGE);
        //  record waveform to show the average waveform
        channel_counter++;

        ChannelNumber++;
        int ch = ChannelNumber-1;

        // -- bit flipped ADC channels
        //bool reject_this_channel = false;
        //if ( ( packet == 9001 || packet == 9002 || packet == 9006 ) && c == 30 )
        //  reject_this_channel = true;

        //if ( reject_this_channel ) continue;

        // std::vector result =  getSignal(p,c); // simple peak extraction
        std::vector<float> resultFast = anaWaveformFast(p, c);  // fast waveform fitting
        float signalFast = resultFast.at(0);
        float timeFast = resultFast.at(1);
        float pedestalFast = resultFast.at(2);
        float prepost = p->iValue(c, "PRE") - p->iValue(c, "POST");
        if ( prepost > 0 )
          {
            p_noiserms_all_channel->Fill(ch,prepost);
            p_noiserms_all_channel->Fill(ch,-prepost);
          }

        // --- Run24pp
        //bool is_good_hit = ( signalFast > 50 && signalFast < 3000 );
        // --- Run25AuAu
        bool is_good_hit = ( signalFast > 50 && signalFast < 15000 );

        // std::vector<float> resultTemp = anaWaveformTemp(p, c);  // template waveform fitting
        // float signalTemp = resultTemp.at(0);
        // float timeTemp  = resultTemp.at(1);
        // float pedestalTemp = resultTemp.at(2);
        if (signalFast > hit_threshold)
        {
          for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
          {
            h2_sepd_waveform->Fill(s, p->iValue(s, c) - pedestalFast);
          }
        }
        // ---
        if ( signalFast > 0 && signalFast < 1e10 )
          {
            // --- 1d ADC distributions for all channels
            h_ADC_channel[ch]->Fill(signalFast);
            // --- total ADC vs channel number
            h_ADC_all_channel->Fill(ch,signalFast);
            // --- total hits vs channel number
            if ( is_good_hit ) h_hits_all_channel->Fill(ch);
            // --- 1d waveform
            h1_waveform_time->Fill(timeFast);
            h1_waveform_pedestal->Fill(pedestalFast);
          }
        // ---

        // h1_sepd_fitting_sigDiff -> Fill(signalFast/signalTemp);
        // h1_sepd_fitting_pedDiff -> Fill(pedestalFast/pedestalTemp);
        // h1_sepd_fitting_timeDiff -> Fill(timeFast - timeTemp);

        int z_bin = -1;
        if ( ch >= 384 && ch <= 767 ) z_bin = 0;
        if ( ch <= 383 && ch >= 0 ) z_bin = 1;

        if ( z_bin == 0 && is_good_hit )
        {
          sumhit_s++;
          sumADC_s += signalFast;
        }
        if ( z_bin == 1 && is_good_hit )
        {
          sumhit_n++;
          sumADC_n += signalFast;
        }

      }  // channel loop end
    rm_packet_chans[packet_index]->Add(&channel_counter);
    h1_packet_chans->SetBinContent(packet_bin, rm_packet_chans[packet_index]->getMean(0));
    }    //  if packet good
    else
    {
      ChannelNumber += 128;
      int zero[1] = {0};
      rm_packet_number[packet_index]->Add(zero);
    }
    h1_packet_number->SetBinContent(packet_bin, rm_packet_number[packet_index]->getMean(0));
    delete p;

  }  // packet id loop end

  h_event->Fill(0);
  // h1_waveform_twrAvg->Scale(1. / 32. / 48.);  // average tower waveform
  h1_waveform_twrAvg->Scale((float) 1 / ChannelNumber);

  // --- need to make a trigger selection?
  if ( is_trigger_okay )
    {
      h_ADC_corr->Fill(sumADC_s, sumADC_n);
      h_hits_corr->Fill(sumhit_s, sumhit_n);
    }

  return 0;

}

int SepdMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;

  return 0;
}

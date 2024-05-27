// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "HcalMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>
#include <onlmon/pseudoRunningMean.h>

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/eventReceiverClient.h>
#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

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

HcalMon::HcalMon(const std::string& name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new HcalMon()
  // if name start with O then packetlow = 8001, packethigh = 8008
  // if name start with I then packetlow = 7001, packethigh = 7008
  if (name[0] == 'O')
  {
    packetlow = 8001;
    packethigh = 8008;
  }
  else if (name[0] == 'I')
  {
    packetlow = 7001;
    packethigh = 7008;
  }
  else
  {
    std::cout << "HcalMon::HcalMon - unknown name(need to be OHCALMON or IHCALMON to know what packet to run) " << name << std::endl;
    exit(1);
  }
  return;
}

HcalMon::~HcalMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  delete WaveformProcessing;
  for (auto iter : rm_vector_sectAvg)
  {
    delete iter;
  }
  for (auto iter : rm_vector_twr)
  {
    delete iter;
  }
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
  for (auto iter : rm_vector_twrTime)
  {
    delete iter;
  }
  for (auto iter : rm_vector_twrhit)
  {
    delete iter;
  }

  if (erc)
  {
    delete erc;
  }

  return;
}

const int depth = 50000;
const int packet_depth = 1000;
const int historyLength = 100;
const int historyScaleDown = 100;
// const int n_channel = 48;
const float hit_threshold = 30;
const float waveform_hit_threshold = 100;
const int n_samples_show = 31;

int HcalMon::Init()
{
  // read our calibrations from HcalMonData.dat
  /*

  const char *hcalcalib = getenv("HCALCALIB");
  if (!hcalcalib)
  {
    std::cout << "HCALCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(hcalcalib) + "/" + "HcalMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  */
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  for (int itrig = 0; itrig < 64; itrig++)
  {
    h2_hcal_hits_trig[itrig] = new TH2F(Form("h2_hcal_hits_trig_%d", itrig), "", 24, 0, 24, 64, 0, 64);
  }

  pr_zsFrac_etaphi = new TProfile2D("pr_zsFrac_etaphi", "", 24, 0, 24, 64, 0, 64);
  h_hcal_trig = new TH1F("h_hcal_trig", "", 64, 0, 64);
  h2_hcal_rm = new TH2F("h2_hcal_rm", "", 24, 0, 24, 64, 0, 64);
  h2_hcal_mean = new TH2F("h2_hcal_mean", "", 24, 0, 24, 64, 0, 64);
  h2_hcal_time = new TH2F("h2_hcal_time", "", 24, 0, 24, 64, 0, 64);
  h2_hcal_hits = new TH2F("h2_hcal_hits", "", 24, 0, 24, 64, 0, 64);
  h2_hcal_waveform = new TH2F("h2_hcal_waveform", "", n_samples_show, 0.5, n_samples_show + 0.5, 1000, 0, 15000);
  h2_hcal_correlation = new TH2F("h2_hcal_correlation", "", 200, 0, 100000, 200, 0, 150000);
  h_event = new TH1F("h_event", "", 1, 0, 1);
  h_waveform_twrAvg = new TH1F("h_waveform_twrAvg", "", n_samples_show, 0.5, n_samples_show + 0.5);
  h_waveform_time = new TH1F("h_waveform_time", "", n_samples_show, 0.5, n_samples_show + 0.5);
  h_waveform_pedestal = new TH1F("h_waveform_pedestal", "", 5e3, 0, 5e3);
  h_sectorAvg_total = new TH1F("h_sectorAvg_total", "", 32, 0.5, 32.5);
  // number of towers above threshold per event
  h_ntower = new TH1F("h_ntower", "", 100, 0, 800);
  // packet stuff
  h1_packet_number = new TH1F("h1_packet_number", "", 8, packetlow - 0.5, packethigh + 0.5);
  h1_packet_length = new TH1F("h1_packet_length", "", 8, packetlow - 0.5, packethigh + 0.5);
  h1_packet_chans = new TH1F("h1_packet_chans", "", 8, packetlow - 0.5, packethigh + 0.5);
  h1_packet_event = new TH1F("h1_packet_event", "", 8, packetlow - 0.5, packethigh + 0.5);
  h_caloPack_gl1_clock_diff = new TH2F("h_caloPack_gl1_clock_diff", "", 8, packetlow - 0.5, packethigh + 0.5, 65536, 0, 65536);
  h_evtRec = new TProfile("h_evtRec", "", 1, 0, 1);

  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih] = new TH1F(Form("h_rm_sectorAvg_s%d", ih), "", historyLength, 0, historyLength * historyScaleDown);
  }
  for (int ieta = 0; ieta < 24; ieta++)
  {
    for (int iphi = 0; iphi < 64; iphi++)
    {
      h_rm_tower[ieta][iphi] = new TH1F(Form("h_rm_tower_%d_%d", ieta, iphi), Form("multiplicity running mean of tower ieta=%d, iphi=%d", ieta, iphi), historyLength, 0, historyLength * historyScaleDown);
    }
  }
  // make the per-packet running mean objects
  // 32 packets and 48 channels for hcal detectors
  for (int i = 0; i < Nsector; i++)
  {
    rm_vector_sectAvg.push_back(new pseudoRunningMean(1, depth));
  }
  for (int i = 0; i < Ntower; i++)
  {
    rm_vector_twr.push_back(new pseudoRunningMean(1, depth));
    rm_vector_twrTime.push_back(new pseudoRunningMean(1, depth));
    rm_vector_twrhit.push_back(new pseudoRunningMean(1, depth));
  }
  for (int i = 0; i < 8; i++)
  {
    rm_packet_number.push_back(new pseudoRunningMean(1, packet_depth));
    rm_packet_length.push_back(new pseudoRunningMean(1, packet_depth));
    rm_packet_chans.push_back(new pseudoRunningMean(1, packet_depth));
  }

  OnlMonServer* se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, h2_hcal_hits);
  for (int itrig = 0; itrig < 64; itrig++)
  {
    se->registerHisto(this, h2_hcal_hits_trig[itrig]);
  }

  se->registerHisto(this, pr_zsFrac_etaphi);
  se->registerHisto(this, h_hcal_trig);
  se->registerHisto(this, h_evtRec);
  se->registerHisto(this, h_caloPack_gl1_clock_diff);
  se->registerHisto(this, h2_hcal_rm);
  se->registerHisto(this, h2_hcal_mean);
  se->registerHisto(this, h2_hcal_time);
  se->registerHisto(this, h2_hcal_waveform);
  se->registerHisto(this, h_event);
  se->registerHisto(this, h_sectorAvg_total);
  se->registerHisto(this, h_waveform_twrAvg);
  se->registerHisto(this, h_waveform_time);
  se->registerHisto(this, h_waveform_pedestal);
  se->registerHisto(this, h_ntower);
  se->registerHisto(this, h1_packet_number);
  se->registerHisto(this, h1_packet_length);
  se->registerHisto(this, h1_packet_chans);
  se->registerHisto(this, h1_packet_event);
  se->registerHisto(this, h2_hcal_correlation);

  for (auto& ih : h_rm_sectorAvg)
  {
    se->registerHisto(this, ih);
  }

  for (auto& ieta : h_rm_tower)
  {
    for (auto& iphi : ieta)
    {
      se->registerHisto(this, iphi);
    }
  }

  Reset();

  // initialize waveform extraction tool
  WaveformProcessing = new CaloWaveformFitting();

  std::string hcaltemplate;
  if (getenv("HCALCALIB"))
  {
    hcaltemplate = getenv("HCALCALIB");
  }
  else
  {
    hcaltemplate = ".";
  }
  hcaltemplate += std::string("/testbeam_ohcal_template.root");
  // WaveformProcessing->initialize_processing(hcaltemplate);

  if (anaGL1)
  {
    erc = new eventReceiverClient("gl1daq");
  }

  return 0;
}

std::vector<float> HcalMon::getSignal(Packet* p, const int channel)
{
  double baseline = 0;
  for (int s = 0; s < 3; s++)
  {
    baseline += p->iValue(s, channel);
  }
  baseline /= 3.;

  double signal = 0;
  int sample = 0;
  for (int s = 3; s < p->iValue(0, "SAMPLES"); s++)
  {
    if (signal > p->iValue(s, channel))
    {
      signal = p->iValue(s, channel);
      sample = s;
    }
  }
  signal -= baseline;

  std::vector<float> result;
  result.push_back(signal);
  result.push_back(sample);
  result.push_back(baseline);

  return result;
}

std::vector<float> HcalMon::anaWaveform(Packet* p, const int channel)
{
  std::vector<float> waveform;
  // waveform.reserve(p->iValue(0, "SAMPLES"));
  float supppressed = 1;
  if (p->iValue(channel, "SUPPRESSED"))
  {
    waveform.push_back(p->iValue(channel, "PRE"));
    waveform.push_back(p->iValue(channel, "POST"));
  }
  else
  {
    supppressed = 0;
    for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
    {
      waveform.push_back(p->iValue(s, channel));
    }
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_ohcal;
  // fitresults_ohcal = WaveformProcessing->process_waveform(multiple_wfs);
  fitresults_ohcal = WaveformProcessing->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_ohcal.at(0);
  result.push_back(supppressed);

  return result;
}

int HcalMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  std::vector<runningMean*>::iterator rm_it;
  for (rm_it = rm_vector_sectAvg.begin(); rm_it != rm_vector_sectAvg.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  for (rm_it = rm_vector_twr.begin(); rm_it != rm_vector_twr.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
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
  for (rm_it = rm_vector_twrTime.begin(); rm_it != rm_vector_twrTime.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  for (rm_it = rm_vector_twrhit.begin(); rm_it != rm_vector_twrhit.end(); ++rm_it)
  {
    (*rm_it)->Reset();
  }
  return 0;
}

int HcalMon::process_event(Event* e /* evt */)
{
  if (e->getEvtType() >= 8)  /// special event where we do not read out the calorimeters
  {
    return 0;
  }
  evtcnt++;
  h_waveform_twrAvg->Reset();  // only record the latest event waveform
  h1_packet_event->Reset();
  unsigned int towerNumber = 0;
  float sectorAvg[Nsector] = {0};
  int npacket1 = 0;
  int npacket2 = 0;
  float energy1 = 0;
  float energy2 = 0;

 
  bool fillhist = true;
  std::vector<bool> trig_bools;
  trig_bools.resize(64);
  long long int gl1_clock = 0;
  bool have_gl1 = false;
  if (anaGL1)
  {
    int evtnr = e->getEvtSequence();
    Event* gl1Event = erc->getEvent(evtnr + 1);
    if (gl1Event)
    {
      have_gl1 = true;
      Packet* p = gl1Event->getPacket(14001);
      h_evtRec->Fill(0.0, 1.0);
      if (p)
      {
        gl1_clock = p->lValue(0, "BCO");
        uint64_t triggervec = p->lValue(0, "TriggerVector");
        for (int i = 0; i < 64; i++)
        {
          bool trig_decision = ((triggervec & 0x1U) == 0x1U);
          trig_bools[i] = trig_decision;

          if (trig_decision)
          {
            h_hcal_trig->Fill(i);
          }
          triggervec = (triggervec >> 1U) & 0xffffffffU;
        }

        delete p;
      }
      delete gl1Event;
    }
    else
    {
      std::cout << "GL1 event is null" << std::endl;
      h_evtRec->Fill(0.0, 0.0);
    }
    // this is for only process event with the MBD>=1 trigger
    if (usembdtrig)
    {
      if (trig_bools.at(10) == 0)
      {
        fillhist = false;
      }
    }
  }

  for (int packet = packetlow; packet <= packethigh; packet++)
  {
    Packet* p = e->getPacket(packet);
    int zero[1] = {0};
    int one[1] = {1};
    int packet_bin = packet - packetlow + 1;
    if (p)
    {
      rm_packet_number[packet - packetlow]->Add(one);
      int packet_length[1] = {p->getLength()};
      rm_packet_length[packet - packetlow]->Add(packet_length);

      h1_packet_length->SetBinContent(packet_bin, rm_packet_length[packet - packetlow]->getMean(0));

      h1_packet_event->SetBinContent(packet - packetlow + 1, p->lValue(0, "CLOCK"));
      if (have_gl1)
      {
        long long int p_clock = p->lValue(0, "CLOCK");
        long long int diff = (p_clock - gl1_clock) % 65536;
        h_caloPack_gl1_clock_diff->Fill(packet, diff);
      }
      int nChannels = p->iValue(0, "CHANNELS");
      if (nChannels > m_nChannels)
      {
        return -1;  // packet is corrupted, reports too many channels
      }
      else
      {
        npacket1++;
        rm_packet_chans[packet - packetlow]->Add(&nChannels);
        h1_packet_chans->SetBinContent(packet_bin, rm_packet_chans[packet - packetlow]->getMean(0));
      }
      for (int c = 0; c < nChannels; c++)
      {
        towerNumber++;

        // std::vector result =  getSignal(p,c); // simple peak extraction
        std::vector<float> result = anaWaveform(p, c);  // full waveform fitting
        float signal = result.at(0);
        float time = result.at(1);
        float pedestal = result.at(2);
        float suppressed = result.at(result.size() - 1);
        if (signal > 15 && signal < 15000)
        {
          energy1 += signal;
        }

        // channel mapping
        unsigned int key = TowerInfoDefs::encode_hcal(towerNumber - 1);
        unsigned int phi_bin = TowerInfoDefs::getCaloTowerPhiBin(key);
        unsigned int eta_bin = TowerInfoDefs::getCaloTowerEtaBin(key);
        int sectorNumber = phi_bin / 2 + 1;
        //________________________________for this part we only want to deal with the MBD>=1 trigger
        if (fillhist)
        {
          if (signal > hit_threshold)
          {
            rm_vector_twrTime[towerNumber - 1]->Add(&time);
            rm_vector_twrhit[towerNumber - 1]->Add(one);
          }
          else
          {
            rm_vector_twrhit[towerNumber - 1]->Add(zero);
          }
          h_waveform_pedestal->Fill(pedestal);

          if (suppressed == 1)
          {
            pr_zsFrac_etaphi->Fill(eta_bin, phi_bin, 0);
          }
          else
          {
            pr_zsFrac_etaphi->Fill(eta_bin, phi_bin, 1);
          }

          sectorAvg[sectorNumber - 1] += signal;

          rm_vector_twr[towerNumber - 1]->Add(&signal);

          int bin = h2_hcal_mean->FindBin(eta_bin + 0.5, phi_bin + 0.5);
          h2_hcal_mean->SetBinContent(bin, h2_hcal_mean->GetBinContent(bin) + signal);
          h2_hcal_rm->SetBinContent(bin, rm_vector_twrhit[towerNumber - 1]->getMean(0));
          h2_hcal_time->SetBinContent(bin, rm_vector_twrTime[towerNumber - 1]->getMean(0));

          // fill tower_rm here
          if (evtcnt <= historyLength * historyScaleDown)
          {
            // only fill every scaledown event
            if (evtcnt % historyScaleDown == 0)
            {
              h_rm_tower[eta_bin][phi_bin]->SetBinContent(evtcnt / historyScaleDown, rm_vector_twrhit[towerNumber - 1]->getMean(0));
            }
          }
          else
          {
            // only fill every scaledown event
            if (evtcnt % historyScaleDown == 0)
            {
              for (int ib = 1; ib < historyLength; ib++)
              {
                h_rm_tower[eta_bin][phi_bin]->SetBinContent(ib, h_rm_tower[eta_bin][phi_bin]->GetBinContent(ib + 1));
              }
              h_rm_tower[eta_bin][phi_bin]->SetBinContent(historyLength, rm_vector_twrhit[towerNumber - 1]->getMean(0));
            }
          }
        }
        //_______________________________________________________end of MBD trigger requirement
        // record waveform
        for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
        {
          h_waveform_twrAvg->Fill(s, p->iValue(s, c));
          if (signal > waveform_hit_threshold)
          {
            h2_hcal_waveform->Fill(s, (p->iValue(s, c) - pedestal));
          }
        }
        if (signal > waveform_hit_threshold)
        {
          h_waveform_time->Fill(time);
        }

        if (signal > hit_threshold)
        {
          h2_hcal_hits->Fill(eta_bin + 0.5, phi_bin + 0.5);
          for (int itrig = 0; itrig < 64; itrig++)
          {
            if (trig_bools[itrig])
            {
              h2_hcal_hits_trig[itrig]->Fill(eta_bin + 0.5, phi_bin + 0.5);
            }
          }
        }

      }  // channel loop

    }  // if packet good
    else
    {
      towerNumber += 192;
      rm_packet_number[packet - packetlow]->Add(zero);
    }
    h1_packet_number->SetBinContent(packet_bin, rm_packet_number[packet - packetlow]->getMean(0));
    delete p;
  }  // packet loop
  // if packetlow == 8001, then packetlowdiff = 7001, if packetlow == 7001, then packetlowdiff = 8001
  int packetlowdiff = 15002 - packetlow;
  int packethighdiff = 15016 - packethigh;

  if (npacket1 == 4)
  {
    for (int i = packetlowdiff; i <= packethighdiff; i++)
    {
      Packet* p = e->getPacket(i);
      if (p)
      {
        int nChannels = p->iValue(0, "CHANNELS");
        if (nChannels > m_nChannels)
        {
          return -1;  // packet is corrupted, reports too many channels
        }
        else
        {
          npacket2++;
        }
        for (int c = 0; c < nChannels; c++)
        {
          // std::vector result =  getSignal(p,c); // simple peak extraction
          std::vector<float> result = anaWaveform(p, c);  // full waveform fitting
          float signal = result.at(0);
          if (signal > 15 && signal < 15000)
          {
            energy2 += signal;
          }
        }
      }
      delete p;
    }
  }
  if (npacket1 == 4 && npacket2 == 4)
  {
    if (packetlow == 8001)
    {
      h2_hcal_correlation->Fill(energy1, energy2);
    }
    else
    {
      h2_hcal_correlation->Fill(energy2, energy1);
    }
  }
  // sector loop
  for (int isec = 0; isec < Nsector; isec++)
  {
    sectorAvg[isec] /= 48;
    h_sectorAvg_total->Fill(isec + 1, sectorAvg[isec]);
    rm_vector_sectAvg[isec]->Add(&sectorAvg[isec]);
    if (evtcnt <= historyLength * historyScaleDown)
    {
      // only fill every scaledown event
      if (evtcnt % historyScaleDown == 0)
      {
        h_rm_sectorAvg[isec]->SetBinContent(evtcnt / historyScaleDown, rm_vector_sectAvg[isec]->getMean(0));
      }
    }
    else
    {
      // only fill every scaledown event
      if (evtcnt % historyScaleDown == 0)
      {
        for (int ib = 1; ib < historyLength; ib++)
        {
          h_rm_sectorAvg[isec]->SetBinContent(ib, h_rm_sectorAvg[isec]->GetBinContent(ib + 1));
        }
        h_rm_sectorAvg[isec]->SetBinContent(historyLength, rm_vector_sectAvg[isec]->getMean(0));
      }
    }

  }  // sector loop

  if (fillhist) h_event->Fill(0);
  h_waveform_twrAvg->Scale(1. / 32. / 48.);  // average tower waveform

  return 0;
}

int HcalMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

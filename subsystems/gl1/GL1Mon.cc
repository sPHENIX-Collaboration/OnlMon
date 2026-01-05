#include "GL1Mon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonServer.h>
#include <onlmon/triggerEnum.h>

#include <Event/Event.h>
#include <Event/eventReceiverClient.h>
#include <Event/packet.h>

#include <TH1.h>

#include <bitset>
#include <iostream>
#include <string>  // for allocator, string, char_traits

GL1Mon::GL1Mon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new GL1Mon()
  // triggernamemap[22] = "22";
  // triggernamemap[23] = "23";
  // triggernamemap[]
  // RareProbeTriggers
  for (const auto &miter : TriggerEnum::RareProbeTriggers)
  {
    // convert int to "int"
    triggernamemap[static_cast<int>(miter.first)] = std::to_string(static_cast<int>(miter.first));
  }
  return;
}

GL1Mon::~GL1Mon()
{
  delete erc;
  return;
}

int GL1Mon::Init()
{
  int ihist = 0;
  OnlMonServer *se = OnlMonServer::instance();
  gl1_stats = new TH1I("gl1_stats", "GL1 statistics", 1, -0.5, 0.5);
  se->registerHisto(this, gl1_stats);
  for (auto &iter : scaledtriggers)
  {
    std::string name = "gl1_scaledtrigger_" + std::to_string(ihist);
    std::string title = "scaled trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(), 130, -0.5, 129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }

  ihist = 0;
  for (auto &iter : livetriggers)
  {
    std::string name = "gl1_livetrigger_" + std::to_string(ihist);
    std::string title = "live trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(), 130, -0.5, 129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }

  ihist = 0;
  for (auto &iter : rawtriggers)
  {
    std::string name = "gl1_rawtrigger_" + std::to_string(ihist);
    std::string title = "raw trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(), 130, -0.5, 129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }
  erc = new eventReceiverClient(eventReceiverClientHost);
  gl1_reject.resize(triggernamemap.size());
  ntriggers.resize(triggernamemap.size(), 0);
  triggernumber.resize(triggernamemap.size());
  triggername.resize(triggernamemap.size());
  int icnt = 0;
  for (const auto &miter : triggernamemap)
  {
    triggernumber[icnt] = miter.first;
    triggername[icnt] = miter.second;
    std::string hname = "gl1_reject_" + std::to_string(icnt);
    gl1_reject[icnt] = new TH1F(hname.c_str(), miter.second.c_str(), 1000, 0, 1000);
    se->registerHisto(this, gl1_reject[icnt]);
    icnt++;
  }
  TimeToLastEvent[0] = new TH1F("gl1_timetolastevent0", "Time to previous Event enlarged", 131, -0.5, 130.5);
  TimeToLastEvent[1] = new TH1F("gl1_timetolastevent1", "Time to previous Event", 2001, 1, 10001);
  TimeToLastEvent[2] = new TH1F("gl1_timetolastevent2", "Time to 2nd Event", 2001, 1, 10001);
  TimeToLastEvent[3] = new TH1F("gl1_timetolastevent3", "Time to 3rd Event", 2001, 1, 10001);
  TimeToLastEvent[4] = new TH1F("gl1_timetolastevent4", "Time to 4th Event", 2001, 1, 10001);
  for (auto iter : TimeToLastEvent)
  {
    se->registerHisto(this, iter);
  }
  for (int i = 1; i <= 5; i++)
  {
    eventticdeque.push_back(std::make_pair(-1*i,0));
  }
  return 0;
}

int GL1Mon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  if (erc->getStatus() != 0)
  {
    delete erc;
    erc = new eventReceiverClient(eventReceiverClientHost);
  }
  OnlMonServer *se = OnlMonServer::instance();
  se->UseGl1();
  lastupdate = se->CurrentTicks();
  starttime = lastupdate;
  return 0;
}

int GL1Mon::process_event(Event *evt)
{
  if (evt->getEvtType() == 1)
  {
    Packet *p = evt->getPacket(14001);
    if (p)
    {
      OnlMonServer *se = OnlMonServer::instance();
      int bunchnr = (p->lValue(0, "BunchNumber"));
      uint64_t trigscaled = static_cast<uint64_t>(p->lValue(0, "ScaledVector"));
      uint64_t triglive = static_cast<uint64_t>(p->lValue(0, "LiveVector"));
      uint64_t trigraw = static_cast<uint64_t>(p->lValue(0, "RawVector"));
      // triglive |= 0x1;
      // trigscaled |= 0x1;
      //  if ((triglive & trigscaled) != trigscaled) // this fails for the clock trigger
      //  {
      //    std::cout << "scaled trig vector: " << std::bitset<64>(trigscaled) << std::endl;
      //    std::cout << "live trig vector:   " << std::bitset<64>(triglive) << std::endl << std::endl;
      //  }
      //  std::cout << "scaled trig vector: " << std::bitset<64>(trigscaled) << std::endl;
      //  std::cout << "live trig vector:   " << std::bitset<64>(triglive) << std::endl << std::endl;
      //  std::cout << "raw trig vector: " << std::bitset<64>(trigraw) << std::endl;
      for (int itrig = 0; itrig < 64; itrig++)
      {
        uint64_t trigbit = 0x1UL << itrig;
        // fill with bunchnr+1, so the 0th bunch goes into the first channel (channel 0 is underflow)
        if ((trigscaled & trigbit) != 0)
        {
          scaledtriggers[itrig]->AddBinContent(bunchnr + 1);
        }
        if ((triglive & trigbit) != 0)
        {
          livetriggers[itrig]->AddBinContent(bunchnr + 1);
        }
        if ((trigraw & trigbit) != 0)
        {
          rawtriggers[itrig]->AddBinContent(bunchnr + 1);
        }
      }
      int eventnumber = evt->getEvtSequence();
      uint64_t bco = static_cast<uint64_t>(p->lValue(0, "BCO"));
      eventticdeque.push_back(std::make_pair(eventnumber,bco));
      std::pair<int, uint64_t> eventtic = eventticdeque.front();
      eventticdeque.pop_front();
      int event5th = eventtic.first;
      if (event5th > 0) // this will discard the first 5 events
      {
        for (int i =0; i < 5; i++)
        {
	  if (eventticdeque[i].first == event5th+1+i)
	  {
	    TimeToLastEvent[i]->Fill(eventticdeque[i].second - eventtic.second);
	  }
	}
      }

      Event *gl1Event{nullptr};
      if (erc->getStatus() == 0)
      {
        gl1Event = erc->getEvent(eventnumber);
      }
      if (gl1Event)
      {
        se->IncrementGl1FoundCounter();
        if (gl1Event->getEvtSequence() != eventnumber)
        {
          std::cout << "event number mismatch, asked for " << eventnumber
                    << ", got " << gl1Event->getEvtSequence() << std::endl;
        }
        Packet *p_gl1 = gl1Event->getPacket(14001);
        if (p_gl1)
        {
          if (p_gl1->lValue(0, "BCO") != p->lValue(0, "BCO"))
          {
            std::cout << "BCO mismatch for event " << eventnumber
                      << std::endl;
          }
          if (p_gl1->iValue(0) != p->iValue(0))
          {
            std::cout << "Packet number mismatch for event " << eventnumber
                      << "erc: " << p_gl1->iValue(0) << " current event: "
                      << p->iValue(0) << std::endl;
            gl1_stats->AddBinContent(1);
          }
        }
        delete p_gl1;
      }
      delete gl1Event;
      // rejection factors

      static constexpr time_t mintimediff{60};  //*60}; // every 5 minutes
      time_t ticks = se->CurrentTicks();
      if (ticks - lastupdate > mintimediff)
      {
        //      std::cout << "ticks: " << ticks << ", last: " << lastupdate << std::endl;
        int64_t current_mbtrigs = p->lValue(TriggerEnum::BitCodes::MBD_NS1_ZVRTX150, "TRIGGERRAW") - n_minbias;

        for (size_t i = 0; i < triggernumber.size(); i++)
        {
          int64_t curscale = p->lValue(triggernumber[i], "TRIGGERRAW") - ntriggers[i];
          // std::cout << "mb trigs: " << current_mbtrigs << " " <<  triggername[i]
          // 	    << ": " << curscale << std::endl;
          float rejection = (current_mbtrigs * 1.) / (curscale * 1.);
          ntriggers[i] += curscale;
          if (std::isfinite(rejection))
          {
            int nEntries = gl1_reject[i]->GetEntries();
            gl1_reject[i]->SetBinContent(nEntries + 1, rejection);
            gl1_reject[i]->SetBinError(nEntries + 1, ticks - starttime);
          }
          // std::cout << "setting rejection to " << rejection << " for trigger " << triggername[i] << std::endl;
        }
        n_minbias += current_mbtrigs;
        lastupdate = ticks;
      }
      // 	std::cout << "minbias count: " << p->lValue(12,"TRIGGERSCALED") << std::endl;
      // std::cout << "photon10: " << p->lValue(22,"TRIGGERSCALED") << std::endl;
      // std::cout << "photon12: " << p->lValue(23,"TRIGGERSCALED") << std::endl;
      // float rejection10 =
      delete p;
    }
  }
  return 0;
}

int GL1Mon::Reset()
{
  // reset our internal counters
  n_minbias = 0;
  std::fill(ntriggers.begin(), ntriggers.end(),0);
  return 0;
}

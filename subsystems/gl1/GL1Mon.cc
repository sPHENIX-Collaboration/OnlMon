#include "GL1Mon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
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
  return;
}

GL1Mon::~GL1Mon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int GL1Mon::Init()
{
  int ihist = 0;
  OnlMonServer *se = OnlMonServer::instance();
  for (auto &iter : scaledtriggers)
  {
    std::string name = "gl1_scaledtrigger_" + std::to_string(ihist);
    std::string title = "scaled trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(),130,-0.5,129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }

  ihist = 0;
  for (auto &iter : livetriggers)
  {
    std::string name = "gl1_livetrigger_" + std::to_string(ihist);
    std::string title = "live trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(),130,-0.5,129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }

  ihist = 0;
  for (auto &iter : rawtriggers)
  {
    std::string name = "gl1_rawtrigger_" + std::to_string(ihist);
    std::string title = "raw trigger bit " + std::to_string(ihist);
    iter = new TH1I(name.c_str(), title.c_str(),130,-0.5,129.5);
    se->registerHisto(this, iter);  // uses the TH1->GetName() as key
    ihist++;
  }
  // register histograms with server otherwise client won't get them
  return 0;
}

int GL1Mon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int GL1Mon::process_event(Event *evt)
{
  if (evt->getEvtType() == 1)
  {
    Packet *p = evt->getPacket(14001);
    if (p)
    {
      int bunchnr = (p->lValue(0, "BunchNumber"));
      uint64_t trigscaled = static_cast<uint64_t>( p->lValue(0,"ScaledVector") );	     
      uint64_t triglive = static_cast<uint64_t>( p->lValue(0,"LiveVector") );	     
      uint64_t trigraw = static_cast<uint64_t>( p->lValue(0,"RawVector") );	     
      // std::cout << "scaled trig vector: " << std::bitset<64>(trigscaled) << std::endl;
      // std::cout << "live trig vector: " << std::bitset<64>(triglive) << std::endl;
      // std::cout << "raw trig vector: " << std::bitset<64>(trigraw) << std::endl;
      for (int itrig = 0; itrig < 64; itrig++ )
      {
	uint64_t trigbit = 0x1UL << itrig;
	if ( (trigscaled&trigbit) != 0 )
	{
//	  std::cout << " setting trig " << itrig << std::endl;
	  scaledtriggers[itrig]->AddBinContent(bunchnr);
	}
	if ( (triglive&trigbit) != 0 )
	{
	  livetriggers[itrig]->AddBinContent(bunchnr);
	}
	if ( (trigraw&trigbit) != 0 )
	{
	  rawtriggers[itrig]->AddBinContent(bunchnr);
	}
      }
      delete p;
    }
  }
  return 0;
}

int GL1Mon::Reset()
{
  // reset our internal counters
  return 0;
}

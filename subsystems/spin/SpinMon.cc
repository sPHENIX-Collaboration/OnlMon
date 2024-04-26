// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "SpinMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

// #include <XingShiftCal.h>

#include <Event/msg_profile.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>
#include <Event/eventReceiverClient.h>


#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>

#include <cmath>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

SpinMon::SpinMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new SpinMon()
  return;
}

SpinMon::~SpinMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int SpinMon::Init()
{

  spin_patternBlueUp = new TH2I("h2_spinpatternBlueUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueDown = new TH2I("h2_spinpatternBlueDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueUnpol = new TH2I("h2_spinpatternBlueUnpol","",120,-0.5,119.5,2,0.5,2.5);

  spin_patternYellowUp = new TH2I("h2_spinpatternYellowUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowDown = new TH2I("h2_spinpatternYellowDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowUnpol = new TH2I("h2_spinpatternYellowUnpol","",120,-0.5,119.5,2,0.5,2.5);

  for (int i = 0; i < NTRIG; i++)
  {
    gl1_counter[i] = new TH1I(Form("gl1_counter_trig%d",i),Form("gl1_counter_trig%d",i),120,-0.5,119.5);
  }


  OnlMonServer *se = OnlMonServer::instance();
  se->registerHisto(this, spin_patternBlueUp);
  se->registerHisto(this, spin_patternBlueDown);
  se->registerHisto(this, spin_patternBlueUnpol);
  se->registerHisto(this, spin_patternYellowUp);
  se->registerHisto(this, spin_patternYellowDown);
  se->registerHisto(this, spin_patternYellowUnpol);

  for (int i = 0; i < NTRIG; i++)
  {
    se->registerHisto(this, gl1_counter[i]);
  }

  Reset();

//  erc = new eventReceiverClient("gl1daq"); // commented out to be able to do prun(100) in the server
  
  // CalculateCrossingShift(xingshift, scalercounts, success);

  return 0;
} 

int SpinMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int SpinMon::process_event(Event *e /* evt */)
{
  evtcnt++;
  //******* Spin patterns from Martin html delivery *******//
  //******* from BeginRun event *************
  
  if (e->getEvtType() == 9) //spin patterns stored in BeginRun event (9)
  {
    pBlueSpin = e->getPacket(packet_BLUESPIN);
    pYellSpin = e->getPacket(packet_YELLSPIN);
    for (int i = 0; i < NBUNCHES; i++)
    {
      if (i > 110) 
      {
        blueSpinPattern[i] = 0;
        yellSpinPattern[i] = 0;
      }
      else
      {
        blueSpinPattern[i] = pBlueSpin->iValue(i);
        yellSpinPattern[i] = pYellSpin->iValue(i);

        if (blueSpinPattern[i] == 1){spin_patternBlueUp->Fill(i,2);}
        if (blueSpinPattern[i] == -1){spin_patternBlueDown->Fill(i,2);}
        if (blueSpinPattern[i] == 0){spin_patternBlueUnpol->Fill(i,2);}

        if (yellSpinPattern[i] == 1){spin_patternYellowUp->Fill(i,2);}
        if (yellSpinPattern[i] == -1){spin_patternYellowDown->Fill(i,2);}
        if (yellSpinPattern[i] == 0){spin_patternYellowUnpol->Fill(i,2);}
      }

      // up = 1
      // down = -1
      // 0 for abort gap. bunches 111 -> 120

    }
    

    delete pBlueSpin;
    delete pYellSpin;
  }

  //******** gl1p scalers *********//

  //int evtnr = e->getEvtSequence();
  //Event *gl1Event = erc->getEvent(evtnr);
  //if (gl1Event){
    Packet* p = e->getPacket(packetid_GL1);
    if (p)
      {
      //int triggervec = p->lValue(0,"TriggerVector");
      int bunchnr = p->lValue(0,"BunchNumber");
      for (int i = 0; i < 16; i++) 
	{ 
	//2nd arg of lValue: 0 is raw trigger count, 1 is live trigger count, 2 is scaled trigger count
        int counts = p->lValue(i,1); //live gl1 cnts. (change to gl1p cnts. on bunchnr crossng when implemented) 
	//update instead of add
        gl1_counter[i]->SetBinContent(bunchnr+1,counts); //update bin with new scaler info. instead of adding every evt
	}
      }
  
    //}
  
  return 0;

}

int SpinMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}

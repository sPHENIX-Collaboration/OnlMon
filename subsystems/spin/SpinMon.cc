// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "SpinMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/eventReceiverClient.h>
#include <Event/msg_profile.h>

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
  hpCspinpatternBlue = new TH1I("h1_pCspinpatternBlue", "", 120, -0.5, 119.5);
  hpCspinpatternYellow = new TH1I("h1_pCspinpatternYellow", "", 120, -0.5, 119.5);

  hspinpatternBlue = new TH1I("h1_spinpatternBlue", "", 120, -0.5, 119.5);
  hspinpatternYellow = new TH1I("h1_spinpatternYellow", "", 120, -0.5, 119.5);

  pCspin_patternBlueUp = new TH2I("h2_pCspinpatternBlueUp", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  pCspin_patternBlueDown = new TH2I("h2_pCspinpatternBlueDown", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  pCspin_patternBlueUnpol = new TH2I("h2_pCspinpatternBlueUnpol", "", 120, -0.5, 119.5, 2, 0.5, 2.5);

  pCspin_patternYellowUp = new TH2I("h2_pCspinpatternYellowUp", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  pCspin_patternYellowDown = new TH2I("h2_pCspinpatternYellowDown", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  pCspin_patternYellowUnpol = new TH2I("h2_pCspinpatternYellowUnpol", "", 120, -0.5, 119.5, 2, 0.5, 2.5);

  spin_patternBlueUp = new TH2I("h2_spinpatternBlueUp", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  spin_patternBlueDown = new TH2I("h2_spinpatternBlueDown", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  spin_patternBlueUnpol = new TH2I("h2_spinpatternBlueUnpol", "", 120, -0.5, 119.5, 2, 0.5, 2.5);

  spin_patternYellowUp = new TH2I("h2_spinpatternYellowUp", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  spin_patternYellowDown = new TH2I("h2_spinpatternYellowDown", "", 120, -0.5, 119.5, 2, 0.5, 2.5);
  spin_patternYellowUnpol = new TH2I("h2_spinpatternYellowUnpol", "", 120, -0.5, 119.5, 2, 0.5, 2.5);

  hpolBlue = new TH1D("h1_polBlue", "", 1, 0, 1);
  hpolYellow = new TH1D("h1_polYellow", "", 1, 0, 1);

  hxingshift = new TH1I("h1_xingshift", "", 2, 0, 2);

  hfillnumber = new TH1I("h1_fillnumber", "", 2, 0, 2);
  hfilltypeBlue = new TH1I("h1_filltypeBlue", "", 1, 0, 1);
  hfilltypeYellow = new TH1I("h1_filltypeYellow", "", 1, 0, 1);

  hCorrect = new TH1F("hCorrect","Bunch Numbers", 130, -0.5, 129.5);
  hAbortgap = new TH1F("hAbortgap","Bunch Numbers", 130, -0.5, 129.5);
  hForbidden = new TH1F("hForbidden","Bunch Numbers", 130, -0.5, 129.5);

  for (int i = 0; i < NTRIG; i++)
  {
    gl1_counter[i] = new TH1I(Form("gl1_counter_trig%d", i), Form("gl1p_trig%d", i), 120, -0.5, 119.5);
  }

  for (auto &scalercount : scalercounts)
  {
    for (unsigned long &j : scalercount)
    {
      j = 0;
    }
  }

  OnlMonServer *se = OnlMonServer::instance();

  // se->registerHisto(this, hpCspinpatternBlue);
  // se->registerHisto(this, hpCspinpatternYellow);
  // se->registerHisto(this, pCspin_patternBlueUp);
  // se->registerHisto(this, pCspin_patternBlueDown);
  // se->registerHisto(this, pCspin_patternBlueUnpol);
  // se->registerHisto(this, pCspin_patternYellowUp);
  // se->registerHisto(this, pCspin_patternYellowDown);
  // se->registerHisto(this, pCspin_patternYellowUnpol);

  // se->registerHisto(this, hspinpatternBlue);
  // se->registerHisto(this, hspinpatternYellow);
  // se->registerHisto(this, spin_patternBlueUp);
  // se->registerHisto(this, spin_patternBlueDown);
  // se->registerHisto(this, spin_patternBlueUnpol);
  // se->registerHisto(this, spin_patternYellowUp);
  // se->registerHisto(this, spin_patternYellowDown);
  // se->registerHisto(this, spin_patternYellowUnpol);

  // se->registerHisto(this, hpolBlue);
  // se->registerHisto(this, hpolYellow);
  // se->registerHisto(this, hxingshift);
  // se->registerHisto(this, hfillnumber);
  // se->registerHisto(this, hfilltypeBlue);
  // se->registerHisto(this, hfilltypeYellow);

  se->registerHisto(this, hCorrect);
  se->registerHisto(this, hAbortgap);
  se->registerHisto(this, hForbidden);

  // for (auto &i : gl1_counter)
  // {
  //   se->registerHisto(this, i);
  // }

  Reset();

//  erc = new eventReceiverClient(eventReceiverClientHost);

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

  if (e->getEvtType() == 9)  // spin patterns stored in BeginRun
  {
    //================ BeginRunEvent packets ================//
    pBlueSpin = e->getPacket(packet_BLUESPIN);
    pYellSpin = e->getPacket(packet_YELLSPIN);

    pBluePol = e->getPacket(packet_BLUEPOL);
    pYellPol = e->getPacket(packet_YELLPOL);

    pBlueAsym = e->getPacket(packet_BLUEASYM);
    pYellAsym = e->getPacket(packet_YELLASYM);

    pBlueIntPattern = e->getPacket(packet_BLUEINTPATTERN);
    pYellIntPattern = e->getPacket(packet_YELLINTPATTERN);
    pBluePolPattern = e->getPacket(packet_BLUEPOLPATTERN);
    pYellPolPattern = e->getPacket(packet_YELLPOLPATTERN);

    pBlueFillNumber = e->getPacket(packet_BLUEFILLNUMBER);
    pYellFillNumber = e->getPacket(packet_YELLFILLNUMBER);
    //=======================================================//

    //========= Set beam polarization histograms ==============//
    float polBlue = -999;
    float polBlueErr = -999;
    if (pBluePol)
    {
      polBlue = pBluePol->iValue(0) / 10000.0;
      polBlueErr = pBluePol->iValue(1) / 10000.0;
      delete pBluePol;
    }
    hpolBlue->SetBinContent(1, polBlue);
    hpolBlue->SetBinError(1, polBlueErr);

    float polYellow = -999;
    float polYellowErr = -999;
    if (pYellPol)
    {
      polYellow = pYellPol->iValue(0) / 10000.0;
      polYellowErr = pYellPol->iValue(1) / 10000.0;
      delete pYellPol;
    }
    hpolYellow->SetBinContent(1, polYellow);
    hpolYellow->SetBinError(1, polYellowErr);
    //==========================================================//

    //============== Set fill number histogram ==============//
    float fillnumberBlue = 0;
    float fillnumberYellow = 0;
    if (pBlueFillNumber)
    {
      fillnumberBlue = pBlueFillNumber->iValue(0);
      delete pBlueFillNumber;
    }
    if (pYellFillNumber)
    {
      fillnumberYellow = pYellFillNumber->iValue(0);
      delete pYellFillNumber;
    }
    hfillnumber->SetBinContent(1, fillnumberBlue);
    hfillnumber->SetBinContent(2, fillnumberYellow);
    //=======================================================//

    //========== Set xingshift histogram =============//
    hxingshift->SetBinContent(1, defaultxingshift);
    //================================================//

    //============== Set spin pattern histograms ==============//
    /*
    //  up = 1, down = -1, abort gap (bunches 111-120) = 10
    if (pBlueSpin)
    {
    int numbluefilled = pBlueSpin->getDataLength();
    if (numbluefilled == 112){numbluefilled = 111;}
    hfilltypeBlue->SetBinContent(1,numbluefilled);
    int ispinBlue = 0;
    for (int i = 0; i < NBUNCHES; i++)
    {

    if (numbluefilled == 6 && blueFillPattern6[i] == 0)
    {
    hspinpatternBlue->SetBinContent(i+1,10);
    }
    else if (numbluefilled == 111 && blueFillPattern111[i] == 0)
    {
    hspinpatternBlue->SetBinContent(i+1,10);
    }
    else
    {
    blueSpinPattern[i] = pBlueSpin->iValue(ispinBlue);

    hspinpatternBlue->SetBinContent(i+1,blueSpinPattern[i]);

    if (blueSpinPattern[i] == 1){spin_patternBlueUp->Fill(i,1);}
    if (blueSpinPattern[i] == -1){spin_patternBlueDown->Fill(i,1);}
    if (blueSpinPattern[i] == 0){spin_patternBlueUnpol->Fill(i,1);}

    ispinBlue++;
    }
    }

    delete pBlueSpin;
    }

    if (pYellSpin){
    int numyellfilled = pYellSpin->getDataLength();
    if (numyellfilled == 112){numyellfilled = 111;}
    hfilltypeYellow->SetBinContent(1,numyellfilled);

    int ispinYell = 0;

    for (int i = 0; i < NBUNCHES; i++)
    {
    if (numyellfilled == 6 && yellFillPattern6[i] == 0)
    {
    hspinpatternYellow->SetBinContent(i+1,10);
    }
    else if (numyellfilled == 111 && yellFillPattern111[i] == 0)
    {
    hspinpatternYellow->SetBinContent(i+1,10);
    }
    else
    {
    yellSpinPattern[i] = pYellSpin->iValue(ispinYell);

    hspinpatternYellow->SetBinContent(i+1,yellSpinPattern[i]);

    if (yellSpinPattern[i] == 1){spin_patternYellowUp->Fill(i,2);}
    if (yellSpinPattern[i] == -1){spin_patternYellowDown->Fill(i,2);}
    if (yellSpinPattern[i] == 0){spin_patternYellowUnpol->Fill(i,2);}

    ispinYell++;
    }
    }

    delete pYellSpin;
    }
    */
    //==========================================================//

    //============== Set intended spin pattern histograms from buckets ==============//

    // Get bunch asymmetries for measured spin pattern
    // there are 360 buckets for 120 bunches
    if (pBlueIntPattern && pBluePolPattern)
    {
      int numbluefill = 0;
      for (int i = 0; i < 360; i += 3)
      {
	blueFillPattern[i / 3] = pBlueIntPattern->iValue(i);
        if (pBlueIntPattern->iValue(i))
        {
          blueSpinPattern[i / 3] = pBluePolPattern->iValue(i);
          numbluefill++;
        }
        else
        {
          blueSpinPattern[i / 3] = 10;
        }

        hspinpatternBlue->SetBinContent((i / 3) + 1, blueSpinPattern[i / 3]);
        if (blueSpinPattern[i / 3] == 1)
        {
          spin_patternBlueUp->Fill(i / 3, 1);
        }
        if (blueSpinPattern[i / 3] == -1)
        {
          spin_patternBlueDown->Fill(i / 3, 1);
        }
        if (blueSpinPattern[i / 3] == 0)
        {
          spin_patternBlueUnpol->Fill(i / 3, 1);
        }
      }

      hfilltypeBlue->SetBinContent(1, numbluefill);

      delete pBlueIntPattern;
      delete pBluePolPattern;
    }

    if (pYellIntPattern && pYellPolPattern)
    {
      int numyellfill = 0;
      for (int i = 0; i < 360; i += 3)
      {
	yellFillPattern[i / 3] = pYellIntPattern->iValue(i);
        if (pYellIntPattern->iValue(i))
        {
          yellSpinPattern[i / 3] = pYellPolPattern->iValue(i);
          numyellfill++;
        }
        else
        {
          yellSpinPattern[i / 3] = 10;
        }

        hspinpatternYellow->SetBinContent((i / 3) + 1, yellSpinPattern[i / 3]);

        if (yellSpinPattern[i / 3] == 1)
        {
          spin_patternYellowUp->Fill(i / 3, 2);
        }
        if (yellSpinPattern[i / 3] == -1)
        {
          spin_patternYellowDown->Fill(i / 3, 2);
        }
        if (yellSpinPattern[i / 3] == 0)
        {
          spin_patternYellowUnpol->Fill(i / 3, 2);
        }
      }

      hfilltypeYellow->SetBinContent(1, numyellfill);

      delete pYellIntPattern;
      delete pYellPolPattern;
    }

    //=======================================================================//

    //============== Set pC spin pattern histograms from buckets ==============//

    // Get bunch asymmetries for measured spin pattern
    // there are 360 buckets for 120 bunches
    if (pBlueAsym)
    {
      for (int i = 0; i < 360; i += 3)
      {
        float blueAsyms = pBlueAsym->iValue(i) / 10000.0;
        float blueAsymsErr = pBlueAsym->iValue(i + 360) / 10000.0;

        float bluebot = blueAsyms - blueAsymsErr;
        float bluetop = blueAsyms + blueAsymsErr;

        if (blueAsyms != 0 || bluebot != 0 || bluetop != 0)
        {
          if (bluebot > 0 && bluetop > 0)
          {
            hpCspinpatternBlue->SetBinContent((i / 3) + 1, 1);
            pCspin_patternBlueUp->Fill(i / 3, 1);
          }
          else if (bluebot < 0 && bluetop < 0)
          {
            hpCspinpatternBlue->SetBinContent((i / 3) + 1, -1);
            pCspin_patternBlueDown->Fill(i / 3, 1);
          }
          else if (bluebot <= 0 && bluetop >= 0)
          {
            hpCspinpatternBlue->SetBinContent((i / 3) + 1, 0);
            // pCspin_patternBlueUnpol->Fill(i / 3, 1);
          }
        }
        else
        {
          hpCspinpatternBlue->SetBinContent((i / 3) + 1, 10);
        }
      }
      delete pBlueAsym;
    }

    if (pYellAsym)
    {
      for (int i = 0; i < 360; i += 3)
      {
        float yellAsyms = pYellAsym->iValue(i) / 10000.0;
        float yellAsymsErr = pYellAsym->iValue(i + 360) / 10000.0;

        float yellbot = yellAsyms - yellAsymsErr;
        float yelltop = yellAsyms + yellAsymsErr;

        if (yellAsyms != 0 || yellbot != 0 || yelltop != 0)
        {
          if (yellbot > 0 && yelltop > 0)
          {
            hpCspinpatternYellow->SetBinContent((i / 3) + 1, 1);
            pCspin_patternYellowUp->Fill(i / 3, 2);
          }
          else if (yellbot < 0 && yelltop < 0)
          {
            hpCspinpatternYellow->SetBinContent((i / 3) + 1, -1);
            pCspin_patternYellowDown->Fill(i / 3, 2);
          }
          else if (yellbot <= 0 && yelltop >= 0)
          {
            hpCspinpatternYellow->SetBinContent((i / 3) + 1, 0);
            // pCspin_patternYellowUnpol->Fill(i / 3, 2);
          }
        }
        else
        {
          hpCspinpatternYellow->SetBinContent((i / 3) + 1, 10);
        }
      }
      delete pYellAsym;
    }
    //=======================================================================//
  }

  else if (e->getEvtType() == 1)
  {
    //=============== gl1p scalers ===============//
    //    int evtnr = e->getEvtSequence();
    p_gl1 = e->getPacket(packetid_GL1);
    // p_gl1 = e->getPacket(packetid_GL1);
    if (p_gl1)
    {
      // int triggervec = p->lValue(0,"TriggerVector");
      int bunchnr = (p_gl1->lValue(0, "BunchNumber") + defaultxingshift) % NBUNCHES;
      if ( bunchnr <= 110)
      {
	hCorrect->Fill(bunchnr);
      }
      else if ( bunchnr >110 && bunchnr <120)
      {
	hAbortgap->Fill(bunchnr);
      }
      else
      {
	hForbidden->Fill(125);
      }

      for (int i = 0; i < 16; i++)
      {
        // 2nd arg of lValue: 0 is raw trigger count, 1 is live trigger count, 2 is scaled trigger count
        int counts = p_gl1->lValue(i, "GL1PLIVE");  // live gl1p cnts.
        // update instead of add
        gl1_counter[i]->SetBinContent(bunchnr + 1, counts);  // update bin with new scaler info. instead of adding every evt
        scalercounts[i][bunchnr] = counts;
      }
      delete p_gl1;
    }

    //========================//

    if (evtcnt > 4999 && evtcnt % 5000 == 0)
    {
      CalculateCrossingShift(xingshift, scalercounts, success);

      if (success)
      {
        addxingshift = xingshift;
      }
    }
    hxingshift->SetBinContent(2, addxingshift);
  }

  return 0;
}

int SpinMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}

int SpinMon::CalculateCrossingShift(int &xing, uint64_t counts[NTRIG][NBUNCHES], bool &succ)
{
  succ = false;
  int shift_array[NTRIG] = {0};

  int trig_inactive_array[NTRIG] = {0};

  int last_active_index = 0;

  int _temp;
  for (int itrig = 0; itrig < NTRIG; itrig++)
  {
    long long _counts = 0;
    for (int ii = 0; ii < NBUNCHES; ii++)
    {
      _counts += counts[itrig][ii];
    }

    if (_counts < 10000)
    {
      trig_inactive_array[itrig] = 1;
    }
    else
    {
      last_active_index = itrig;
    }

    long long abort_sum_prev = _counts;

    _temp = 0;
    for (int ishift = 0; ishift < NBUNCHES; ishift++)
    {
      long long abort_sum = 0;
      for (int iunfillbunch = 0; iunfillbunch < NBUNCHES; iunfillbunch++)
      {
	if (blueFillPattern[iunfillbunch] && yellFillPattern[iunfillbunch])
	{
	  continue;
	}
	int shiftbunch = iunfillbunch - ishift;
	if (shiftbunch < 0)
	{
	  shiftbunch = 120 + shiftbunch;
	}
        abort_sum += counts[itrig][(shiftbunch) % NBUNCHES];
      }
      if (abort_sum < abort_sum_prev)
      {
        abort_sum_prev = abort_sum;
        _temp = ishift;
      }
    }

    shift_array[itrig] = _temp;
  }

  for (int itrig = 0; itrig < NTRIG; itrig++)
  {
    // if not matching for all trigger selections used, fails
    if (!trig_inactive_array[itrig])
    {
      if (shift_array[itrig] == shift_array[last_active_index])
      {
        xing = shift_array[itrig];
        succ = true;
      }
      else
      {
        xing = 0;
        succ = false;
        return 0;
      }
    }
  }

  // succ = true;
  return 0;
}

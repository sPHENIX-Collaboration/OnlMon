// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "ZdcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>
#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
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

ZdcMon::ZdcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new ZdcMon()
  return;
}

ZdcMon::~ZdcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int ZdcMon::Init()
{
  const float MAX_ENERGY1 = 1500.;
  const float MAX_ENERGY2 = 1500.;
  const float MIN_ENERGY1 = 0.;
  const float MIN_ENERGY2 = 0.;
  const int BIN_NUMBER = 150;
  const int SMD_ADC_BIN = 250;

    
    
  //  gRandom->SetSeed(rand());
  // read our calibrations from ZdcMonData.dat
  const char *zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(zdccalib) + "/" + "ZdcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();

  //getting gains
  float col1, col2, col3;
  std::string gainfile = std::string(zdccalib) + "/" + "/ZdcCalib.pmtgain";
  std::ifstream gain_infile(gainfile);
  std::ostringstream msg(gainfile);

  if (!gain_infile)
  {
    msg << gainfile << " could not be opened.";
    OnlMonServer *se = OnlMonServer::instance();
    se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
    exit(1);
  }

  for (float &i : gain)
  {
    gain_infile >> col1 >> col2 >> col3;
    i = col1;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
  {
    smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
  {
    smd_north_rgain[i] = gain[i + 16];  // 0-7: y channels, 8-14: x channels, 15: analog sum
  }

  gain_infile.close();

  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  // Create hitograms
  zdc_adc_north = new TH1F("zdc_adc_north", "ZDC ADC north", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_adc_south = new TH1F("zdc_adc_south", "ZDC ADC south", BIN_NUMBER, MIN_ENERGY2, MAX_ENERGY2);

  zdc_N1 = new TH1F("zdc_N1", "ZDC1 ADC north", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_N2 = new TH1F("zdc_N2", "ZDC2 ADC north", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_N3 = new TH1F("zdc_N3", "ZDC3 ADC north", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_S1 = new TH1F("zdc_S1", "ZDC1 ADC south", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_S2 = new TH1F("zdc_S2", "ZDC2 ADC south", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  zdc_S3 = new TH1F("zdc_S3", "ZDC3 ADC south", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);

  veto_NF = new TH1F("veto_NF", "veto north front", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  veto_NB = new TH1F("veto_NB", "veto north back", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  veto_SF = new TH1F("veto_SF", "veto south front", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  veto_SB = new TH1F("veto_SB", "veto south back", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
 
  //waveform
    
   h_waveform_timez = new TH1F("h_waveform_timez", "", 16, 0.5, 16 + 0.5);
   h_waveform_timess = new TH1F("h_waveform_timess", "", 16, 0.5, 16 + 0.5);
   h_waveform_timesn = new TH1F("h_waveform_timesn", "", 16, 0.5, 16 + 0.5);
   h_waveform_timevs = new TH1F("h_waveform_timevs", "", 16, 0.5, 16 + 0.5);
   h_waveform_timevn = new TH1F("h_waveform_timevn", "", 16, 0.5, 16 + 0.5);

  h_waveformZDC = new TH2F("h_waveformZDC", "h_waveformZDC", 31, 0.5, 31 + 0.5, 1000, 0, 15000);
  h_waveformSMD_North = new TH2F("h_waveformSMD_North", "h_waveformSMD_North", 31, 0.5, 31 + 0.5, 1000, 0, 15000);
  h_waveformSMD_South = new TH2F("h_waveformSMD_South", "h_waveformSMD_South", 31, 0.5, 31 + 0.5, 1000, 0, 15000);
  h_waveformVeto_North = new TH2F("h_waveformVeto_North", "h_waveformVeto_North", 31, 0.5, 31 + 0.5, 1000, 0, 15000);
  h_waveformVeto_South = new TH2F("h_waveformVeto_South", "h_waveformVeto_South", 31, 0.5, 31 + 0.5, 1000, 0, 15000);

      
  // SMD
  // Individual SMD_ADC Values
  // Horizontal (expert plot)
  for (int i = 0; i < 8; i++)
  {
    smd_adc_n_hor_ind[i] = new TH1I(Form("smd_adc_n_hor_ind%d", i), Form("smd_adc_n_hor_ind%d", i), SMD_ADC_BIN, 0, 5000);
    smd_adc_s_hor_ind[i] = new TH1I(Form("smd_adc_s_hor_ind%d", i), Form("smd_adc_s_hor_ind%d", i), SMD_ADC_BIN, 0, 5000);
  }
  // Vertical (expert plot)
  for (int i = 0; i < 7; i++)
  {
    smd_adc_n_ver_ind[i] = new TH1I(Form("smd_adc_n_ver_ind%d", i), Form("smd_adc_n_ver_ind%d", i), SMD_ADC_BIN, 0, 5000);
    smd_adc_s_ver_ind[i] = new TH1I(Form("smd_adc_s_ver_ind%d", i), Form("smd_adc_s_ver_ind%d", i), SMD_ADC_BIN, 0, 5000);
  }

  // SMD Hit Multiplicity
  smd_north_hor_hits = new TH1F("smd_north_hor_hits", "smd_north_hor_hits", 9, -0.5, 8.5);
  smd_north_ver_hits = new TH1F("smd_north_ver_hits", "smd_north_ver_hits", 8, -0.5, 7.5);
  smd_south_hor_hits = new TH1F("smd_south_hor_hits", "smd_south_hor_hits", 9, -0.5, 8.5);
  smd_south_ver_hits = new TH1F("smd_south_ver_hits", "smd_south_ver_hits", 8, -0.5, 7.5);

  // north smd
  smd_hor_north = new TH1F("smd_hor_north", "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_ver_north = new TH1F("smd_ver_north", "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  
  smd_sum_hor_north = new TH1F("smd_sum_hor_north", "SMD North y", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  smd_sum_ver_north = new TH1F("smd_sum_ver_north", "SMD North x", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  
  smd_hor_north_small = new TH1F("smd_hor_north_small", "Beam centroid distribution, SMD North y, zdc <= 200", 296, -5.92, 5.92);
  smd_ver_north_small = new TH1F("smd_ver_north_small", "Beam centroid distribution, SMD North x, zdc <= 200", 220, -5.5, 5.5);
  smd_hor_north_good = new TH1F("smd_hor_north_good", "Beam centroid distribution, SMD North y, zdc > 200", 296, -5.92, 5.92);
  smd_ver_north_good = new TH1F("smd_ver_north_good", "Beam centroid distribution, SMD North x, zdc > 200", 220, -5.5, 5.5);

  // south smd
  smd_hor_south = new TH1F("smd_hor_south", "Beam centroid distribution, SMD South y", 296, -5.92, 5.92);
  smd_ver_south = new TH1F("smd_ver_south", "Beam centroid distribution, SMD South x", 220, -5.5, 5.5);

  smd_sum_hor_south = new TH1F("smd_sum_hor_south", "SMD South y", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
  smd_sum_ver_south = new TH1F("smd_sum_ver_south", "SMD South x", BIN_NUMBER, MIN_ENERGY1, MAX_ENERGY1);
 
  smd_hor_south_good = new TH1F("smd_hor_south_good", "Beam centroid distribution, SMD South y, zdc1 > 65 zdc2>20 and veto<200", 296, -5.92, 5.92);
  smd_ver_south_good = new TH1F("smd_ver_south_good", "Beam centroid distribution, SMD South x, zdc1 > 65 zdc2>20 and veto<200", 220, -5.5, 5.5);


  // smd values (expert plot)
  smd_value = new TH2F("smd_value", "SMD channel# vs value", 1024, 0, 4096, 33, -0.5, 32.5);
  smd_value_good = new TH2F("smd_value_good", "SMD channel# vs value, zdc > 200", 1024, 0, 4096, 33, -0.5, 32.5);
  smd_value_small = new TH2F("smd_value_small", "SMD channel# vs value, zdc <= 200", 1024, 0, 4096, 33, -0.5, 32.5);
  smd_xy_north = new TH2F("smd_xy_north", "SMD hit position north", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_south = new TH2F("smd_xy_south", "SMD hit position south", 110, -5.5, 5.5, 119, -5.92, 5.92);

  OnlMonServer *se = OnlMonServer::instance();

  //register histograms with server otherwise client won't get them
  //zdc
  se->registerHisto(this, zdc_adc_north);
  se->registerHisto(this, zdc_adc_south);
  se->registerHisto(this, zdc_N1);
  se->registerHisto(this, zdc_N2);
  se->registerHisto(this, zdc_N3);
  se->registerHisto(this, zdc_S1);
  se->registerHisto(this, zdc_S2);
  se->registerHisto(this, zdc_S3);
  se->registerHisto(this, h_waveformZDC);
  se->registerHisto(this, h_waveformSMD_North);
  se->registerHisto(this, h_waveformSMD_South);
  se->registerHisto(this, h_waveformVeto_North);
  se->registerHisto(this, h_waveformVeto_South);
    
  se->registerHisto(this, h_waveform_timez);
  se->registerHisto(this, h_waveform_timess);
  se->registerHisto(this, h_waveform_timesn);
  se->registerHisto(this, h_waveform_timevs);
  se->registerHisto(this, h_waveform_timevn);
 
    
  //veto
  se->registerHisto(this, veto_NF);
  se->registerHisto(this, veto_NB);
  se->registerHisto(this, veto_SF);
  se->registerHisto(this, veto_SB);
 

  // SMD
  // Individual smd_adc channel histos

  for (int i = 0; i < 8; i++)
  {
    se->registerHisto(this, smd_adc_n_hor_ind[i]);
    se->registerHisto(this, smd_adc_s_hor_ind[i]);
  }
  for (int i = 0; i < 7; i++)
  {
    se->registerHisto(this, smd_adc_n_ver_ind[i]);
    se->registerHisto(this, smd_adc_s_ver_ind[i]);
  }

  // SMD Hit Multiplicity
  se->registerHisto(this, smd_north_hor_hits);
  se->registerHisto(this, smd_north_ver_hits);
  se->registerHisto(this, smd_south_hor_hits);
  se->registerHisto(this, smd_south_ver_hits);
  

  // north SMD
  se->registerHisto(this, smd_hor_north);
  se->registerHisto(this, smd_ver_north);
  se->registerHisto(this, smd_sum_hor_north);
  se->registerHisto(this, smd_sum_ver_north);
  se->registerHisto(this, smd_hor_north_small);
  se->registerHisto(this, smd_ver_north_small);
  se->registerHisto(this, smd_hor_north_good);
  se->registerHisto(this, smd_ver_north_good);
  // south SMD
  se->registerHisto(this, smd_hor_south);
  se->registerHisto(this, smd_ver_south);
  se->registerHisto(this, smd_sum_hor_south);
  se->registerHisto(this, smd_sum_ver_south);
  se->registerHisto(this, smd_hor_south_good);
  se->registerHisto(this, smd_ver_south_good);
  // SMD values
  se->registerHisto(this, smd_value);
  se->registerHisto(this, smd_value_good);
  se->registerHisto(this, smd_value_small);
  se->registerHisto(this, smd_xy_north);
  se->registerHisto(this, smd_xy_south);

  WaveformProcessingFast = new CaloWaveformFitting();

  Reset();

  return 0;
}

int ZdcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  return 0;
}

std::vector<float> ZdcMon::anaWaveformFast(Packet *p, const int channel)
{
  std::vector<float> waveform;
  // Chris: preallocation = speed improvement
  waveform.reserve(p->iValue(0, "SAMPLES"));
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);
  return result;
}

int ZdcMon::process_event(Event *e /* evt */)
{
  evtcnt++;
    
  const float waveform_hit_threshold = 100;

  int packet = 12001;

  float totalzdcsouthsignal = 0.;
  float totalzdcnorthsignal = 0.;
  float smd_south_xsum = 0.;
  float smd_south_ysum = 0.;
  float smd_north_xsum = 0.;
  float smd_north_ysum = 0.;

  float veto_cut = 200.0;
  float ZDC1cut = 65.0;
  float ZDC2cut = 20.0;

  std::vector <float> z;
  z.clear();
    
  std::vector <float> sm;
  sm.clear();
    
  std::vector <float> tz;
  tz.clear();
    
  std::vector <float> tsmd;
  tsmd.clear();
    
  std::vector<float> resultFast;
  resultFast.clear();

  float zdctimelow  = 6.0;
  float zdctimehigh  = 9.0;
    
  float smdNtimelow  = 10.0;
  float smdNtimehigh  = 14.0;

  float smdStimelow  = 7.0;
  float smdStimehigh  = 12.0;
    
  float vetoStimelow  = 7.0;
  float vetoStimehigh  = 12.0;
    
  float vetoNtimelow  = 6.0;
  float vetoNtimehigh  = 9.0;
 
  Packet *p = e->getPacket(packet);
  if (p)
  {
        
    for (int c = 0; c < p->iValue(0, "CHANNELS"); c++)
    {
      resultFast = anaWaveformFast(p, c);  // fast waveform fitting
      float signalFast = resultFast.at(0);
      float time = resultFast.at(1);
      float pedestal = resultFast.at(2);
      float signal = signalFast;
        
        for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
        {
            if (c < 16)
            {
                if (signal > waveform_hit_threshold) h_waveformZDC->Fill(s, p->iValue(s, c) - pedestal);
            }
            
            if (c > 15 && c < 18)
            {
                if (signal > waveform_hit_threshold) h_waveformVeto_North->Fill(s, p->iValue(s, c) - pedestal);
            }
            
            if (c > 47 && c < 64)
            {
                if (signal > waveform_hit_threshold) h_waveformSMD_North->Fill(s, p->iValue(s, c) - pedestal);
            }
            
            if (c > 79 && c < 82)
            {
                if (signal > waveform_hit_threshold) h_waveformVeto_South->Fill(s, p->iValue(s, c) - pedestal);
            }
            
            if (c > 111)
            {
                if (signal > waveform_hit_threshold) h_waveformSMD_South->Fill(s, p->iValue(s, c) - pedestal);
            }
        }

     // fill zdc raw signal first
     if(c < 16)
     {
         if(signal > waveform_hit_threshold)
         {
           h_waveform_timez->Fill(time);
         }
 
       z.push_back(signal);
       tz.push_back(time);

      if((c == 0) || (c == 2) || (c == 4))
      {
         totalzdcsouthsignal += signal;
          
         if(c == 0) zdc_S1->Fill(signal);
         if(c == 2) zdc_S2->Fill(signal);
         if(c == 4) zdc_S3->Fill(signal);
      }

      else if((c==8) || (c ==10) || (c == 12))
      {
         totalzdcnorthsignal += signal;
          
         if(c == 8) zdc_N1->Fill(signal);
         if(c == 10) zdc_N2->Fill(signal);
         if(c == 12) zdc_N3->Fill(signal);
      }
     }

    // veto N
    else if(c > 15 && c < 18)
    {
        if(signal > waveform_hit_threshold)
        {
           h_waveform_timevn->Fill(time);
        }
        
        if(((time >= vetoNtimelow) && (time <= vetoNtimehigh)))
        {

            if(c == 16)
            {
                v[0] = signal; if(signal > 0.) veto_NF->Fill(signal);
            }
            
            if (c == 17)
            {
                v[1] = signal; if(signal > 0.) veto_NB->Fill(signal);
            }
        }
        
    }
        
     //smd N
     else if(c > 47 && c < 64)
     {
        sm.push_back(signal);
        tsmd.push_back(time);
         
         if(signal > waveform_hit_threshold)
         {
            h_waveform_timesn->Fill(time);
         }
     }
        
    // veto S
    else if(c > 79 && c < 82)
    {
        if(((time >= vetoStimelow) && (time <= vetoStimehigh)))
        {

            if(c == 80)
            {
                v[2] = signal; if(signal > 0.) veto_SF->Fill(signal);
            }
                
            if (c == 81)
            {
                v[3] = signal; if(signal > 0.) veto_SB->Fill(signal);
            }
        }
        
        if(signal > waveform_hit_threshold)
        {
            h_waveform_timevs->Fill(time);
        }
        
            
    }
        
          
    //smd S
     else if(c > 111)
     {
           sm.push_back(signal);
           tsmd.push_back(time);
         
         if(signal > waveform_hit_threshold)
         {
             h_waveform_timess->Fill(time);
         }
     }


  }// channel loop end

      
   zdc_adc_south->Fill(totalzdcsouthsignal);
   zdc_adc_north->Fill(totalzdcnorthsignal);

        
  int zsize = z.size();
  int ssize = sm.size();
      
   if(zsize != 16)
    {
      std::cout<< "zdc channel mapping error" << std::endl;
      if(tz.size() != 16)
      exit(1);
    }

   if(ssize != 32)
    {
      std::cout<< "smd channel mapping error" << std::endl;
      if(tsmd.size() != 32)
      exit(1);
    }


     for (int i = 0; i < zsize; i++) //zdc
     {
         if ((tz[i] >= zdctimelow) && (tz[i] <= zdctimehigh))
         {
             zdc_adc[i] = z[i];
         }
         else
         {
            zdc_adc[i] = 0.0;
         }
     }//zdc

     for (int j = 0; j < ssize; j++) //smd
      {
          if(j < 16) //smd north [0,15] --> [48,63]
          {
            if((tsmd[j] >= smdNtimelow) && (tsmd[j] <= smdNtimehigh))
              {
                  
                  smd_adc[j] = sm[j];
                  if(j<=7) smd_north_ysum += sm[j];
                  if(j >= 8 && j<=14) smd_north_xsum += sm[j]; //skip sum ch, 15->63
              }
              else
              {
                 smd_adc[j] = 0.0;
              }
           }
        
          if (j >= 16 && j <= 31) //smd south [16,31] --> [112,127]
          {
             if((tsmd[j] >= smdStimelow) && (tsmd[j] <= smdStimehigh ))
              {
                  smd_adc[j] = sm[j];
                  if(j >= 16 && j<=23) smd_south_ysum += sm[j];
                  if(j >= 24 && j<=30) smd_south_xsum += sm[j]; //skip sum ch, 31->127
              }
              else
              {
                  smd_adc[j] = 0.0;
              }
          }
      }//smd



  // call the functions
    CompSmdAdc();
    CompSmdPos();

    // BOOLEANS, INTs AND OTHER DEFINITIONS

    bool fill_hor_south = false;
    bool fill_ver_south = false;

    bool fill_hor_north = false;
    bool fill_ver_north = false;

    int s_ver = 0;
    int s_hor = 0;

    int n_ver = 0;
    int n_hor = 0;

    float zdc_adc_threshold = 200.;
    float smd_adc_threshold = 20.;
   
    // counters
    int smd_n_h_counter = 0;
    int smd_n_v_counter = 0;
    int smd_s_h_counter = 0;
    int smd_s_v_counter = 0;
    
      
    float hor_cut = 8.0;
    float ver_cut = 8.0;
    float ped_cut = 0.0; //suppress zeroes

    for (int i = 0; i < 8; i++)
    {

     if (smd_adc[i] > hor_cut)
      {
        n_hor++;
      }
      

     if (smd_adc[i] > ped_cut) smd_adc_n_hor_ind[i]->Fill(smd_adc[i]);

     smd_value->Fill(smd_adc[i], float(i));
   
     if (zdc_adc[8] > veto_cut)
      {
         smd_value_good->Fill(smd_adc[i], float(i));
      }
      else
      {
         smd_value_small->Fill(smd_adc[i], float(i));
      }

      if ((smd_adc[i] > smd_adc_threshold) && (zdc_adc[0] > zdc_adc_threshold))
      {
        smd_n_h_counter++;
      }

      if (smd_adc[i] > smd_adc_threshold)
      {
        smd_n_h_counter++;
      }

      if (smd_adc[i + 16] > hor_cut)
      {
        s_hor++;
      }
    
      if (smd_adc[i + 16] > ped_cut) smd_adc_s_hor_ind[i]->Fill(smd_adc[i + 16]);
      
      smd_value->Fill(smd_adc[i + 16], float(i) + 16);
     
      if (zdc_adc[0] > veto_cut)
      {
        smd_value_good->Fill(smd_adc[i + 16], float(i) + 16);
      }
      else
      {
        smd_value_small->Fill(smd_adc[i + 16], float(i) + 16);
      }


      if (smd_adc[i + 16] > smd_adc_threshold)
      {
        smd_s_h_counter++;
      }
        
    }

    for (int i = 0; i < 7; i++)
    {
      if (smd_adc[i + 8] > ver_cut)
      {
        n_ver++;
      }
     
  
      if(smd_adc[i + 8] > ped_cut) smd_adc_n_ver_ind[i]->Fill(smd_adc[i + 8]);

      smd_value->Fill(smd_adc[i + 8], float(i) + 8);
   
      if (zdc_adc[8] > veto_cut)
      {
        smd_value_good->Fill(smd_adc[i + 8], float(i) + 8);
      }
      else
      {
        smd_value_small->Fill(smd_adc[i + 8], float(i) + 8);
      }

      if (smd_adc[i + 8] > smd_adc_threshold)
      {
        smd_n_v_counter++;
      }
      //****************************

      //****smd south vertical individual channels****
      if (smd_adc[i + 24] > ver_cut)
      {
        s_ver++;
      }
      
      if (smd_adc[i + 24] > ped_cut)   smd_adc_s_ver_ind[i]->Fill(smd_adc[i + 24]);

      smd_value->Fill(smd_adc[i + 24], float(i) + 24);
    
      if (zdc_adc[0] > veto_cut)
      {
        smd_value_good->Fill(smd_adc[i + 24], float(i) + 24);
      }
      else
      {
        smd_value_small->Fill(smd_adc[i + 24], float(i) + 24);
      }

      if (smd_adc[i + 24] > smd_adc_threshold)
      {
        smd_s_v_counter++;
      }
      //****************************

      // Fill out the SMD counters with doubles instead of integers.
      double nh = smd_n_h_counter + 0.0;
      smd_north_hor_hits->Fill(nh);
      double nv = smd_n_v_counter + 0.0;
      smd_north_ver_hits->Fill(nv);
      double sh = smd_s_h_counter + 0.0;
      smd_south_hor_hits->Fill(sh);
      double sv = smd_s_v_counter + 0.0;
      smd_south_ver_hits->Fill(sv);
    }

    
    bool fired_smd_hor_n = (n_hor > 1);
    bool fired_smd_ver_n = (n_ver > 1);

    bool fired_smd_hor_s = (s_hor > 1);
    bool fired_smd_ver_s = (s_ver > 1);

   //compute, if smd is overloaded
    bool smd_ovld_north = false;
    bool smd_ovld_south = false;

    // FILLING OUT THE HISTOGRAMS
    

    if (fired_smd_hor_s && fired_smd_ver_s && !smd_ovld_south)
    {
      fill_hor_south = true;
      fill_ver_south = true;
      smd_hor_south->Fill(smd_pos[2]);
      smd_ver_south->Fill(smd_pos[3]);
      smd_sum_ver_south->Fill(smd_south_xsum);
      smd_sum_hor_south->Fill(smd_south_ysum);
    }

    if(fill_hor_south && fill_ver_south && (zdc_adc[0] > ZDC1cut) && (zdc_adc[2] > ZDC2cut) && (v[2] < veto_cut) && (v[3] < veto_cut))
    {
      smd_xy_south->Fill(smd_pos[3], smd_pos[2]);
      smd_hor_south_good->Fill(smd_pos[2]);
      smd_ver_south_good->Fill(smd_pos[3]);
    }

   
    if (fired_smd_hor_n && fired_smd_ver_n && !smd_ovld_north)
    {
      fill_hor_north = true;
      fill_ver_north = true;
      smd_hor_north->Fill(smd_pos[0]);
      smd_ver_north->Fill(smd_pos[1]);
      smd_sum_ver_north->Fill(smd_north_xsum);
      smd_sum_hor_north->Fill(smd_north_ysum);
      
    }

    if(fill_hor_north && fill_ver_north && (zdc_adc[8] > ZDC1cut) && (zdc_adc[10] > ZDC2cut) && (v[0] < veto_cut) && (v[1] < veto_cut))
    {
      smd_xy_north->Fill(smd_pos[1], smd_pos[0]);
      smd_hor_north_good->Fill(smd_pos[0]);
      smd_ver_north_good->Fill(smd_pos[1]);
    }

   
  }  // if packet good

  z.clear();
  sm.clear();
  tz.clear();
  tsmd.clear();
    
  delete p;
  return 0;
}

int ZdcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

void ZdcMon::CompSmdAdc()  // mulitplying by relative gains
{
  for (int i = 0; i < 15; i++)  // last one is reserved for analogue sum
  {
    // multiply SMD channels with their gain factors
    // to get the absolute ADC values in the same units
    //rgains come from CompSmdAdc()
    smd_adc[i] = smd_adc[i] * smd_north_rgain[i];            // sout -> north for PHENIX -> sPHENIX
    smd_adc[i + 16] = smd_adc[i + 16] * smd_south_rgain[i];  // north -> south for PHENIX-> sPHENIX
  }
}

void ZdcMon::CompSmdPos()  //computing position with weighted averages
{
  float w_ave[4];  // 0 -> north hor; 1 -> noth vert; 2 -> south hor; 3 -> south vert.
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights));  // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));

  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(PI / 4);  // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
  {
    weights[0] += smd_adc[i];  //summing weights
    weights[2] += smd_adc[i + 16];
    w_sum[0] += (float) i * smd_adc[i];  //summing for the average
    w_sum[2] += ((float) i + 16.) * smd_adc[i + 16];
  }
  for (int i = 0; i < 7; i++)
  {
    weights[1] += smd_adc[i + 8];
    weights[3] += smd_adc[i + 24];
    w_sum[1] += ((float) i + 8.) * smd_adc[i + 8];
    w_sum[3] += ((float) i + 24.) * smd_adc[i + 24];
  }

  if (weights[0] > 0.0)
  {
    w_ave[0] = w_sum[0] / weights[0];  //average = sum / sumn of weights...
    smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
  }
  else
  {
    smd_pos[0] = 0;
  }
  if (weights[1] > 0.0)
  {
    w_ave[1] = w_sum[1] / weights[1];
    smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
  }
  else
  {
    smd_pos[1] = 0;
  }

  if (weights[2] > 0.0)
  {
    w_ave[2] = w_sum[2] / weights[2];
    smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
  }
  else
  {
    smd_pos[2] = 0;
  }

  if (weights[3] > 0.0)
  {
    w_ave[3] = w_sum[3] / weights[3];
    smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
  }
  else
  {
    smd_pos[3] = 0;
  }
}




// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "LocalPolMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/eventReceiverClient.h>
#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TProfile.h>
#include <TRandom.h>
#include <TString.h>
#include <TSystem.h>

#include <cmath>
#include <cstdio>  // for printf
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>  // for allocator, string, char_traits
#include <utility>

LocalPolMon::LocalPolMon(const std::string& name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new LocalPolMon()
  return;  // test
}

int LocalPolMon::Init()
{
  iPoint = 0;
  for (bool& i : goodtrigger)
  {  // 16 triggers for gl1p
    i = false;
  }
  goodtrigger[10] = true;
  OnlMonServer* se = OnlMonServer::instance();
  const char* locpolcal = getenv("LOCALPOLCALIB");
  if (!locpolcal)
  {
    std::cout << "LOCALPOLCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string lpconfigfilename = std::string(locpolcal) + "/" + "LocPolConfig.dat";
  std::ifstream config(lpconfigfilename);
  std::ostringstream msg_config(lpconfigfilename);

  if (!config)
  {
    msg_config << lpconfigfilename << " could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL, msg_config.str(), 2);
    exit(1);
  }
  int err_counter = 0;
  TString key;
  TString val;
  while (!config.eof())
  {
    config >> key >> val;
    std::cout << "Config in " << config.good() << " state\n";
    if (!config.good())
    {
      break;
    }
    key.ToLower();
    std::cout << "Reading " << key << " with value " << val << std::endl;
    if (key == "trigger")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting an integer (2^(trigger bit number))\n keep only bit 10 as default value.\n";
        goodtrigger[10] = true;
      }
      else
      {
        int ival = val.Atoi();
        if (ival < 0 || ival > 65535)
        {
          std::cout << key << ": value outside expected range [0; 65535]\n Keep only bit 10 as default value.\n";
          goodtrigger[10] = true;
        }
        else
        {
          for (int i = 0; i < 16; i++)
          {
            goodtrigger[i] = false;
            if ((ival & (1 << i)))
            {
              goodtrigger[i] = true;
            }
          }
        }
      }
    }
    else if (key == "testfake")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting 0/1 for true or false\n Keep false as default.\n";
        fake = false;
      }
      else
      {
        int ival = val.Atoi();
        if (ival != 0 && ival != 1)
        {
          std::cout << key << ": expecting 0/1 for true or false\n Keep false as default.\n";
          fake = false;
        }
        else
        {
          if (ival == 1)
          {
            fake = true;
            erc = nullptr;
          }
          else
          {
            fake = false;
          }
        }
      }
    }
    else if (key == "monitoring"){
      val.ToLower();
      if (val == "online"){
	erc = new eventReceiverClient(eventReceiverClientHost);
      }
      else if (val == "offline" ){
	erc = new eventReceiverClient("localhost");
      }
      else {
	std::cout<< key << ": expecting either online (data stream)/offline (with eventServer -d 5263250 -s 5 -i -v -f path to gl1 prdf) "<<std::endl;
	std::cout<<"Fall back to online monitoring"<<std::endl;
	erc = new eventReceiverClient(eventReceiverClientHost);
      }
    }
    else if (key == "sphenixgap")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting an integer as the first bunch number of the continuous gap sequence\n Keep 111 as default.\n";
        ExpectedsPhenixGapPosition = 111;
      }
      else
      {
        int ival = val.Atoi();
        if (ival >= 0 && ival < 120)
        {
          ExpectedsPhenixGapPosition = ival;
        }
        else
        {
          std::cout << key << ": value outside expected range [0;119]\n Keep 117 as default value\n";
          ExpectedsPhenixGapPosition = 111;
        }
      }
    }
    else if (key == "threshold")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting a positive integer\n Keep 6000 as default value\n";
        EventCountThresholdGap = 6000;
      }
      else
      {
        int ival = val.Atoi();
        if (ival > 0 && ival < 1e5)
        {
          EventCountThresholdGap = ival;
        }
        else
        {
          std::cout << key << ": integer seems too big (>1e5)\n Keep default 6000\n";
          EventCountThresholdGap = 6000;
        }
      }
    }
    else if (key == "verbosity")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting 0/1 for false or true\n Keep 0(false) as default\n";
        verbosity = false;
      }
      else
      {
        int ival = val.Atoi();
        if (ival != 0 && ival != 1)
        {
          std::cout << key << ": value should be 0 or 1 for false of true\n Keep 0(false) as default\n";
          verbosity = false;
        }
        else if (ival == 1)
        {
          std::cout << "Making it verbose" << std::endl;
          verbosity = true;
        }
      }
    }
    else if (key == "integrated")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": expecting 0/1 for false or true\n Keep 0(false) as default\n";
        Integrated = false;
      }
      else
      {
        int ival = val.Atoi();
        if (ival != 0 && ival != 1)
        {
          std::cout << key << ": value should be 0 or 1 for false of true\n Keep 0(false) as default\n";
          Integrated = false;
        }
        else if (ival == 1)
        {
          std::cout << "Making it integrated" << std::endl;
          Integrated = true;
        }
      }
    }
    else if (key == "x0north")
    {
      if (val.IsFloat())
      {
        if (val.Atof() < -3. || val.Atof() > 3.)
        {
          std::cout << key << ": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value." << std::endl;
        }
        else
        {
          ZeroPosition[1] = val.Atof();
        }
      }
      else
      {
        std::cout << key << ": value is not a float\n keeping 0.0 as default value" << std::endl;
        ZeroPosition[1] = 0.;
      }
    }
    else if (key == "y0north")
    {
      if (val.IsFloat())
      {
        if (val.Atof() < -3. || val.Atof() > 3.)
        {
          std::cout << key << ": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value." << std::endl;
        }
        else
        {
          ZeroPosition[0] = val.Atof();
        }
      }
      else
      {
        std::cout << key << ": value is not a float\n keeping 0.0 as default value" << std::endl;
        ZeroPosition[0] = 0.;
      }
    }
    else if (key == "x0south")
    {
      if (val.IsFloat())
      {
        if (val.Atof() < -3. || val.Atof() > 3.)
        {
          std::cout << key << ": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value." << std::endl;
        }
        else
        {
          ZeroPosition[3] = val.Atof();
        }
      }
      else
      {
        std::cout << key << ": value is not a float\n keeping 0.0 as default value" << std::endl;
        ZeroPosition[3] = 0.;
      }
    }
    else if (key == "y0south")
    {
      if (val.IsFloat())
      {
        if (val.Atof() < -3. || val.Atof() > 3.)
        {
          std::cout << key << ": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value." << std::endl;
        }
        else
        {
          ZeroPosition[2] = val.Atof();
        }
      }
      else
      {
        std::cout << key << ": value is not a float\n keeping 0.0 as default value" << std::endl;
        ZeroPosition[2] = 0.;
      }
    }
    else if (key == "thresholdasymnewpoint")
    {
      if (!val.IsDigit())
      {
        std::cout << key << ": value is expected to be a positive integer\n Keeping default value of 5000 events." << std::endl;
      }
      else
      {
        std::cout << key << "Changed from " << EventsAsymmetryNewPoint << " to " << val.Atoi() << std::endl;
        EventsAsymmetryNewPoint = val.Atoi();
      }
    }
    else if (key == "zdc1cut")
      {
	if(!val.IsFloat())
	  {
	    std::cout<< key << " : value is expected to be a positive float\n Keeping default value of 60."<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< ZDC1Cut << " to "<<val.Atof()<<std::endl;
	    ZDC1Cut = val.Atof();
	  }
      }
    else if (key == "zdc2cut")
      {
	if(!val.IsFloat())
	  {
	    std::cout<< key << " : value is expected to be a positive float\n Keeping default value of 15."<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< ZDC2Cut << " to "<<val.Atof()<<std::endl;
	    ZDC2Cut = val.Atof();
	  }
      }
    else if (key == "vetocut")
      {
	if(!val.IsFloat())
	  {
	    std::cout<< key << " : value is expected to be a positive float\n Keeping default value of 150."<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< VetoCut << " to "<<val.Atof()<<std::endl;
	    VetoCut = val.Atof();
	  }
      }
    else if (key == "multiplicitylow")
      {
	if(!val.IsDigit())
	  {
	    std::cout<< key << " : value is expected to be a positive integer\n Keeping default value of 1"<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< MultiLow << " to "<<val.Atoi()<<std::endl;
	    MultiLow = val.Atoi();
	  }
      }
    else if (key == "multiplicityhigh")
      {
	if(!val.IsDigit())
	  {
	    std::cout<< key << " : value is expected to be a positive integer\n Keeping default value of 7 (for vertical and +1 for horizontal)"<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< MultiHigh << " to "<<val.Atoi()<<std::endl;
	    MultiHigh = val.Atoi();
	  }
      }
    else if (key == "smdthreshold")
      {
	if(!val.IsFloat())
	  {
	    std::cout<< key << " : value is expected to be a positive float\n Keeping default value of 5"<<std::endl; 
	  }
	else
	  {
	    std::cout<< key << " Changed from "<< SMDthr << " to "<<val.Atof()<<std::endl;
	    SMDthr = val.Atof();
	  }
      }
    else
    {
      err_counter++;
      std::cout << "Unknown configuration \n Expected:\n -verbosity\n -threshold\n -sphenixgap\n -testfake\n -monitoring\n -multiplicitylow\n -multiplicityHigh\n -trigger\n -X0north\n -X0south\n -Y0north\n -Y0south\n -thresholdasymnewpoint\n -nVetocut\n -SMDthreshold\n -ZDC1cut\n -ZDC2cut\n -Integrated\n key words" << std::endl;
    }
    if (err_counter > 3)
    {
      std::cout << "More than 3 unknown key words, abort" << std::endl;
      exit(1);
    }
  }

  std::string lpmappingfilename = std::string(locpolcal) + "/" + "ChannelMapping.txt";
  std::ifstream mapping(lpmappingfilename);
  std::ostringstream msg_mapping(lpmappingfilename);

  if (!mapping)
  {
    msg_mapping << lpmappingfilename << " could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL, msg_mapping.str(), 2);
    exit(1);
  }
  int adc, array, lowcut, highcut;
  std::string ChannelName;
  for (int i = 0; i < 128; i++)
  {
    mapping >> adc >> array >> lowcut >> highcut >> ChannelName;
    Chmapping[adc] = array;
    if(array>-1){
      lowSample[array]=lowcut;
      highSample[array]=highcut;
    }
  }

  const char* zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string calibfilename = std::string(zdccalib) + "/" + "SMDRelativeGain.dat";
  std::ifstream calibinput(calibfilename);
  std::ostringstream msg(calibfilename);

  if (!calibinput)
  {
    msg << calibfilename << " could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL, msg.str(), 2);
    exit(1);
  }
  int NS;
  int channel;
  float gain;
  for (int ch = 0; ch < 32; ch++)
  {
    calibinput >> NS >> channel >> gain;

    if (NS == 0 && channel >= 0 && channel < 16)
    {
      smd_north_relatgain[channel] = gain;
    }
    else if (NS == 1 && channel >= 0 && channel < 16)
    {
      smd_south_relatgain[channel] = gain;
    }
    else
    {
      msg << calibfilename << "could not understand the content, expect int(0/1) int(0-15) float for North or South, channel number and relative gain";
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }
  }

  h_Counts = new TH1D*[4];
  h_CountsScramble = new TH1D*[4];

  h_Counts[0] = new TH1D("h_BlueCountsUD", "h_BlueCountsUD", 4, 0, 4);
  h_Counts[1] = new TH1D("h_BlueCountsLR", "h_BlueCountsLR", 4, 0, 4);
  h_Counts[2] = new TH1D("h_YellCountsUD", "h_YellCountsUD", 4, 0, 4);
  h_Counts[3] = new TH1D("h_YellCountsLR", "h_YellCountsLR", 4, 0, 4);
  h_CountsScramble[0] = new TH1D("h_BlueCountsScrambleUD", "h_BlueCountsScrambleUD", 4, 0, 4);
  h_CountsScramble[1] = new TH1D("h_BlueCountsScrambleLR", "h_BlueCountsScrambleLR", 4, 0, 4);
  h_CountsScramble[2] = new TH1D("h_YellCountsScrambleUD", "h_YellCountsScrambleUD", 4, 0, 4);
  h_CountsScramble[3] = new TH1D("h_YellCountsScrambleLR", "h_YellCountsScrambleLR", 4, 0, 4);

  for(int i=0; i<4; i++){
    se->registerHisto(this,h_Counts[i]);
    se->registerHisto(this,h_CountsScramble[i]);
  }

  h_time = new TProfile("h_times", "h_time", 5000, -0.5, 4999.5);
  se->registerHisto(this, h_time);

  TString BeamName[2] = {"Blue", "Yell"};
  TString MethodName[2] = {"Arithmetic", "Geometric"};
  //TString Orientation[2] = {"LR", "UD"};
  TString Orientation[2] = {"UD", "LR"};
  h_Asym = new TH1D***[2];
  h_AsymScramble = new TH1D***[2];
  for (int beam = 0; beam < 2; beam++)
  {
    h_Asym[beam] = new TH1D**[2];
    h_AsymScramble[beam] = new TH1D**[2];
    for (int method = 0; method < 2; method++)
    {
      h_Asym[beam][method] = new TH1D*[2];
      h_AsymScramble[beam][method] = new TH1D*[2];
      for (int orient = 0; orient < 2; orient++)
      {
        h_Asym[beam][method][orient] = new TH1D(Form("h_Asym%s%s%s", BeamName[beam].Data(), MethodName[method].Data(), Orientation[orient].Data()), Form("Fwd %s %s %s Asym.", BeamName[beam].Data(), Orientation[orient].Data(), MethodName[method].Data()), 5000, -0.5, 4999.5);
        h_AsymScramble[beam][method][orient] = new TH1D(Form("h_AsymScramble%s%s%s", BeamName[beam].Data(), MethodName[method].Data(), Orientation[orient].Data()), Form("Bwk %s %s %s Asym.", BeamName[beam].Data(), Orientation[orient].Data(), MethodName[method].Data()), 5000, -0.5, 4999.5);

        se->registerHisto(this, h_Asym[beam][method][orient]);
        se->registerHisto(this, h_AsymScramble[beam][method][orient]);
      }
    }
  }
  for(int itrig=0; itrig<16; itrig++){
    h_trigger[itrig]=new TH1D(Form("h_trigger%d",itrig),Form("h_trigger%d",itrig),120,0,120);
    se->registerHisto(this,h_trigger[itrig]);
  }
  h_events=new TH1D("h_events","h_events",20,0,20);
  se->registerHisto(this,h_events);

  hspinpattern=new TH2I("hspinpattern","hspinpattern",120,0,120,2,0,2);
  se->registerHisto(this,hspinpattern);

  hmultiplicity[0]=new TH1D("hmultiplicitySMD_NH","hmultiplicitySMD_NH",9,0,9);
  hmultiplicity[1]=new TH1D("hmultiplicitySMD_NV","hmultiplicitySMD_NV",8,0,8);
  hmultiplicity[2]=new TH1D("hmultiplicitySMD_SH","hmultiplicitySMD_SH",9,0,9);
  hmultiplicity[3]=new TH1D("hmultiplicitySMD_SV","hmultiplicitySMD_SV",8,0,8);

  hposition[0]=new TH1D("hpositionSMD_NH_up","hpositionSMD_NH_up",100,-5,5);
  hposition[1]=new TH1D("hpositionSMD_NV_up","hpositionSMD_NV_up",100,-5,5);
  hposition[2]=new TH1D("hpositionSMD_SH_up","hpositionSMD_SH_up",100,-5,5);
  hposition[3]=new TH1D("hpositionSMD_SV_up","hpositionSMD_SV_up",100,-5,5);
  hposition[4]=new TH1D("hpositionSMD_NH_dn","hpositionSMD_NH_dn",100,-5,5);
  hposition[5]=new TH1D("hpositionSMD_NV_dn","hpositionSMD_NV_dn",100,-5,5);
  hposition[6]=new TH1D("hpositionSMD_SH_dn","hpositionSMD_SH_dn",100,-5,5);
  hposition[7]=new TH1D("hpositionSMD_SV_dn","hpositionSMD_SV_dn",100,-5,5);

  hadcsum[0]=new TH1D("hadcsumSMD_NH","hadcsumSMD_NH",100,0,4.5);
  hadcsum[1]=new TH1D("hadcsumSMD_NV","hadcsumSMD_NV",100,0,4.5);
  hadcsum[2]=new TH1D("hadcsumSMD_SH","hadcsumSMD_SH",100,0,4.5);
  hadcsum[3]=new TH1D("hadcsumSMD_SV","hadcsumSMD_SV",100,0,4.5);

  for(int i=0; i<4; i++){
    se->registerHisto(this,hmultiplicity[i]);
    se->registerHisto(this,hposition[i]);
    se->registerHisto(this,hposition[i+4]);
    se->registerHisto(this,hadcsum[i]);
  }

  hwaveform[0]=new TH2D("hwaveform0","hwaveform0",16,-0.5,15.5,16384,0,16384);
  hwaveform[1]=new TH2D("hwaveform1","hwaveform1",16,-0.5,15.5,16384,0,16384);
  hwaveform[2]=new TH2D("hwaveform2","hwaveform2",16,-0.5,15.5,16384,0,16384);
  hwaveform[3]=new TH2D("hwaveform3","hwaveform3",16,-0.5,15.5,16384,0,16384);
  hwaveform[4]=new TH2D("hwaveform4","hwaveform4",16,-0.5,15.5,16384,0,16384);
  hwaveform[5]=new TH2D("hwaveform5","hwaveform5",16,-0.5,15.5,16384,0,16384);
  for(int i=0; i<6; i++){
    se->registerHisto(this,hwaveform[i]);
  }
  
  //htimesync=new TH2I("htimesync","htimesync",65000,0,65000,65000,0,65000);
  //se->registerHisto(this,htimesync);
  hsyncfrac=new TH1D("hsyncfrac","hsyncfrac",2,0,2);
  se->registerHisto(this,hsyncfrac);

  Bluespace=new TH2D("Bluespace","Bluespace",50,-5,5,50,-5,5);
  se->registerHisto(this,Bluespace);
 
  Yellowspace=new TH2D("Yellowspace","Yellowspace",50,-5,5,50,-5,5);
  se->registerHisto(this,Yellowspace);

  hclocks = new TH2D("hclocks","hclocks",8192,0,8192,8192,0,8192);
  se->registerHisto(this, hclocks);

  hevolsync = new TH2D("hevolsync","",10000,0,30000000,2,0,2);
  se->registerHisto(this,hevolsync);

  hshiftevol=new TH2D("hshiftevol","",10000,0,30000000,20,0,20);
  se->registerHisto(this,hshiftevol);
  
  WaveformProcessingFast = new CaloWaveformFitting();
  myRandomBunch = new TRandom(0);
  Reset();
  return 0;
}

int LocalPolMon::BeginRun(const int runno)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  // Initialisation of the map (hopefully this step will not be required when gl1p scalers become available
  for (int i = 0; i < 16; i++)
  {  // 16 triggers for gl1p
    for (int bunchnr = 0; bunchnr < 120; bunchnr++)
    {
      gl1_counter[i][bunchnr] = 0;
    }
    if (verbosity)
    {
      std::cout << goodtrigger[15 - i];
    }
  }
  if (verbosity)
  {
    std::cout << "\n";
  }
  EvtShift=0;
  Initfirstbunch=false;
  Reset();
  RetrieveSpinPattern(runno);
  // goodtrigger[10]=true;//In 2023, it was MBD N&S
  return 0;
}

float LocalPolMon::anaWaveformFast(Packet* p, const int channel, const int low, const int high, const int ihisto)
{
  std::vector<float> waveform;
  waveform.reserve(high-low);
  //waveform.reserve(p->iValue(0, "SAMPLES"));
  //for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  for (int s = low; s < high; s++)
  {
    waveform.push_back(p->iValue(s, channel));
    hwaveform[ihisto]->Fill(s,p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);
  return result.at(0);
}

int LocalPolMon::process_event(Event* e /* evt */)
{

  //if (e->getEvtType() == BEGRUNEVENT /*9*/)
  //{  // spin patterns stored in BeginRun event
  //  RetrieveSpinPattern(e);
  //}

  RetrieveTriggerDistribution(e);
  h_events->Fill(0);
  /******Here we do the matching to identify the crossing shift   ******************/
  if (evtcnt > EventCountThresholdGap){
    StartAbortGapData = RetrieveAbortGapData();
  }
  else
  {
    StartAbortGapData = ExpectedsPhenixGapPosition;  // default value from config
  }

  CrossingShift = StartAbortGapPattern - StartAbortGapData;
  if (verbosity)
  {
    std::cout << "Crossing shift: " << CrossingShift << " = ( " << StartAbortGapPattern << " - " << StartAbortGapData << " )\n";
  }
  /************* end of crossing shift *********************/

  Packet* psmd = e->getPacket(packetid_smd);
  int bunchnr = 0;
  if (psmd)
  {
    h_events->Fill(1);
    long long int zdc_clock=psmd->lValue(0,"CLOCK");
    bunchnr=RetrieveBunchNumber(e,zdc_clock);
    //Did not manage to retrieve the proper bunch number, no need to continue the calculation
    //let us clean and leave
    if(bunchnr<0){
      delete psmd;
      psmd=nullptr;
      return 0;
    }
    h_events->Fill(2);

    if (fake)
    {
      bunchnr += 15 + myRandomBunch->Integer(4);
    }

    // get minimum on ZDC second module
    // get minimum on ZDC second module
    signalZDCN1 = anaWaveformFast(psmd, ZDCN1, lowSample[ZDCN1], highSample[ZDCN1],0);
    signalZDCS1 = anaWaveformFast(psmd, ZDCS1, lowSample[ZDCS1], highSample[ZDCS1],1);
    signalZDCN2 = anaWaveformFast(psmd, ZDCN2, lowSample[ZDCN2], highSample[ZDCN2],0);
    signalZDCS2 = anaWaveformFast(psmd, ZDCS2, lowSample[ZDCS2], highSample[ZDCS2],1);
    vetoNF = anaWaveformFast(psmd, 16, lowSample[ivetoNF], highSample[ivetoNF],4);
    vetoNB = anaWaveformFast(psmd, 17, lowSample[ivetoNB], highSample[ivetoNB],4);
    vetoSF = anaWaveformFast(psmd, 80, lowSample[ivetoSF], highSample[ivetoSF],5);
    vetoSB = anaWaveformFast(psmd, 81, lowSample[ivetoSB], highSample[ivetoSB],5);

    if ( (signalZDCN2 < 10 || signalZDCN1<75) && (signalZDCS2 < 10 || signalZDCS1<75) )
    {
      delete psmd;
      psmd=nullptr;
      return 0;
    }
    h_events->Fill(3);

    // for(int ch=16; ch<47; ch++){//according to mapping in ZDC logbook entry #85 March 9th 2024
    for (auto& it : Chmapping)
    {  // new mapping for ZDC/SMD/Veto with 2 ADC boards//May 13th 2024
      if (it.second < 16)
      {
        continue;
      }
      if (it.second > 47)
      {
        continue;
      }
      float signalFast = anaWaveformFast(psmd, it.first, lowSample[it.second],highSample[it.second],it.second/32+2);  // fast waveform fitting

      int ch = it.second;
      // Scale according to relative gain calibration factor
      //if (ch < 16){
      //	zdc_adc[ch]=signalFast;
      //}
      //else 
      if (ch < 32)
      {
        smd_adc[ch - 16] = signalFast * smd_north_relatgain[ch - 16];
      }
      else
      {
        smd_adc[ch - 16] = signalFast * smd_south_relatgain[ch - 32];
      }
    }
    float Weights[4] = {0};
    memset(Weights, 0, sizeof(Weights));
    float AveragePosition[4] = {0};
    memset(AveragePosition, 0, sizeof(AveragePosition));

    for (int ch = 0; ch < 8; ch++)
    {
      Weights[0] += smd_adc[ch];
      AveragePosition[0] += ConversionSign[0] * pitchY * (ch - (nchannelsY-1.) / 2) * smd_adc[ch];  // North Y direction (Asym in blue)
      Weights[2] += smd_adc[ch + 16];
      AveragePosition[2] += ConversionSign[2] * pitchY * (ch - (nchannelsY-1.) / 2) * smd_adc[ch + 16];  // South Y direction (Asym in Yellow)

      if (ch == 7)
      {
        continue;
      }
      Weights[1] += smd_adc[ch + 8];
      AveragePosition[1] += ConversionSign[1] * pitchX * (ch - (nchannelsX-1.) / 2) * smd_adc[ch + 8];  // North X direction (Asym in Blue)
      Weights[3] += smd_adc[ch + 24];
      AveragePosition[3] += ConversionSign[3] * pitchX * (ch - (nchannelsX-1.) / 2) * smd_adc[ch + 24];  // South X direction (Asym in Yellow)
    }

    for (int i = 0; i < 4; i++)
    {
      if (Weights[i] > 0.0)
      {
        AveragePosition[i] /= Weights[i];
	h_events->Fill(4+i);
      }
      else
      {
	AveragePosition[i]=0.;
        continue;  // most likely the most appropriate
      }

      if (AveragePosition[i] < ZeroPosition[i] - 0.5)
      {
	h_events->Fill(8+i);
	if (verbosity)
	  {
	    std::cout<<"Spin pattern size for "<<i/2<<" : "<<SpinPatterns[i/2].size()<<std::endl;
	  }
        // (i/2)=0 for blue beam=North, (i/2)=1 for yellow beam=South
        if (SpinPatterns[i / 2].at((120 + bunchnr + CrossingShift) % 120) > 0)
        {
          if(GoodSelection(i/2)) {
	    h_Counts[i]->Fill(3);  // Right for pointing up
	    hposition[i]->Fill(AveragePosition[i]);
	  }
        }
        else if (SpinPatterns[i / 2].at((120 + bunchnr + CrossingShift) % 120) < 0)
        {
          if(GoodSelection(i/2)) {
	    h_Counts[i]->Fill(1);  // Left for pointing down
	    hposition[i+4]->Fill(AveragePosition[i]);
	  }
        }

        // we swap the spin pattern to get random orientation and check biased asymmetry
        if (SpinPatterns[i / 2 == 0 ? 1 : 0].at((120 + bunchnr + CrossingShift) % 120) > 0)
        {
          if(GoodSelection(i/2))h_CountsScramble[i]->Fill(3);  // Right for pointing up
        }
        else if (SpinPatterns[i / 2 == 0 ? 1 : 0].at((120 + bunchnr + CrossingShift) % 120) < 0)
        {
          if(GoodSelection(i/2))h_CountsScramble[i]->Fill(1);  // Left for pointing down
        }
      }
      else if (AveragePosition[i] > ZeroPosition[i] + 0.5)
      {
	h_events->Fill(8+i);
        // (i/2)=0 for blue beam, =1 for yellow beam
        if (SpinPatterns[i / 2].at((120 + bunchnr + CrossingShift) % 120) > 0)
        {
          if(GoodSelection(i/2)){
	    h_Counts[i]->Fill(0);  // Left for pointing up
	    hposition[i]->Fill(AveragePosition[i]);
	  }
        }
        else if (SpinPatterns[i / 2].at((120 + bunchnr + CrossingShift) % 120) < 0)
        {
          if(GoodSelection(i/2)){
	    h_Counts[i]->Fill(2);  // Right for pointing down
	    hposition[i+4]->Fill(AveragePosition[i]);
	  }
        }

        // we swap the spin pattern to get random orientation and check biased asymmetry
        if (SpinPatterns[i / 2 == 0 ? 1 : 0].at((120 + bunchnr + CrossingShift) % 120) > 0)
        {
          if(GoodSelection(i/2))h_CountsScramble[i]->Fill(0);  // Left for pointing up
        }
        else if (SpinPatterns[i / 2 == 0 ? 1 : 0].at((120 + bunchnr + CrossingShift) % 120) < 0)
        {
          if(GoodSelection(i/2))h_CountsScramble[i]->Fill(2);  // Right for pointing down
        }
      }
    }
    double rnorth2=pow(AveragePosition[0]-ZeroPosition[0],2)+pow(AveragePosition[1]-ZeroPosition[1],2);
    double rsouth2=pow(AveragePosition[2]-ZeroPosition[2],2)+pow(AveragePosition[3]-ZeroPosition[3],2);
    if(rnorth2>1 && GoodSelection(0)){
      Bluespace->Fill(AveragePosition[1],AveragePosition[0]);
    }
    if(rsouth2>1 && GoodSelection(1)){
      Yellowspace->Fill(AveragePosition[3],AveragePosition[2]);
    }
    h_time->Fill(iPoint, e->getTime());
    evtcnt++;
    evtcntA++;
    delete psmd;
    psmd=nullptr;
  }
  else
  {
    if (verbosity)
    {
      std::cout << "Missing ZDC/SMD packet " << packetid_smd << std::endl;
    }
  }

  /**** Compute asymmetries if we have enough events ****************/
  //if (evtcnt % EventsAsymmetryNewPoint == 0)
  if (evtcntA > EventsAsymmetryNewPoint )
  {
    h_events->Fill(12);
    for (int ibeam = 0; ibeam < 2; ibeam++)
    {
      for (int orient = 0; orient < 2; orient++)
      {
        double L_U = h_Counts[2 * ibeam + orient]->GetBinContent(1);
        double R_D = h_Counts[2 * ibeam + orient]->GetBinContent(2);
        double L_D = h_Counts[2 * ibeam + orient]->GetBinContent(3);
        double R_U = h_Counts[2 * ibeam + orient]->GetBinContent(4);
        // if(L_U<0){
        //   std::cout<<iPoint<<" "<<evtcnt<<" "<<h_Counts[2*ibeam+orient]->GetEntries()<<" "<<std::endl;
        // }
        double* asymresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);

        h_Asym[ibeam][0][orient]->SetBinContent(iPoint + 1, asymresult[0]);
        h_Asym[ibeam][0][orient]->SetBinError(iPoint + 1, asymresult[1]);

        h_Asym[ibeam][1][orient]->SetBinContent(iPoint + 1, asymresult[2]);
        h_Asym[ibeam][1][orient]->SetBinError(iPoint + 1, asymresult[3]);

        L_U = h_CountsScramble[2 * ibeam + orient]->GetBinContent(1);
        R_D = h_CountsScramble[2 * ibeam + orient]->GetBinContent(2);
        L_D = h_CountsScramble[2 * ibeam + orient]->GetBinContent(3);
        R_U = h_CountsScramble[2 * ibeam + orient]->GetBinContent(4);

        double* scrambleresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);

        h_AsymScramble[ibeam][0][orient]->SetBinContent(iPoint + 1, scrambleresult[0]);
        h_AsymScramble[ibeam][0][orient]->SetBinError(iPoint + 1, scrambleresult[1]);

        h_AsymScramble[ibeam][1][orient]->SetBinContent(iPoint + 1, scrambleresult[2]);
        h_AsymScramble[ibeam][1][orient]->SetBinError(iPoint + 1, scrambleresult[3]);

        delete asymresult;
        delete scrambleresult;
      }
    }
    // Now that everything has been calculated, let move to the next point for next event
    iPoint++;
    evtcntA=0;
    if(!Integrated){
      for(int i =0; i<4; i++){
	h_Counts[i]->Reset();
	h_CountsScramble[i]->Reset();
      }
    }
  }

  return 0;
}

int LocalPolMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  evtcntA=0;
  iPoint = 0;
  failuredepth=0;
  for (int i = 0; i < 4; i++)
  {
    h_Counts[i]->Reset();
    h_CountsScramble[i]->Reset();
  }
  h_time->Reset();
  return 0;
}

bool LocalPolMon::GoodSelection(int i)
{
  bool goodselection=true;
  if(i==0){//dealing with north side
    int nv=0; 
    int nh=0;
    double sumH=0;
    double sumV=0;
    for(int ch=0; ch<8; ch++) {
      nh +=(smd_adc[ch]  >SMDthr)?1:0;
      sumH+=smd_adc[ch];
    }
    for(int ch=0; ch<7; ch++) {
      nv +=(smd_adc[ch+8]>SMDthr)?1:0;
      sumV+=smd_adc[ch+8];
    }
    goodselection &=(signalZDCN1>ZDC1Cut && signalZDCN2>ZDC2Cut);
    goodselection &=(vetoNF<VetoCut&&vetoNB<VetoCut);
    if(goodselection&&(nv>MultiLow&&nv<MultiHigh+1)) hmultiplicity[0]->Fill(nh);
    if(goodselection&&(nh>MultiLow&&nh<MultiHigh+1)) hmultiplicity[1]->Fill(nv);
    goodselection &=(nv>MultiLow&&nh>MultiLow);
    goodselection &=(nv<MultiHigh&&nh<MultiHigh+1);
    if((sumH>0)&&goodselection) hadcsum[0]->Fill(log10(sumH));
    if((sumV>0)&&goodselection) hadcsum[1]->Fill(log10(sumV));
  }
  else{//south side
    int nv=0; 
    int nh=0;
    double sumH=0;
    double sumV=0;
    for(int ch=0; ch<8; ch++) {
      nh +=(smd_adc[ch+16] >SMDthr)?1:0;
      sumH+=smd_adc[ch+16];
    }
    for(int ch=0; ch<7; ch++) {
      nv +=(smd_adc[ch+24] >SMDthr)?1:0;
      sumV+=smd_adc[ch+24];
    }
    goodselection &=(signalZDCS1>ZDC1Cut && signalZDCS2>ZDC2Cut);
    goodselection &=(vetoSF<VetoCut&&vetoSB<VetoCut);
    if(goodselection&&(nv>MultiLow&&nv<MultiHigh+1)) hmultiplicity[2]->Fill(nh);
    if(goodselection&&(nh>MultiLow&&nh<MultiHigh+1)) hmultiplicity[3]->Fill(nv);
    goodselection &=(nv>MultiLow&&nh>MultiLow);
    goodselection &=(nv<MultiHigh+1&&nh<MultiHigh+1);
    if((sumH>0)&&goodselection) hadcsum[2]->Fill(log10(sumH));
    if((sumV>0)&&goodselection) hadcsum[3]->Fill(log10(sumV));
  }

  return goodselection;
}

double* LocalPolMon::ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U)
{
  double* result = new double[4];
  double leftA = L_U + R_D;
  double rightA = L_D + R_U;
  double tmpNumA = leftA - rightA;
  double tmpDenA = leftA + rightA;
  result[0] = 0;
  result[1] = 0;

  if (tmpDenA > 0)
  {
    result[0] = tmpNumA / tmpDenA;
    result[1] = 2 * sqrt(pow(rightA, 2) * leftA + pow(leftA, 2) * rightA) / pow(tmpDenA, 2);
  }
  //std::cout<<leftA<<" "<<rightA<<"\t\t"<<result[0]<<" +/- "<<result[1]<<std::endl;

  double leftG = sqrt(L_U * R_D);
  double rightG = sqrt(L_D * R_U);
  double tmpNumG = leftG - rightG;
  double tmpDenG = leftG + rightG;
  result[2] = 0;
  result[3] = 0;

  if (tmpDenG > 0)
  {
    result[2] = tmpNumG / tmpDenG;
    result[3] = sqrt(pow(rightG, 2) * leftA + pow(leftG, 2) * rightA) / pow(tmpDenG, 2);
  }
  return result;
}

void LocalPolMon::RetrieveSpinPattern(int runnb){
  SpinPatterns[BLUE].clear();
  SpinPatterns[YELLOW].clear();
  for(int i = 0; i < 120; i++){
    SpinPatterns[BLUE][i]=0;
    SpinPatterns[YELLOW][i]=0;
  }
  TString retvalBlue;
  TString retvalYellow;
  const char* user=getenv("USER");
  if(strcmp(user,"phnxrc")==0){
    //We are on machine which can access operator1 computer without password
    retvalBlue=gSystem->GetFromPipe(Form("$ONLMON_MACROS/GetSpinPatternHTTP.sh %d",14902));
    retvalYellow=gSystem->GetFromPipe(Form("$ONLMON_MACROS/GetSpinPatternHTTP.sh %d",14903));
    if(verbosity){
      std::cout<<"Retrieving SpinPattern from HTTP"<<std::endl;
    }
  }
  else{
    OnlMonServer* se = OnlMonServer::instance();
    TString runtype=se->GetRunType();
    runtype.ToLower();
    retvalBlue=gSystem->GetFromPipe(Form("$ONLMON_MACROS/GetSpinPatternGL1.sh %d %s %d",14902,runtype.Data(),runnb));
    retvalYellow=gSystem->GetFromPipe(Form("$ONLMON_MACROS/GetSpinPatternGL1.sh %d %s %d",14903,runtype.Data(),runnb));
    if(verbosity){
      std::cout<<"Retrieving SpinPattern from GL1 "<<runtype.Data() <<" prdf file for run "<<runnb<<std::endl;
    }
  }
  int flip=-1;//https://phenix-intra.sdcc.bnl.gov/phenix/WWW/offline/wikioff/index.php/Spin_Database_:_Note_for_Analyzer
  TObjArray* BunchSpinBlue=retvalBlue.Tokenize(" ");
  BunchSpinBlue->SetOwner(kTRUE);
  int nFilledBunchesBlue=BunchSpinBlue->GetEntries();
  if(verbosity){
    std::cout<<"Blue spin patterns for "<<nFilledBunchesBlue<<" bunches "<<std::endl;
    std::cout<<retvalBlue<<std::endl;
  }
  int stepBlue=(112-nFilledBunchesBlue)/28+1;
  //for(int i=0; i<120; i=i+stepBlue){
  for(int i=0; i<nFilledBunchesBlue; i++){
    SpinPatterns[BLUE][stepBlue*i]=flip*((TObjString*)BunchSpinBlue->At(i))->String().Atoi();
    hspinpattern->Fill(stepBlue*i,1,((TObjString*)BunchSpinBlue->At(i))->String().Atoi());
  }
  BunchSpinBlue->Clear();
  delete BunchSpinBlue;

  TObjArray* BunchSpinYellow=retvalYellow.Tokenize(" ");
  BunchSpinYellow->SetOwner(kTRUE);
  int nFilledBunchesYell=BunchSpinYellow->GetEntries();
  if(verbosity){
    std::cout<<"Yellow spin patterns for "<<nFilledBunchesYell<<" bunches "<<std::endl;
    std::cout<<retvalYellow<<std::endl;
  }
  int stepYellow=(112-nFilledBunchesYell)/28+1;
  //for(int i=0; i<120; i=i+stepYellow){
  for(int i=0; i<nFilledBunchesYell; i++){
    SpinPatterns[YELLOW][stepYellow*i]=flip*((TObjString*)BunchSpinYellow->At(i))->String().Atoi();
    hspinpattern->Fill(stepYellow*i,0.,((TObjString*)BunchSpinYellow->At(i))->String().Atoi());
  }
  //Check if consitent information (as the method works only with multiples of 28 filled bunches and a constant abort gap of 9 bunches
  if(stepYellow==stepBlue){
    nEmptyAbort=stepYellow-1;
  }
  BunchSpinYellow->Clear();
  delete BunchSpinYellow;
}

void LocalPolMon::RetrieveTriggerDistribution(Event* e){
  Event* egl1p=nullptr;
  Packet* pgl1p = nullptr;
  if (erc){
    if(verbosity){
      std::cout<<"Inside RetrieveTrigger::ERC"<<e->getEvtSequence()<<" "<<EvtShift<<std::endl;
    }
    egl1p = erc->getEvent(e->getEvtSequence()+EvtShift);
    if(verbosity){
      std::cout<<egl1p<<std::endl;
    }
  }
  if (egl1p){
    pgl1p = egl1p->getPacket(packetid_gl1);
    if (pgl1p){
      int bunchnr = pgl1p->lValue(0, "BunchNumber");
      for (int i=0; i<16; i++){//auto& i : gl1_counter)
	// 16 triggers for gl1p
	// With prdf pgl1->lValue(i,2); simply returns the current processed event number (which can shaddow the abort gap: the lagging bunch# at some point get back to the position of the others)
	// So instead, we increment the number of processed events per bunch number, for the various triggers
	//gl1_counter[i][bunchnr]+= (pgl1p->lValue(i,2)>0)?1:0;
	if(pgl1p->lValue(i,2)>0){
	  gl1_counter[i][bunchnr]++;
	  h_trigger[i]->Fill(bunchnr);
	}
      }
      delete pgl1p;
      pgl1p=nullptr;
    }
    delete egl1p;
    egl1p=nullptr;
  }
}

int LocalPolMon::RetrieveAbortGapData(){
  // Here we do the magic to identify the abort gap
  std::vector<int> gap[16];
  std::map<int, int> begingap;
  std::map<int, int> endgap;

  for (int i = 0; i < 16; i++){
    if (!goodtrigger[i]){
      continue;
    }
    if(h_trigger[i]->GetEntries()<1000) continue;
    std::map<int, long long> tmpmap = gl1_counter[i];
    for (int emptyfill = 0; emptyfill < 9; emptyfill++){
      int myminimum = min_element(tmpmap.begin(), tmpmap.end(), [](const std::pair<int, long long>& lhs, const std::pair<int, long long>& rhs)
				  { return lhs.second < rhs.second; })
	->first;
      if (myminimum < 111){
	gap[i].push_back(120 + myminimum);
      }
      else{
	gap[i].push_back(myminimum);
      }
      tmpmap.erase(myminimum);
    }
    begingap[i] = (*min_element(gap[i].begin(), gap[i].end()))+nEmptyAbort;
    endgap[i] = (*max_element(gap[i].begin(), gap[i].end()));
    if (endgap[i] - begingap[i] > 9){
      std::cout << " Weird abort gap is larger than 9 bunches " << endgap[i] << " - " << begingap[i] << " for trigger " << i << std::endl;
    }
  }
  for (auto ib = begingap.begin(); ib != begingap.end(); ++ib){
    if (begingap.begin()->second != ib->second)
      {
	std::cout << " Weird abort gap not in the same location between trigger bit 0 and trigger bit " << ib->second << std::endl;
      }
  }
  if (begingap.empty()) return 111;
  else return (begingap.begin()->second) % 120;
}


int LocalPolMon::RetrieveBunchNumber(Event* e, long long int zdc_clock){
  Event* egl1 = nullptr;
  int bunch=-1;
  //static int localfail=0;
  if(!Initfirstbunch){
    EvtShift=0;
    EvtShiftValid=0;
    Prevzdc_clock=zdc_clock;
  }
  if(verbosity){
    std::cout<<"Inside RetrieveBunchNumber"<<std::endl;
  }
  if(failuredepth>1000){
    if(verbosity){
      std::cout<<"ZDC and GL1p asynchronous by more than 1000 events"<<std::endl;
      std::cout<<"Giving up this event and go to the next one"<<std::endl;
    }
    EvtShift=EvtShiftValid;
    failuredepth=0;
    return -1;
  }
  //std::cout<<e->getEvtSequence()<<std::endl;
  if (erc){
    if(verbosity){
      std::cout<<"Inside RetrieveBunchNumber::ERC "<<e->getEvtSequence() <<" "<<EvtShift <<std::endl;
    }
    egl1 = erc->getEvent(e->getEvtSequence()+EvtShift);
    if(verbosity){
      std::cout<<egl1<<std::endl;
    }
  }
  if (egl1){
    if(verbosity){
      std::cout<<"Inside RetrieveBunchNumber::GL1"<<std::endl;
    }
    Packet* pgl1p = egl1->getPacket(packetid_gl1);
    if (pgl1p){
      if(verbosity){
	std::cout<<"Inside RetrieveBunchNumber::PGL1p"<<std::endl;
      }
      long long int gl1_clock=pgl1p->lValue(0, "BCO");
      if(verbosity){
       std::cout<<"EvtShift: "<<EvtShift<<" zdc: "<<zdc_clock<<"    gl1p: "<<gl1_clock<<std::endl;
      }

      if(!Initfirstbunch){
	Prevgl1_clock=gl1_clock;
	Initfirstbunch=true;
	bunch = pgl1p->lValue(0, "BunchNumber");
	if(verbosity){
	  std::cout<<"Init Bunch number is : "<<bunch<<std::endl;
	}
	delete pgl1p;
	pgl1p=nullptr;
	delete egl1;
	egl1=nullptr;
	return bunch;
      }
      if(zdc_clock<Prevzdc_clock){
	//Prevzdc_clock=Prevzdc_clock-4294967296;//despite the long long, it seems it is only 32 bits
	//std::cerr<<"Mismatched: "<< e->getEvtSequence()<<" shift: "<<EvtShift<<" zdc: "<<zdc_clock<<" - "<<Prevzdc_clock<<" = "<<(zdc_clock-Prevzdc_clock) <<"    gl1p: "<<gl1_clock<<" - "<<Prevgl1_clock<<" = "<<(gl1_clock-Prevgl1_clock) <<std::endl;
	zdc_clock+=(long long int)1<<32;
	//std::cerr<<"And now Mismatched: "<< e->getEvtSequence()<<" shift: "<<EvtShift<<" zdc: "<<zdc_clock<<" - "<<Prevzdc_clock<<" = "<<(zdc_clock-Prevzdc_clock) <<"    gl1p: "<<gl1_clock<<" - "<<Prevgl1_clock<<" = "<<(gl1_clock-Prevgl1_clock) <<std::endl;
	//exit(1);
      }
      hclocks->Fill((gl1_clock-Prevgl1_clock)%8192,(zdc_clock-Prevzdc_clock)%8192);
      if((gl1_clock-Prevgl1_clock)!=(zdc_clock-Prevzdc_clock)){
	if(verbosity){
	  std::cout<<"Mismatched: "<<EvtShift<<" zdc: "<<(zdc_clock-Prevzdc_clock) <<"    gl1p: "<<(gl1_clock-Prevgl1_clock) <<std::endl;
        }
	//std::cerr<<"Mismatched: "<< e->getEvtSequence()<<" shift: "<<EvtShift<<" zdc: "<<zdc_clock<<" - "<<Prevzdc_clock<<" = "<<(zdc_clock-Prevzdc_clock) <<"    gl1p: "<<gl1_clock<<" - "<<Prevgl1_clock<<" = "<<(gl1_clock-Prevgl1_clock) <<std::endl;
	//if((zdc_clock-Prevzdc_clock)<0){
	//  exit(1) ;
	//}
	EvtShift++;
	delete pgl1p;
	pgl1p=nullptr;
	delete egl1;
	egl1=nullptr;
	failuredepth++;
	hsyncfrac->Fill(0.);
	hevolsync->Fill(e->getEvtSequence(),0);
	bunch=RetrieveBunchNumber(e,zdc_clock);
      }
      else{
	Prevgl1_clock=gl1_clock;
	if(zdc_clock>(long long int)1<<32){
	  zdc_clock-=(long long int)1<<32;
	}
	Prevzdc_clock=zdc_clock;
	EvtShiftValid=EvtShiftValid;
	hsyncfrac->Fill(1.);
	hevolsync->Fill(e->getEvtSequence(),1);
	hshiftevol->Fill(e->getEvtSequence(),EvtShift);
	bunch = pgl1p->lValue(0, "BunchNumber");
	//failuredepth=0;
	if(verbosity){
	  std::cout<<"Bunch number is : "<<bunch<<std::endl;
	}
      }
      delete pgl1p;
      pgl1p = nullptr;
    }
    else{
      if (verbosity){
	std::cout << "Failed grabing gl1 from event receiver, Bunch number unknown" << std::endl;
      }
    }
    delete egl1;
    egl1 = nullptr;
  }
  return bunch;
}

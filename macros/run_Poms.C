// Phenix Online Monitoring System
//
// Macro loader for starting Online Monitoring GUI Framework
//
// Author:  Cody McCain (mcm99c@acu.edu)
// Date:    January 2, 2003
//
// See run_Poms.C.README

#include <onlmon/Poms.h>
#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libpoms.so)
R__LOAD_LIBRARY(libonlmonclient.so)

void StartPoms()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->ReadServerHistoMap();
  PomsMainFrame *pmf;
  pmf = PomsMainFrame::Instance();

  // Register SubSystems
  //  pmf->RegisterSubSystem("EXAMPLE", "example");
  SubSystem *subsys = nullptr;

  subsys = new SubSystem("MBD", "bbc");
  subsys->AddAction("bbcDraw(\"FIRST\")", "MBD Vertex Monitor");
  subsys->AddAction("bbcDraw(\"SECOND\")", "MBD Timing Monitor");
  subsys->AddAction("bbcDraw(\"THIRD\")", "MBD Triggered Monitor");
  subsys->AddAction("bbcDraw(\"BbcMon5\")", "Vertex for All Triggers");
  subsys->AddAction("bbcDraw(\"MBD2MCR\")", "MBD TOGGLE VTX TO MCR");
  subsys->AddAction("bbcDraw(\"MBDZRESET\")", "MBD RESET VTX");
  // crashes
  //  subsys->AddAction("bbcDraw(\"BbcMon6\")","Packet Decoder Status");
  //  subsys->AddAction("bbcDraw(\"BbcAutoUpdate\")", "MBD VTX Auto Update");
  //  subsys->AddAction("bbcDraw(\"BADGL1\")", "MBD TOGGLE IGNORE GL1 MISS");
  subsys->AddAction("bbcDraw(\"BbcMonServerStats\")", "MBD Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("CEMC", "cemc");
  subsys->AddAction("cemcDraw(\"FIRST\")", "Towers");
  subsys->AddAction("cemcDraw(\"SECOND\")", "Packet Decoder Status");
  subsys->AddAction("cemcDraw(\"THIRD\")", "Wave Forms");
  subsys->AddAction("cemcDraw(\"FIFTH\")", "Trigger [Expert]");
  subsys->AddAction("cemcDraw(\"ALLTRIGHITS\")", "All Trigger Tower Hits");
  subsys->AddAction("cemcDraw(\"SEVENTH\")", "Zero-suppression info");
  subsys->AddAction("cemcDraw(\"ALLTRIGZS\")", "All Trigger ZS info");
  subsys->AddAction("cemcDraw(\"BADCHI2\")", "Bad Chi2 Towers [Expert]");
  subsys->AddAction("cemcDraw(\"NOISERMS\")", "tower prepost RMS");
  subsys->AddAction("cemcDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  // subsys = new SubSystem("DAQ", "daq");
  // subsys->AddAction("daqDraw(\"FIRST\")", "Calo-GL1 Check");
  // subsys->AddAction("daqDraw(\"SECOND\")", "Calo FEM Check");
  // subsys->AddAction("daqDraw(\"SERVERSTATS\")", "Server Stats");
  // subsys->AddAction(new SubSystemActionSavePlot(subsys));
  // pmf->RegisterSubSystem(subsys);


   subsys = new SubSystem("GL1", "gl1");
   subsys->AddAction("gl1Draw(\"SCALED\")", "Scaled Triggers");
   subsys->AddAction("gl1Draw(\"LIVE\")", "Live Triggers [Expert]");
   subsys->AddAction("gl1Draw(\"REJECTION\")", "Rejection");
   subsys->AddAction("gl1Draw(\"SERVERSTATS\")", "Server Stats");
   subsys->AddAction(new SubSystemActionSavePlot(subsys));
   pmf->RegisterSubSystem(subsys);

   // subsys = new SubSystem("SPIN", "spin");
   // subsys->AddAction("spinDraw(\"THIRD\")", "The Old Abort Gap");
   // subsys->AddAction(new SubSystemActionSavePlot(subsys));
   // pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("Inner HCAL", "ihcal");
  subsys->AddAction("ihcalDraw(\"FIRST\")", "Towers");
  //subsys->AddAction("ihcalDraw(\"SECOND\")", "Sector Average [Expert]");
  subsys->AddAction("ihcalDraw(\"THIRD\")", "Wave Form");
  //subsys->AddAction("ihcalDraw(\"FOURTH\")", "Packet Health [Expert]");
  //subsys->AddAction("ihcalDraw(\"EIGHTH\")","Packet Decoder Status");
  subsys->AddAction("ihcalDraw(\"FIFTH\")", "Trigger [Expert]");
  subsys->AddAction("ihcalDraw(\"SIXTH\")", "Tower Status [Expert]");
  subsys->AddAction("ihcalDraw(\"ALLTRIGHITS\")", "All Trigger Tower Hits");
  subsys->AddAction("ihcalDraw(\"SEVENTH\")", "Zero-suppression info");
  subsys->AddAction("ihcalDraw(\"ALLTRIGZS\")", "All Trigger ZS info");
  subsys->AddAction("ihcalDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction("ihcalDraw(\"NOISERMS\")", "tower prepost RMS");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("Outer HCAL", "ohcal");
  subsys->AddAction("ohcalDraw(\"FIRST\")", "Towers");
  //subsys->AddAction("ohcalDraw(\"SECOND\")", "Sector Average [Expert]");
  subsys->AddAction("ohcalDraw(\"THIRD\")", "Wave Form");
  //subsys->AddAction("ohcalDraw(\"FOURTH\")", "Packet Health [Expert]");
  subsys->AddAction("ohcalDraw(\"FIFTH\")", "Trigger [Expert]");
  subsys->AddAction("ohcalDraw(\"SIXTH\")", "Tower Status [Expert]");
  subsys->AddAction("ohcalDraw(\"ALLTRIGHITS\")", "All Trigger Tower Hits");
  subsys->AddAction("ohcalDraw(\"SEVENTH\")", "Zero-suppression info");
  //subsys->AddAction("ohcalDraw(\"EIGHTH\")","Packet Decoder Status");
  subsys->AddAction("ohcalDraw(\"ALLTRIGZS\")", "All Trigger ZS info");
  subsys->AddAction("ohcalDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction("ohcalDraw(\"NOISERMS\")", "tower prepost RMS");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("INTT", "intt");
  // subsys->AddAction("inttDraw(\"timing_okay\")", "Timing (Triggered)");
  subsys->AddAction("inttDraw(\"chip_hitmap\")", "Chip Hitmap");
  // subsys->AddAction("inttDraw(\"bco_diff\")", "BCO Diff (Triggered) [Expert]");
  subsys->AddAction("inttDraw(\"zoomed_fphx_bco\")", "Zoomed Fphx BCO (Streaming)");
  // subsys->AddAction("inttDraw(\"history\")", "Decoding Rate");
  subsys->AddAction("inttDraw(\"fphx_bco\")", "Fphx BCO (Streaming) [Expert]");
  subsys->AddAction("inttDraw(\"hitrates\")", "Hitrates [Expert]");
  subsys->AddAction("inttDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("Local Level 1", "ll1");
  subsys->AddAction("ll1Draw(\"FIRST\")", "Hits");
  subsys->AddAction("ll1Draw(\"SECOND\")", "Hits Correlation");
  subsys->AddAction("ll1Draw(\"THIRD\")", "Hits Lineup");
  subsys->AddAction("ll1Draw(\"FOURTH\")", "EMCAL LL1 - Photon");
  subsys->AddAction("ll1Draw(\"FIFTH\")", "Jet Input - HCAL");
  subsys->AddAction("ll1Draw(\"SIXTH\")", "Jet LL1");
  subsys->AddAction("ll1Draw(\"SEVENTH\")", "EMCAL 2x2 Sums");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("MVTX", "mvtx");
  subsys->AddAction("mvtxDraw(\"GENERAL\")", "General Monitor");
  subsys->AddAction("mvtxDraw(\"FEE\")", "FEE");
  subsys->AddAction("mvtxDraw(\"FHR\")", "FHR");
  subsys->AddAction("mvtxDraw(\"OCC\")", "OCC");
  subsys->AddAction("mvtxDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("SEPD", "sepd");
  subsys->AddAction("sepdDraw(\"FIRST\")", "Average ADC vs Tile (Wheel Plot)");
  subsys->AddAction("sepdDraw(\"SECOND\")", "ADC distributions");
  subsys->AddAction("sepdDraw(\"THIRD_EXP\")", "EXPERT North vs South Correlations");
  subsys->AddAction("sepdDraw(\"THIRD\")", "SHIFT CREW North vs South Correlations");
  subsys->AddAction("sepdDraw(\"FOURTH\")", "Wave Forms");
  subsys->AddAction("sepdDraw(\"FIFTH\")", "Packet Information");
  subsys->AddAction("sepdDraw(\"SIXTH\")", "Noise RMS");
  //subsys->AddAction("sepdDraw(\"SEVENTH\")", "Packet Decoder Status");
  subsys->AddAction("sepdDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  // subsys = new SubSystem("SPIN", "spin");
  // subsys->AddAction("spinDraw(\"FIRST\")", "Spin");
  // subsys->AddAction("spinDraw(\"SECOND\")", "GL1p");
  // subsys->AddAction("spinDraw(\"THIRD\")", "Abort Gap");
  // subsys->AddAction(new SubSystemActionSavePlot(subsys));
  // pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("TPC EXPERT", "tpc");
  subsys->AddAction("tpcFetchHistos()", "Fetch Fresh Tpc Histos");
  subsys->AddAction("tpcDraw(\"TPCMODULE\")", "TPC SUM[ADC]");
  subsys->AddAction("tpcDraw(\"TPCSAMPLESIZE\")", "TPC Sample Size");
  subsys->AddAction("tpcDraw(\"TPCCHECKSUMERROR\")", "TPC Checksum Error Prob.");
  subsys->AddAction("tpcDraw(\"TPCPARITYERROR\")", "TPC Parity Error Prob.");
  subsys->AddAction("tpcDraw(\"TPCADCVSSAMPLE\")", "TPC ADC vs. Sample");
  subsys->AddAction("tpcDraw(\"TPCADCVSSAMPLELARGE\")", "TPC ADC vs. Large Sample");
  subsys->AddAction("tpcDraw(\"TPCMAXADCMODULE\")", "TPC MAX10ADC-Pedestal 1D");
  subsys->AddAction("tpcDraw(\"TPCRAWADC1D\")", "TPC RAW ADC 1D");
  subsys->AddAction("tpcDraw(\"TPCPEDESTSUBADC1D\")", "TPC RAWADC-Pedestal 1D");
  subsys->AddAction("tpcDraw(\"TPCMAXADC1D\")", "TPC (WindowMAX-Pedestal) ADC 1D");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERSXYWEIGTHED\")", "TPC Cluster XY MaxADC-Pedestal,w");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERSXYUNWEIGTHED\")", "TPC Cluster XY MaxADC-Pedestal,u");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERSZYWEIGTHED\")", "TPC Cluster ZY MaxADC-Pedestal,w");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERSZYUNWEIGTHED\")", "TPC Cluster ZY MaxADC-Pedestal,u");
  subsys->AddAction("tpcDraw(\"TPCLASERCLUSTERSXYWEIGTHED\")", "TPC DIFF. LASER Cluster XY MaxADC-Pedestal,w");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERS5EXYUNWEIGTHED\")", "TPC Cluster XY <= 5 Events MaxADC-Pedestal,u");
  subsys->AddAction("tpcDraw(\"TPCCHANNELPHI_LAYER_WEIGHTED\")", "TPC ChannelPhi vs Layer vs Pedest Sub. ADC,w");
  subsys->AddAction("tpcDraw(\"TPCNEVENTSEBDC\")", "TPC NEvents vs EBDC");
  subsys->AddAction("tpcDraw(\"TPCPEDESTSUBADCVSSAMPLE\")", "TPC Pedest Sub. ADC vs Sample");
  subsys->AddAction("tpcDraw(\"TPCPEDESTSUBADCVSSAMPLE_R1\")", "TPC Pedest Sub. ADC vs Sample R1 ONLY");
  subsys->AddAction("tpcDraw(\"TPCPEDESTSUBADCVSSAMPLE_R2\")", "TPC Pedest Sub. ADC vs Sample R2 ONLY");
  subsys->AddAction("tpcDraw(\"TPCPEDESTSUBADCVSSAMPLE_R3\")", "TPC Pedest Sub. ADC vs Sample R3 ONLY");
  subsys->AddAction("tpcDraw(\"TPCNSTREAKERSVSEVENTNO\")", "TPC HORIZONTALS MONITOR ");
  subsys->AddAction("tpcDraw(\"TPCDRIFTWINDOW\")", "TPC DRIFT WINDOW PLOTS");
  subsys->AddAction("tpcDraw(\"TPCSTUCKCHANNELS\")", "TPC Stuck Channels in FEE");
  subsys->AddAction("tpcDraw(\"TPCCHANSINPACKETNS\")", "TPC NS Chan. Per Packet per RCDAQ EVENT");
  subsys->AddAction("tpcDraw(\"TPCCHANSINPACKETSS\")", "TPC SS Chan. Per Packet per RCDAQ EVENT");
  subsys->AddAction("tpcDraw(\"TPCNONZSCHANNELS\")", "TPC Non-ZS channels Per SAMPA chip");
  subsys->AddAction("tpcDraw(\"TPCZSTRIGGERADCVSSAMPLE\")", "TPC ZS trigger threshold QA");
  subsys->AddAction("tpcDraw(\"TPCFIRSTNONZSADCVSFIRSTNONZSSAMPLE\")", "TPC 1st nonZS ADC vs 1st nonZS Sample");
  subsys->AddAction("tpcDraw(\"TPCPACKETYPEFRACTION\")", "TPC PACKET TYPE WF FRACTION");
  subsys->AddAction("tpcDraw(\"TPCPACKETTYPEVSSAMPLEADC\")", "TPC PACKET TYPE vs SAMPLE wtd. by ADC");
  subsys->AddAction("tpcDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("TPC SHIFTER", "tpc");
  subsys->AddAction("tpcFetchHistos()", "Fetch Fresh Tpc Histos");
  subsys->AddAction("tpcDraw(\"SHIFTER_DRIFT_PLOT\")", "SHIFTER TPC DRIFT");
  subsys->AddAction("tpcDraw(\"TPCCLUSTERSXYWEIGTHED\")", "SHIFTER TPC ACCEPTANCE");
  subsys->AddAction("tpcDraw(\"SHIFTER_TRANSMISSION_PLOT\")", "SHIFTER TPC TRANSMISSION");
  subsys->AddAction("tpcDraw(\"TPCDCVSSAMPA\")", "SHIFTER TPC DIGITAL CURRENT");
  subsys->AddAction("tpcDraw(\"TPCCHANSPERLVL1NS\")", "SHIFTER TPC NS OCCUP.");
  subsys->AddAction("tpcDraw(\"TPCCHANSPERLVL1SS\")", "SHIFTER TPC SS OCCUP.");
  subsys->AddAction("tpcDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("TPC DIGITAL CURRENT", "tpc");
  subsys->AddAction("tpcFetchHistos()", "Fetch Fresh Tpc Histos");
  subsys->AddAction("tpcDraw(\"TPCDCVSSAMPA\")", "TPC DIGITAL CURRENT VS SAMPA");
  subsys->AddAction("tpcDraw(\"TPCDCSAMPAVSTIME\")", "TPC SAMPA VS TIME WTD. BY DC");
  subsys->AddAction("tpcDraw(\"SERVERSTATS\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);
  
  subsys = new SubSystem("TPOT", "tpot");
  subsys->AddAction("tpotDraw(\"TPOT_counts_vs_sample\")", "Counts vs Sample");
  subsys->AddAction("tpotDraw(\"TPOT_hit_charge\")", "Hit Charge");
  subsys->AddAction("tpotDraw(\"TPOT_hit_vs_channel\")", "Hit vs Strip");
  subsys->AddAction("tpotDraw(\"TPOT_counters\")", "Event counters [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_heartbeat_vs_channel\")", "Heartbit vs sampa [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_waveform_vs_channel\")", "Waveform vs Strip [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_detector_occupancy\")", "Detector Occupancy [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_resist_occupancy\")", "Resist Occupnacy [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_adc_vs_sample\")", "ADC vs Sample [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_sample_vs_channel\")", "Sample vs Channel [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_adc_vs_channel\")", "ADC vs Strip [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_hit_multiplicity\")", "Hit Multiplicity [EXPERT]");
  subsys->AddAction("tpotDraw(\"TPOT_server_stats\")", "Server Stats");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("ZDC/SMD", "zdc");
  subsys->AddAction("zdcDraw(\"FIRST\")", "ZDC 1st Monitor");
  subsys->AddAction("zdcDraw(\"SECOND\")", "ZDC 2nd Monitor");
  subsys->AddAction("zdcDraw(\"ZDC_WAVEFORM\")", "ZDC Wave form");
  subsys->AddAction("zdcDraw(\"SMDVALUES\")", "SMD Values");
  subsys->AddAction("zdcDraw(\"SMDN&S\")", "SMD North South");
  subsys->AddAction("zdcDraw(\"SMD_N_IND\")", "SMD North Individual");
  subsys->AddAction("zdcDraw(\"SMD_S_IND\")", "SMD South Individual");
  subsys->AddAction("zdcDraw(\"SMD_MULTIPLICITIES\")", "SMD Multiplicities");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);

  subsys = new SubSystem("Spin", "spin");
  subsys->AddAction("spinDraw(\"FIRST\")", "Spin");
  subsys->AddAction("spinDraw(\"SECOND\")", "GL1p");
  subsys->AddAction("spinDraw(\"THIRD\")", "Abort Gap");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);


  subsys = new SubSystem("LOCALPOL", "localpol");
  subsys->AddAction("localpolDraw(\"FIRST\")", "Asymmetries [expert]");
  subsys->AddAction("localpolDraw(\"SECOND\")", "Polarisation direction [expert]");
  subsys->AddAction("localpolDraw(\"THIRD\")", "Spin pattern [expert]");
  subsys->AddAction("localpolDraw(\"FOURTH\")", "Trigger [expert]");
  subsys->AddAction("localpolDraw(\"FIFTH\")", "SMD [expert]");
  subsys->AddAction("localpolDraw(\"SIXTH\")", "waveforms [expert]");
  subsys->AddAction("localpolDraw(\"SEVENTH\")", "ZDC/GL1 matching [expert]");
  subsys->AddAction("localpolDraw(\"EIGHTH\")", "SMD 2D distributions [expert]");
  subsys->AddAction("localpolDraw(\"NINTH\")", "SMD 2D distributions [expert]");
  subsys->AddAction(new SubSystemActionSavePlot(subsys));
  pmf->RegisterSubSystem(subsys);



  pmf->Draw();
}

void listCanvases()
{
  TSeqCollection* allCanvases = gROOT->GetListOfCanvases();
  for (int i = 0; i<allCanvases->GetEntries(); i++)
    {
      cout << allCanvases->At(i)->GetName() << endl;
    }
}

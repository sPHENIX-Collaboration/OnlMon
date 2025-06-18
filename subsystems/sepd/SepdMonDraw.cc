#include "SepdMonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TFrame.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>    // for vector

SepdMonDraw::SepdMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int SepdMonDraw::Init()
{
  return 0;
}

int SepdMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "SepdMon0")
  {
    int canvasindex = 0;

    // --- this is called by int DrawFirst(string&)
    // xpos (-1) negative: do not draw menu bar
    //TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 1 - UNDER CONSTRUCTION - Hits/Event vs Tile", -1, 0, 1200, 600);
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 1 - Hits/Event vs Tile", -1, 0, 1200, 600);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("sepdpad0", "Left", 0., 0., 0.5, 1);
    Pad[1] = new TPad("sepdpad1", "Right", 0.5, 0., 1, 1);
    Pad[0]->Draw();
    Pad[1]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "SepdMon1")
  {
    int canvasindex = 1;
    // xpos negative: do not draw menu bar
    //TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 2 - ADC Distributions", 1200, 600);
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 2 - EXPERT - ADC Distributions", -1, 0, 1600, 800);
    gSystem->ProcessEvents();
    for ( int i = 0; i < 32; ++i )
      {
        // left 4x4 is for the 16 rings in the north, the right 4x4 is for the 16 rings in the south
        //if ( i == 16 ) std::cout << "Initializing 32 pads " << std::endl;
        double xlo = (i%4)*0.125 + (i/16)*0.5;
        double xhi = xlo + 0.125;
        double ylo = 0.75 - (i/4)*0.25 + (i/16);
        double yhi = ylo + 0.25;
        //std::cout << xlo << "\t" << ylo << "\t" << xhi << "\t" << yhi << std::endl;
        adc_dist_pad[i] = new TPad(Form("adc_dist_pad_%d",i),"",xlo,ylo,xhi,yhi);
        adc_dist_pad[i]->Draw();
        adc_dist_pad[i]->SetTopMargin(0.2);
        adc_dist_pad[i]->SetLeftMargin(0.2);
      }
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "SepdMon2")
  {
    int canvasindex = 2;
    // xpos negative: do not draw menu bar
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 3 - North vs South Correlations", -1, 0, 1200, 600);
    gSystem->ProcessEvents();
    Pad[4] = new TPad("sepdpad4", "Left", 0., 0., 0.5, 1);
    Pad[5] = new TPad("sepdpad5", "Right", 0.5, 0., 1, 1);
    Pad[4]->Draw();
    Pad[5]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "SepdMon3")
  {
    int canvasindex = 3;
    //TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 4 - Waveform Info", -1, 0, xsize / 3, ysize);
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 4 - Waveform Info", -1, 0, 650, 850);
    gSystem->ProcessEvents();
    Pad[6] = new TPad("sepdpad6", "ADC vs sample #", 0.0, 0.6, 1.0, 0.95, 0);
    Pad[7] = new TPad("sepdpad7", "counts vs sample #", 0.0, 0.3, 1.0, 0.6, 0);
    Pad[8] = new TPad("sepdpad8", "pedestals", 0.0, 0.0, 1.0, 0.3, 0);
    Pad[6]->Draw();
    Pad[7]->Draw();
    Pad[8]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);
  }
  else if (name == "SepdMon4")
  {
    int canvasindex = 4;
    // xpos negative: do not draw menu bar
    //TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 5 - Packet Information", -1, 0, xsize / 3, ysize);
    //TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 5 - Packet Information", -1, 0, 1200, 850);
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 5 - EXPERT - Packet Information", -1, 0, 1200, 850);
    gSystem->ProcessEvents();
    Pad[10] = new TPad("sepdpad10", "packet event check", 0.0, 0.6, 1.0 / 2, 0.95, 0);
    Pad[11] = new TPad("sepdpad11", "packet size", 0.0, 0.3, 1.0 / 2, 0.6, 0);
    Pad[12] = new TPad("sepdpad12", "packet channels", 0.0, 0.0, 1.0 / 2, 0.3, 0);
    Pad[13] = new TPad("sepdpad13", "event number offset", 0.5, 0.6, 1.0, 0.95, 0);

    Pad[10]->Draw();
    Pad[11]->Draw();
    Pad[12]->Draw();
    Pad[13]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent1", "this does not show", 0, 0, 1., 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();

    // packet warnings
    // xlow, ylow, xup, yup
    warning[1] = new TPad("warning1", "sEPD Packet Warnings", 0.5, 0, 1, 0.65);
    warning[1]->SetFillStyle(4000);
    warning[1]->Draw();
    TC[canvasindex]->SetEditable(0);
  }
  else if (name == "SepdMon5")
  {
    int canvasindex = 5;
    TC[canvasindex] = new TCanvas(name.c_str(), "sEPD Monitor 6 - Noise RMS", -1, 0, 1200, 600);
    gSystem->ProcessEvents();
    Pad[14] = new TPad("sepdpad14", "Left", 0., 0., 0.5, 1);
    Pad[15] = new TPad("sepdpad15", "Right", 0.5, 0., 1, 1);
    Pad[14]->Draw();
    Pad[15]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "SepdServerStats")
  {
    int canvasindex = 6;
    TC[canvasindex] = new TCanvas(name.c_str(), "SepdMon Server Stats", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->Draw();
    transparent[canvasindex]->SetFillColor(kGray);
    TC[canvasindex]->SetEditable(false);
  }

  return 0;
}

int SepdMonDraw::Draw(const std::string &what)
{
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "FIRST")
  {
    iret += DrawFirst(what);
    idraw++;
  }
  if (what == "ALL" || what == "SECOND")
  {
    iret += DrawSecond(what);
    idraw++;
  }
  if (what == "ALL" || what == "THIRD")
  {
    iret += DrawThird(what);
    idraw++;
  }
  if (what == "ALL" || what == "FOURTH")
  {
    iret += DrawFourth(what);
    idraw++;
  }
  if (what == "ALL" || what == "FIFTH")
  {
    iret += DrawFifth(what);
    idraw++;
  }
  if (what == "ALL" || what == "SIXTH")
  {
    iret += DrawSixth(what);
    idraw++;
  }
  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    idraw++;
  }

  /*
    if (what == "ALL" || what == "HISTORY")
    {
      iret += DrawHistory(what);
      idraw++;
    }
    */
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}



int SepdMonDraw::DrawFirst(const std::string & /* what */)
{
  int canvasindex = 0;
  if (!gROOT->FindObject("SepdMon0"))
  {
    MakeCanvas("SepdMon0");
  }
  OnlMonClient *cl = OnlMonClient::instance();
  //TH1D *h_ADC_all_channel = cl->getHisto("SEPDMON_0", "h_ADC_all_channel");
  TH1 *h_hits_all_channel = cl->getHisto("SEPDMON_0", "h_hits_all_channel");
  TH1 *h_event = cl->getHisto("SEPDMON_0", "h_event");
  std::pair<time_t, int> evttime = cl->EventTime("CURRENT");
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");

  //if (!h_ADC_all_channel)
  if (!h_hits_all_channel)
  {
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }

  // ----------------------------
  // --- begin Rosi (mostly ) ---
  // ----------------------------

  TH2* polar_histS = new TH2F("polar_histS","polar_hist",
                               24, 0, 2*M_PI,
                               16, 0.15, 3.5);
  TH2* polar_histN = new TH2F("polar_histN","polar_hist",
                               24, 0, 2*M_PI,
                               16, 0.15, 3.5);

  //tile 0 is 2x the angular size of the rest of the tiles and needs a separate histogram
  TH2* polar_histS01 = new TH2F("polar_histS01","polar_hist",
                                 12, 0, 2*M_PI,
                                 16, 0.15, 3.5);
  TH2* polar_histN01 = new TH2F("polar_histN01","polar_hist",
                                 12, 0, 2*M_PI,
                                 16, 0.15, 3.5);

  // --- normalize
  //h_ADC_all_channel->Divide(h_hits_all_channel);
  int nevt = h_event->GetEntries();
  //h_ADC_all_channel->Scale(1.0/nevt);
  h_hits_all_channel->Scale(1.0/nevt);
  for ( int i = 0; i < 768; ++i )
    {
      int adc_channel = i;
      float adc_signal = h_hits_all_channel->GetBinContent(i+1);
      if ( adc_signal <= 0.0001 ) adc_signal = 0.0001;
      int tile = returnTile(i);
      int odd = (tile+1)%2;
      //int ring = returnRing(adc_channel);
      int sector = returnSector(adc_channel);
      int arm = returnArm(adc_channel);
      if ( arm == 0 )
        {
          if ( tile == 0 ) polar_histS01->SetBinContent(sector+1,1,adc_signal);
          else polar_histS->SetBinContent(sector*2+1+odd,(tile+1)/2+1,adc_signal);
        }
      if ( arm == 1 )
        {
          if ( tile == 0 ) polar_histN01->SetBinContent(sector+1,1,adc_signal);
          else polar_histN->SetBinContent(sector*2+1+odd,(tile+1)/2+1,adc_signal);
        }
    }

  // -------------------------
  // --- end Rosi (mostly) ---
  // -------------------------

  // --- may need to update these depending on whether there are "hot" tiles
  double zmin = 0.0;
  //double zmax = 0.1;
  double zmax = 1.0;
  //double zmax = 300;
  //double zmax = 1.1*h_ADC_all_channel->GetMaximum();

  TText tarm;
  tarm.SetNDC();
  tarm.SetTextFont(42);
  tarm.SetTextSize(0.05);

  gStyle->SetOptStat(0);
  // ---
  Pad[0]->cd();
  polar_histS->GetZaxis()->SetRangeUser(zmin,zmax);
  polar_histS01->GetZaxis()->SetRangeUser(zmin,zmax);
  // gPad->SetLeftMargin(0.2);
  // gPad->SetRightMargin(0.0);
  gPad->SetTicks(1,1);
  gPad->DrawFrame(-3.8, -3.8,3.8, 3.8);
  polar_histS->Draw("same col pol AH");
  polar_histS01->Draw("same col pol AH");
  tarm.DrawText(0.45,0.91,"South");
  gStyle->SetPalette(57);
  // ---
  Pad[1]->cd();
  polar_histN->GetZaxis()->SetRangeUser(zmin,zmax);
  polar_histN01->GetZaxis()->SetRangeUser(zmin,zmax);
  gPad->SetLeftMargin(0.05);
  gPad->SetRightMargin(0.15);
  gPad->SetTicks(1,1);
  gPad->DrawFrame(-3.8, -3.8,3.8, 3.8);
  polar_histN->Draw("same colz pol AH");
  polar_histN01->Draw("same col pol AH");
  tarm.DrawText(0.40,0.91,"North");
  //tarm.DrawText(0.35,0.91,"North");
  //tarm.DrawText(0.45,0.91,"North");
  gStyle->SetPalette(57);

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  //runnostream << "UNDER CONSTRUCTION " << ThisName << "_1 Run " << cl->RunNumber()
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}



int SepdMonDraw::DrawSecond(const std::string & /* what */)
{
  int canvasindex = 1;
  if (!gROOT->FindObject("SepdMon1"))
  {
    MakeCanvas("SepdMon1");
  }

  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *h_ADC_channel[768];
  for ( int i = 0; i < 768; ++i )
    {
      h_ADC_channel[i] = cl->getHisto("SEPDMON_0",Form("h_ADC_channel_%d",i));
    }

  TH1 *h_event = cl->getHisto("SEPDMON_0", "h_event");
  int nevt = h_event->GetEntries();
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");

  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");

  gStyle->SetPalette(kRainBow);
  TText tring;
  tring.SetNDC();
  tring.SetTextFont(42);
  tring.SetTextSize(0.1);
  TText tside;
  tside.SetNDC();
  tside.SetTextFont(42);
  tside.SetTextSize(0.1);
  for ( int i = 0; i < 768; ++i )
    {
      h_ADC_channel[i] = cl->getHisto("SEPDMON_0",Form("h_ADC_channel_%d",i));
      int tile = returnTile(i);
      if ( tile < 0 || tile > 31 ) continue;
      int odd = (tile+1)%2;
      int sector = returnSector(i);
      if ( sector < 0 || sector > 11 ) continue;
      int ring = returnRing(i);
      if ( ring < 0 || ring > 15 ) continue;
      int arm = returnArm(i);
      if ( arm < 0 || arm > 1 ) continue;
      int pad_index = ring + arm*16;
      if ( pad_index < 0 || pad_index > 31 ) continue;
      if ( adc_dist_pad[pad_index] ) adc_dist_pad[pad_index]->cd();
      // ---
      //int color = colors[tile];
      int color_index = sector;
      int skip = 255/12;
      if ( ring > 0 )
        {
          color_index = sector*2+odd;
          skip = 255/24;
        }
      int color = gStyle->GetColorPalette(color_index*skip);
      // ---
      if ( h_ADC_channel[i] )
        {
          h_ADC_channel[i]->Scale(1.0/nevt);
          h_ADC_channel[i]->GetXaxis()->SetNdivisions(505);
          h_ADC_channel[i]->GetXaxis()->SetRangeUser(0,500);
          h_ADC_channel[i]->SetMinimum(0.0);
          h_ADC_channel[i]->SetMaximum(0.003);
          h_ADC_channel[i]->SetLineColor(color);
          //h_ADC_channel[i]->SetFillColor(color);
          h_ADC_channel[i]->Draw("same hist");
        }
      else std::cout << "Missing histogram for ADC channel " << i << std::endl;
      tring.DrawText(0.55,0.7,Form("Ring %d",ring));
      if ( arm == 0 ) tring.DrawText(0.55,0.6,"South");
      if ( arm == 1 ) tring.DrawText(0.55,0.6,"North");
      gPad->SetTicky();
      gPad->SetTickx();
    }
  //std::cout << "Done drawing 768 ADC channel histograms... " << std::endl;

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << "EXPERT ONLY " << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);

  return 0;

}



int SepdMonDraw::DrawThird(const std::string & /* what */)
{
  int canvasindex = 2;
  if (!gROOT->FindObject("SepdMon2"))
  {
    MakeCanvas("SepdMon2");
  }
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *h_ADC_corr = (TH2 *) cl->getHisto("SEPDMON_0", "h_ADC_corr");
  TH2 *h_hits_corr = (TH2 *) cl->getHisto("SEPDMON_0", "h_hits_corr");
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  if (!h_ADC_corr)
  {
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }
  // --- rebin histograms
  //h_ADC_corr->Rebin2D(5,5);
  //h_hits_corr->Rebin2D(5,5);
  // ---
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  Pad[4]->cd();
  h_ADC_corr->GetYaxis()->SetNdivisions(505);
  h_ADC_corr->GetXaxis()->SetNdivisions(505);
  h_ADC_corr->GetYaxis()->SetRangeUser(0,1.5e6);
  h_ADC_corr->GetXaxis()->SetRangeUser(0,1.5e6);
  h_ADC_corr->Draw("COLZ");
  // ---
  gPad->SetLogz();
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  // ---
  Pad[5]->cd();
  h_hits_corr->GetYaxis()->SetNdivisions(505);
  h_hits_corr->GetXaxis()->SetNdivisions(505);
  h_hits_corr->GetYaxis()->SetRangeUser(0,380);
  h_hits_corr->GetXaxis()->SetRangeUser(0,380);
  h_hits_corr->Draw("COLZ");
  // ---
  gPad->SetLogz();
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  // ---
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int SepdMonDraw::DrawFourth(const std::string & /* what */)
{
  int canvasindex = 3;
  if (!gROOT->FindObject("SepdMon3"))
  {
    MakeCanvas("SepdMon3");
  }

  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *h_waveform_time = cl->getHisto("SEPDMON_0", "h1_waveform_time");
  TH1 *h_waveform_pedestal = cl->getHisto("SEPDMON_0", "h1_waveform_pedestal");
  TH2 *h2_sepd_waveform = (TH2 *) cl->getHisto("SEPDMON_0", "h2_sepd_waveform");

  //TH1 *h_event = cl->getHisto("SEPDMON_0", "h_event");
  //int nevt = h_event->GetEntries();


  TC[canvasindex]->SetEditable(1);
  TC[canvasindex]->Clear("D");
  Pad[6]->cd();
  if (!h2_sepd_waveform || !h_waveform_time || !h_waveform_pedestal)
  {
    //cout which one is not found
    if (!h2_sepd_waveform) std::cout << "h2_sepd_waveform not found" << std::endl;
    if (!h_waveform_time) std::cout << "h_waveform_time not found" << std::endl;
    if (!h_waveform_pedestal) std::cout << "h_waveform_pedestal not found" << std::endl;
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(0);
    return -1;
  }

  Pad[6]->cd();
  gStyle->SetTitleFontSize(0.03);
  float ymaxp = h2_sepd_waveform->ProfileX()->GetMaximum();
  float ymaxdraw = ymaxp * 10; // was originally 20, but that is too much
  h2_sepd_waveform->GetYaxis()->SetRangeUser(0,ymaxdraw);
  h2_sepd_waveform->GetXaxis()->SetRangeUser(0, 11);
  h2_sepd_waveform->Draw("colz");
  // --- add a profile on top
  TProfile* tp1f_sepd_waveform = h2_sepd_waveform->ProfileX();
  tp1f_sepd_waveform->SetLineColor(kBlack);
  tp1f_sepd_waveform->SetLineWidth(2);
  tp1f_sepd_waveform->Draw("same");
  // --- draw vertical lines where the waveform should be
  // x1 y1 x2 y2
  TLine* lineleft = new TLine(4.5,0,4.5,ymaxdraw);
  TLine* lineright = new TLine(7.5,0,7.5,ymaxdraw);
  lineleft->SetLineColor(kBlack);
  lineleft->SetLineWidth(2);
  lineleft->SetLineStyle(2);
  lineleft->Draw();
  lineright->SetLineColor(kBlack);
  lineright->SetLineWidth(2);
  lineright->SetLineStyle(2);
  lineright->Draw();

  float tsize = 0.09;
  //h2_sepd_waveform->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_sepd_waveform->GetXaxis()->SetNdivisions(12);
  h2_sepd_waveform->GetXaxis()->SetTitle("Sample #");
  h2_sepd_waveform->GetYaxis()->SetTitle("Waveform [ADC]");
  h2_sepd_waveform->GetXaxis()->SetLabelSize(tsize/1.15);
  h2_sepd_waveform->GetYaxis()->SetLabelSize(tsize/1.15);
  h2_sepd_waveform->GetXaxis()->SetTitleSize(tsize/1.15);
  h2_sepd_waveform->GetYaxis()->SetTitleSize(tsize/1.15);
  h2_sepd_waveform->GetXaxis()->SetTitleOffset(1.0);
  h2_sepd_waveform->GetYaxis()->SetTitleOffset(1.3);
  gPad->SetLogz();
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": Pulse fitting, Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  Pad[7]->cd();

  gStyle->SetTitleFontSize(0.06);

  h_waveform_time->GetXaxis()->SetRangeUser(0,11);
  //h_waveform_time->Scale(1.0/nevt);
  h_waveform_time->Draw("hist");
  // ---
  //h_waveform_time->GetXaxis()->SetNdivisions(510, kTRUE);
  h_waveform_time->GetXaxis()->SetNdivisions(12);
  h_waveform_time->GetXaxis()->SetTitle("Sample #");
  h_waveform_time->GetYaxis()->SetTitle("Normalized Counts");
  h_waveform_time->GetXaxis()->SetLabelSize(tsize);
  h_waveform_time->GetYaxis()->SetLabelSize(tsize);
  h_waveform_time->GetXaxis()->SetTitleSize(tsize);
  h_waveform_time->GetYaxis()->SetTitleSize(tsize);
  h_waveform_time->GetXaxis()->SetTitleOffset(1.0);
  h_waveform_time->GetYaxis()->SetTitleOffset(1.25);
  h_waveform_time->SetFillColorAlpha(kBlue, 0.1);
  if ( h_waveform_time->GetEntries() )
    {
      h_waveform_time->Scale(1.0/h_waveform_time->GetEntries());
    }
  gPad->Update();
  // draw two black lines for the okay timing range
  TLine line3(4.5, 0, 4.5, gPad->GetFrame()->GetY2());
  line3.SetLineColor(1);
  line3.SetLineWidth(3);
  line3.SetLineStyle(1);
  line3.DrawLine(4.5, 0, 4.5, gPad->GetFrame()->GetY2());
  // high line
  TLine line4(7.5, 0, 7.5, gPad->GetFrame()->GetY2());
  line4.SetLineColor(1);
  line4.SetLineWidth(3);
  line4.SetLineStyle(1);
  line4.DrawLine(7.5, 0, 7.5, gPad->GetFrame()->GetY2());
  // Draw a red line at mean x
  TLine line5(h_waveform_time->GetMean(), 0, h_waveform_time->GetMean(), gPad->GetFrame()->GetY2());
  line5.SetLineColor(2);
  line5.SetLineWidth(3);
  line5.SetLineStyle(1);
  line5.DrawLine(h_waveform_time->GetMean(), 0, h_waveform_time->GetMean(), gPad->GetFrame()->GetY2());
  // ---
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[8]->cd();

  gStyle->SetTitleFontSize(0.06);

  // x-axis range is set in SepdMon.cc, need to change there if want a wider range
  //h_waveform_pedestal->Scale(1.0/nevt);
  h_waveform_pedestal->Draw("hist");
  h_waveform_pedestal->GetXaxis()->SetNdivisions(505);
  h_waveform_pedestal->GetXaxis()->SetTitle("ADC Pedestal");
  h_waveform_pedestal->GetYaxis()->SetTitle("Normalized Counts");
  h_waveform_pedestal->GetXaxis()->SetLabelSize(tsize);
  h_waveform_pedestal->GetYaxis()->SetLabelSize(tsize);
  h_waveform_pedestal->GetXaxis()->SetTitleSize(tsize);
  h_waveform_pedestal->GetYaxis()->SetTitleSize(tsize);
  h_waveform_pedestal->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_pedestal->GetYaxis()->SetTitleOffset(1.25);
  h_waveform_pedestal->SetFillColorAlpha(kBlue, 0.1);
  if ( h_waveform_pedestal->GetEntries() )
    {
      h_waveform_pedestal->Scale(1.0/h_waveform_pedestal->GetEntries());
    }
  gPad->Update();
  TLine line6(1000, 0, 1000, gPad->GetFrame()->GetY2());
  line6.SetLineColor(1);
  line6.SetLineWidth(3);
  line6.SetLineStyle(1);
  line6.DrawLine(1000, 0, 1000, gPad->GetFrame()->GetY2());
  TLine line7(2000, 0, 2000, gPad->GetFrame()->GetY2());
  line7.SetLineColor(1);
  line7.SetLineWidth(3);
  line7.SetLineStyle(1);
  line7.DrawLine(2000, 0, 2000, gPad->GetFrame()->GetY2());
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gPad->SetLogy();
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(0);

  return 0;
}



int SepdMonDraw::DrawFifth(const std::string & /* what */)
{
  int canvasindex = 4;
  if (!gROOT->FindObject("SepdMon4"))
  {
    MakeCanvas("SepdMon4");
  }
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *h1_packet_number = cl->getHisto("SEPDMON_0", "h1_packet_number");
  TH1 *h1_packet_length = cl->getHisto("SEPDMON_0", "h1_packet_length");
  TH1 *h1_packet_chans = cl->getHisto("SEPDMON_0", "h1_packet_chans");
  TH1 *h1_packet_event = cl->getHisto("SEPDMON_0", "h1_packet_event");

  TC[canvasindex]->SetEditable(1);
  TC[canvasindex]->Clear("D");

  if (!h1_packet_number || !h1_packet_length || !h1_packet_chans || !h1_packet_event)
  {
    // print out which is not found
    if (!h1_packet_number) std::cout << "h1_packet_number not found" << std::endl;
    if (!h1_packet_length) std::cout << "h1_packet_length not found" << std::endl;
    if (!h1_packet_chans) std::cout << "h1_packet_chans not found" << std::endl;
    if (!h1_packet_event) std::cout << "h1_packet_event not found" << std::endl;

    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(0);
    return -1;
  }

  // int maxbin = h1_packet_event->GetMaximumBin();
  int maxy = h1_packet_event->GetMaximum();
  // substract all none zero bin by maxy
  for (int i = 1; i <= h1_packet_event->GetNbinsX(); i++)
  {
    if (h1_packet_event->GetBinContent(i) != 0)
    {
      h1_packet_event->SetBinContent(i, h1_packet_event->GetBinContent(i) - maxy);
    }
  }

  // find the x range for h1_packet_number
  double xmin = h1_packet_number->GetXaxis()->GetXmin();
  double xmax = h1_packet_number->GetXaxis()->GetXmax();

  TLine *one = new TLine(xmin, 1, xmax, 1);
  one->SetLineStyle(7);

  // --- Martin says 1047 for NZS
  int PACKET_SIZE = 1047;
  TLine *goodSize = new TLine(xmin, PACKET_SIZE, xmax, PACKET_SIZE);
  goodSize->SetLineStyle(7);

  // --- 128 channels per packet
  int N_CHANNELS = 128;
  TLine *goodChans = new TLine(xmin, N_CHANNELS, xmax, N_CHANNELS);
  goodChans->SetLineStyle(7);

  float param = 0.95;

  float tsize = 0.08;
  TLegend *leg = new TLegend(0.3, 0.16, 0.95, 0.4);
  leg->SetTextFont(42);
  leg->SetTextSize(tsize);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  TLine *warnLineOne = new TLine(xmin, param * 1, xmax, param * 1);
  warnLineOne->SetLineStyle(7);
  warnLineOne->SetLineColor(2);
  leg->AddEntry(warnLineOne, "95% Threshold", "l");

  // --- packet size is 1047 NZS
  TLine *warnLineSize = new TLine(xmin, param * PACKET_SIZE, xmax, param * PACKET_SIZE);
  warnLineSize->SetLineStyle(7);
  warnLineSize->SetLineColor(2);

  // --- 128 channels per packet
  TLine *warnLineChans = new TLine(xmin, param * N_CHANNELS, xmax, param * N_CHANNELS);
  warnLineChans->SetLineStyle(7);
  warnLineChans->SetLineColor(2);


  // --- this one is okay
  Pad[10]->cd();
  //float tsize = 0.08;
  h1_packet_number->GetYaxis()->SetRangeUser(0.0, 1.3);
  h1_packet_number->Draw("hist");
  one->Draw("same");
  warnLineOne->Draw("same");
  leg->Draw("same");
  h1_packet_number->GetXaxis()->SetNdivisions(6);
  h1_packet_number->GetXaxis()->SetTitle("Packet #");
  h1_packet_number->GetYaxis()->SetTitle("% Of Events Present");
  // the sizing is funny on this pad...
  h1_packet_number->GetXaxis()->SetLabelSize(tsize/1.15);
  h1_packet_number->GetYaxis()->SetLabelSize(tsize/1.15);
  h1_packet_number->GetXaxis()->SetTitleSize(tsize/1.15);
  h1_packet_number->GetYaxis()->SetTitleSize(tsize/1.15);
  h1_packet_number->GetXaxis()->SetTitleOffset(1);
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  // --- this one is okay (1047 for NZS, variable for ZS)
  Pad[11]->cd();
  h1_packet_length->Draw("hist");
  h1_packet_length->GetYaxis()->SetRangeUser(0, 1200);
  goodSize->Draw("same");
  warnLineSize->Draw("same");
  h1_packet_length->GetXaxis()->SetNdivisions(6);
  h1_packet_length->GetXaxis()->SetTitle("Packet #");
  h1_packet_length->GetYaxis()->SetTitle("Average Packet Size");
  h1_packet_length->GetXaxis()->SetLabelSize(tsize);
  h1_packet_length->GetYaxis()->SetLabelSize(tsize);
  h1_packet_length->GetXaxis()->SetTitleSize(tsize);
  h1_packet_length->GetYaxis()->SetTitleSize(tsize);
  h1_packet_length->GetXaxis()->SetTitleOffset(1);
  h1_packet_length->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  // --- this one is okay
  Pad[12]->cd();
  h1_packet_chans->Draw("hist");
  h1_packet_chans->GetYaxis()->SetRangeUser(0, 150);
  goodChans->Draw("same");
  warnLineChans->Draw("same");
  h1_packet_chans->GetXaxis()->SetNdivisions(6);
  h1_packet_chans->GetXaxis()->SetTitle("Packet #");
  h1_packet_chans->GetYaxis()->SetTitle("Average # of Channels");
  h1_packet_chans->GetXaxis()->SetLabelSize(tsize);
  h1_packet_chans->GetYaxis()->SetLabelSize(tsize);
  h1_packet_chans->GetXaxis()->SetTitleSize(tsize);
  h1_packet_chans->GetYaxis()->SetTitleSize(tsize);
  h1_packet_chans->GetXaxis()->SetTitleOffset(0.8);
  h1_packet_chans->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[13]->cd();
  //  h1_packet_event->Draw("hist");
  // h1_packet_event->SetLineColor(kWhite);;
  // h1_packet_event->Draw("AH");
  double ymax = h1_packet_event->GetMaximum();
  double ymin = h1_packet_event->GetMinimum();

  // --- this one seems okay
  h1_packet_event->GetXaxis()->SetNdivisions(6);
  h1_packet_event->GetYaxis()->SetRangeUser(ymin - 0.3 * (ymax - ymin + 30), ymax + 0.3 * (ymax - ymin + 30));
  // h1_packet_event->GetXaxis()->SetTitle("Packet #");
  // h1_packet_event->GetYaxis()->SetTitle("clock offset");
  h1_packet_event->GetXaxis()->SetLabelSize(tsize/1.2);
  h1_packet_event->GetYaxis()->SetLabelSize(tsize/1.2);
  h1_packet_event->GetXaxis()->SetTitleSize(tsize/1.2);
  h1_packet_event->GetYaxis()->SetTitleSize(tsize/1.2);
  h1_packet_event->GetXaxis()->SetTitleOffset(0.8);
  h1_packet_event->GetYaxis()->SetTitleOffset(1.2);
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();


  // draw the warning here:

  warning[1]->cd();


  std::vector<int> badPackets;
  std::string list_of_bad_packets;
  bool packet_is_bad[7];
  for (int i = 1; i <= 6; i++)
  {
    packet_is_bad[i] = false;
    bool missing = false;
    bool badnumber = false;
    //bool badlength = false;
    bool badchans = false;
    if (h1_packet_number->GetBinContent(i) == 0)
    {
      missing = true;
    }
    if (h1_packet_number->GetBinContent(i) < param)
    {
      badnumber = true;
    }
    // if (h1_packet_length->GetBinContent(i) < param * PACKET_SIZE)
    // {
    //   badlength = true;
    // }
    if (h1_packet_chans->GetBinContent(i) < param * N_CHANNELS)
    {
      badchans = true;
    }
    //if (badnumber || badlength || badchans || missing)
    if (badnumber || badchans || missing)
    {
      int the_bad_packet = (int)h1_packet_number->GetBinCenter(i);
      packet_is_bad[i] = true;
      badPackets.push_back(the_bad_packet);
      list_of_bad_packets += std::to_string(the_bad_packet);
      list_of_bad_packets += ", ";
    }
  }
  // remove the final comma and space
  if (! list_of_bad_packets.empty())
    {
      list_of_bad_packets.resize(list_of_bad_packets.size() - 2);
    }
  // --- draw the packet information
  TText PacketWarn;
  PacketWarn.SetTextFont(42);
  //PacketWarn.SetTextSize(0.04);
  PacketWarn.SetTextSize(0.05);
  PacketWarn.SetTextColor(kBlack);
  PacketWarn.SetNDC();
  //PacketWarn.SetTextAlign(23);
  PacketWarn.DrawText(0.01, 0.75, "Packet Status:");
  for (int i = 1; i <= 6; i++)
    {
      if ( packet_is_bad[i] ) PacketWarn.SetTextColor(kRed);
      // PacketWarn.DrawText(0.01, 0.7 - 0.05 * i, Form("%d: %d%% events, %d size, %d channels, %d offset", i+9000,
      //                                               int(100*h1_packet_number->GetBinContent(i)+0.5),
      //                                               (int)h1_packet_length->GetBinContent(i),
      //                                               (int)h1_packet_chans->GetBinContent(i),
      //                                               (int)h1_packet_event->GetBinContent(i)) );
      PacketWarn.DrawText(0.01, 0.7 - 0.05 * i, Form("%d: %d%% events, %d size, %d channels", i+9000,
                                                     int(100*h1_packet_number->GetBinContent(i)+0.5),
                                                     (int)h1_packet_length->GetBinContent(i),
                                                     (int)h1_packet_chans->GetBinContent(i)) );
      PacketWarn.SetTextColor(kBlack);
    }
  if ( badPackets.size() == 0 )
    {
      PacketWarn.DrawText(0.01, 0.30, Form("No bad packets, everything okay"));
    }
  if ( badPackets.size() == 1 )
    {
      PacketWarn.SetTextColor(kRed);
      PacketWarn.DrawText(0.01, 0.30, Form("%d bad packet: %s",(int)badPackets.size(),list_of_bad_packets.c_str()));
    }
  if ( badPackets.size() > 1 )
    {
      PacketWarn.SetTextColor(kRed);
      PacketWarn.DrawText(0.01, 0.30, Form("%d bad packets: %s",(int)badPackets.size(),list_of_bad_packets.c_str()));
    }


  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.02);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::ostringstream runnostream2;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string

  // runnostream << "EXPERT ONLY " << ThisName << "_2 Run " << cl->RunNumber()
  //             << ", Time: " << ctime(&evttime.first);
  // runnostream << "Packet Information";
  // runnostream2 << " Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);
  runnostream << "EXPERT ONLY Packet Information";
  runnostream2 << " Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);
  transparent[canvasindex]->cd();

  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, .99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, .966, runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(0);
  // if (save) TC[canvasindex]->SaveAs("plots/packets.pdf");
  return 0;
}

int SepdMonDraw::DrawSixth(const std::string & /* what */)
{
  int canvasindex = 5;
  if (!gROOT->FindObject("SepdMon5"))
  {
    MakeCanvas("SepdMon5");
  }
  OnlMonClient *cl = OnlMonClient::instance();
  TProfile *profile_rms = (TProfile*) cl->getHisto("SEPDMON_0", "p_noiserms_all_channel");
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  if (!profile_rms)
  {
    std::cout << "p_noiserms_all_channel not found" << std::endl;
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }
  
  std::pair<time_t, int> evttime = cl->EventTime("CURRENT");

  TH2* polar_histS_rms = new TH2F("polar_histS_rms","polar_hist_rms",
                               24, 0, 2*M_PI,
                               16, 0.15, 3.5);
  TH2* polar_histN_rms = new TH2F("polar_histN_rms","polar_hist_rms",
                               24, 0, 2*M_PI,
                               16, 0.15, 3.5);
  
  TH2* polar_histS01_rms = new TH2F("polar_histS01_rms","polar_hist_rms",
                                 12, 0, 2*M_PI,
                                 16, 0.15, 3.5);
  TH2* polar_histN01_rms = new TH2F("polar_histN01_rms","polar_hist_rms",
                                 12, 0, 2*M_PI,
                                 16, 0.15, 3.5);

  // --- fill polar
  for ( int i = 0; i < 768; ++i )
    {
      int adc_channel = i;
      float noiserms = profile_rms->GetBinError(i+1);
      int tile = returnTile(i);
      int odd = (tile+1)%2;
      //int ring = returnRing(adc_channel);
      int sector = returnSector(adc_channel);
      int arm = returnArm(adc_channel);
      if ( arm == 0 )
        {
          if ( tile == 0 ) polar_histS01_rms->SetBinContent(sector+1,1,noiserms);
          else polar_histS_rms->SetBinContent(sector*2+1+odd,(tile+1)/2+1,noiserms);
        }
      if ( arm == 1 )
        {
          if ( tile == 0 ) polar_histN01_rms->SetBinContent(sector+1,1,noiserms);
          else polar_histN_rms->SetBinContent(sector*2+1+odd,(tile+1)/2+1,noiserms);
        }
    }

  // -------------------------
  // --- end Rosi (mostly) ---
  // -------------------------

  // --- may need to update these depending on whether there are "hot" tiles
  double zmin = 0.0;
  //double zmax = 0.1;
  double zmax = 100.0;
  //double zmax = 300;
  //double zmax = 1.1*h_ADC_all_channel->GetMaximum();

  TText tarm;
  tarm.SetNDC();
  tarm.SetTextFont(42);
  tarm.SetTextSize(0.05);

  gStyle->SetOptStat(0);
  // ---
  Pad[14]->cd();
  polar_histS_rms->GetZaxis()->SetRangeUser(zmin,zmax);
  polar_histS01_rms->GetZaxis()->SetRangeUser(zmin,zmax);
  // gPad->SetLeftMargin(0.2);
  // gPad->SetRightMargin(0.0);
  gPad->SetTicks(1,1);
  gPad->DrawFrame(-3.8, -3.8,3.8, 3.8);
  polar_histS_rms->Draw("same col pol AH");
  polar_histS01_rms->Draw("same col pol AH");
  tarm.DrawText(0.45,0.91,"South");
  gStyle->SetPalette(57);
  // ---
  Pad[15]->cd();
  polar_histN_rms->GetZaxis()->SetRangeUser(zmin,zmax);
  polar_histN01_rms->GetZaxis()->SetRangeUser(zmin,zmax);
  gPad->SetLeftMargin(0.05);
  gPad->SetRightMargin(0.15);
  gPad->SetTicks(1,1);
  gPad->DrawFrame(-3.8, -3.8,3.8, 3.8);
  polar_histN_rms->Draw("same colz pol AH");
  polar_histN01_rms->Draw("same col pol AH");
  tarm.DrawText(0.40,0.91,"North");
  //tarm.DrawText(0.35,0.91,"North");
  //tarm.DrawText(0.45,0.91,"North");
  gStyle->SetPalette(57);

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  //runnostream << "UNDER CONSTRUCTION " << ThisName << "_1 Run " << cl->RunNumber()
  runnostream << ThisName << "_6 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}




int SepdMonDraw::SavePlot(const std::string &what, const std::string &type)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
    return iret;
  }
  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" +
                           std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}



int SepdMonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient *cl = OnlMonClient::instance();
  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    // Register the canvas png file to the menu and produces the png file.
    std::string pngfile = cl->htmlRegisterPage(*this, canvas->GetTitle(), std::to_string(icnt), "png");
    cl->CanvasToPng(canvas, pngfile);
  }

  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  // std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
  // std::ofstream out(logfile.c_str());
  // out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
  //     << "</TITLE></HEAD>" << std::endl;
  // out << "<P>Some log file output would go here." << std::endl;
  // out.close();

  // std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
  // std::ofstream out2(status.c_str());
  // out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
  //      << "</TITLE></HEAD>" << std::endl;
  // out2 << "<P>Some status output would go here." << std::endl;
  // out2.close();
  // cl->SaveLogFile(*this);
  return 0;
}

int SepdMonDraw::returnRing(int ch)
{
  // 16 rings total, 0 is innermost, 15 is outermost
  // ring 0 is tile 0
  // ring 1 is tiles 1 and 2
  // ring 2 is tiles 3 and 4
  // etc
  int tile = returnTile(ch);
  int ring = (tile+1)/2;
  return ring;
}

int SepdMonDraw::returnArm(int ch)
{
  // 0 is south, 1 is north
  if ( ch >= 384 && ch <= 767 ) return 0;
  else if ( ch <= 383 && ch >= 0 ) return 1;
  else return -1;
}

int SepdMonDraw::returnSector(int ch){
    int sEPD_adctosector[768];
    sEPD_adctosector[0] = 7;
    sEPD_adctosector[1] = 7;
    sEPD_adctosector[2] = 7;
    sEPD_adctosector[3] = 7;
    sEPD_adctosector[4] = 7;
    sEPD_adctosector[5] = 7;
    sEPD_adctosector[6] = 7;
    sEPD_adctosector[7] = 7;
    sEPD_adctosector[8] = 7;
    sEPD_adctosector[9] = 7;
    sEPD_adctosector[10] = 7;
    sEPD_adctosector[11] = 7;
    sEPD_adctosector[12] = 7;
    sEPD_adctosector[13] = 7;
    sEPD_adctosector[14] = 7;
    sEPD_adctosector[15] = 7;
    sEPD_adctosector[16] = 7;
    sEPD_adctosector[17] = 7;
    sEPD_adctosector[18] = 7;
    sEPD_adctosector[19] = 7;
    sEPD_adctosector[20] = 7;
    sEPD_adctosector[21] = 7;
    sEPD_adctosector[22] = 7;
    sEPD_adctosector[23] = 7;
    sEPD_adctosector[24] = 7;
    sEPD_adctosector[25] = 7;
    sEPD_adctosector[26] = 7;
    sEPD_adctosector[27] = 7;
    sEPD_adctosector[28] = 7;
    sEPD_adctosector[29] = 7;
    sEPD_adctosector[30] = 7;
    sEPD_adctosector[31] = 7;
    sEPD_adctosector[32] = 6;
    sEPD_adctosector[33] = 6;
    sEPD_adctosector[34] = 6;
    sEPD_adctosector[35] = 6;
    sEPD_adctosector[36] = 6;
    sEPD_adctosector[37] = 6;
    sEPD_adctosector[38] = 6;
    sEPD_adctosector[39] = 6;
    sEPD_adctosector[40] = 6;
    sEPD_adctosector[41] = 6;
    sEPD_adctosector[42] = 6;
    sEPD_adctosector[43] = 6;
    sEPD_adctosector[44] = 6;
    sEPD_adctosector[45] = 6;
    sEPD_adctosector[46] = 6;
    sEPD_adctosector[47] = 6;
    sEPD_adctosector[48] = 6;
    sEPD_adctosector[49] = 6;
    sEPD_adctosector[50] = 6;
    sEPD_adctosector[51] = 6;
    sEPD_adctosector[52] = 6;
    sEPD_adctosector[53] = 6;
    sEPD_adctosector[54] = 6;
    sEPD_adctosector[55] = 6;
    sEPD_adctosector[56] = 6;
    sEPD_adctosector[57] = 6;
    sEPD_adctosector[58] = 6;
    sEPD_adctosector[59] = 6;
    sEPD_adctosector[60] = 6;
    sEPD_adctosector[61] = 6;
    sEPD_adctosector[62] = 6;
    sEPD_adctosector[63] = 6;
    sEPD_adctosector[64] = 8;
    sEPD_adctosector[65] = 8;
    sEPD_adctosector[66] = 8;
    sEPD_adctosector[67] = 8;
    sEPD_adctosector[68] = 8;
    sEPD_adctosector[69] = 8;
    sEPD_adctosector[70] = 8;
    sEPD_adctosector[71] = 8;
    sEPD_adctosector[72] = 8;
    sEPD_adctosector[73] = 8;
    sEPD_adctosector[74] = 8;
    sEPD_adctosector[75] = 8;
    sEPD_adctosector[76] = 8;
    sEPD_adctosector[77] = 8;
    sEPD_adctosector[78] = 8;
    sEPD_adctosector[79] = 8;
    sEPD_adctosector[80] = 8;
    sEPD_adctosector[81] = 8;
    sEPD_adctosector[82] = 8;
    sEPD_adctosector[83] = 8;
    sEPD_adctosector[84] = 8;
    sEPD_adctosector[85] = 8;
    sEPD_adctosector[86] = 8;
    sEPD_adctosector[87] = 8;
    sEPD_adctosector[88] = 8;
    sEPD_adctosector[89] = 8;
    sEPD_adctosector[90] = 8;
    sEPD_adctosector[91] = 8;
    sEPD_adctosector[92] = 8;
    sEPD_adctosector[93] = 8;
    sEPD_adctosector[94] = 8;
    sEPD_adctosector[95] = 8;
    sEPD_adctosector[96] = 5;
    sEPD_adctosector[97] = 5;
    sEPD_adctosector[98] = 5;
    sEPD_adctosector[99] = 5;
    sEPD_adctosector[100] = 5;
    sEPD_adctosector[101] = 5;
    sEPD_adctosector[102] = 5;
    sEPD_adctosector[103] = 5;
    sEPD_adctosector[104] = 5;
    sEPD_adctosector[105] = 5;
    sEPD_adctosector[106] = 5;
    sEPD_adctosector[107] = 5;
    sEPD_adctosector[108] = 5;
    sEPD_adctosector[109] = 5;
    sEPD_adctosector[110] = 5;
    sEPD_adctosector[111] = 5;
    sEPD_adctosector[112] = 5;
    sEPD_adctosector[113] = 5;
    sEPD_adctosector[114] = 5;
    sEPD_adctosector[115] = 5;
    sEPD_adctosector[116] = 5;
    sEPD_adctosector[117] = 5;
    sEPD_adctosector[118] = 5;
    sEPD_adctosector[119] = 5;
    sEPD_adctosector[120] = 5;
    sEPD_adctosector[121] = 5;
    sEPD_adctosector[122] = 5;
    sEPD_adctosector[123] = 5;
    sEPD_adctosector[124] = 5;
    sEPD_adctosector[125] = 5;
    sEPD_adctosector[126] = 5;
    sEPD_adctosector[127] = 5;
    sEPD_adctosector[128] = 9;
    sEPD_adctosector[129] = 9;
    sEPD_adctosector[130] = 9;
    sEPD_adctosector[131] = 9;
    sEPD_adctosector[132] = 9;
    sEPD_adctosector[133] = 9;
    sEPD_adctosector[134] = 9;
    sEPD_adctosector[135] = 9;
    sEPD_adctosector[136] = 9;
    sEPD_adctosector[137] = 9;
    sEPD_adctosector[138] = 9;
    sEPD_adctosector[139] = 9;
    sEPD_adctosector[140] = 9;
    sEPD_adctosector[141] = 9;
    sEPD_adctosector[142] = 9;
    sEPD_adctosector[143] = 9;
    sEPD_adctosector[144] = 9;
    sEPD_adctosector[145] = 9;
    sEPD_adctosector[146] = 9;
    sEPD_adctosector[147] = 9;
    sEPD_adctosector[148] = 9;
    sEPD_adctosector[149] = 9;
    sEPD_adctosector[150] = 9;
    sEPD_adctosector[151] = 9;
    sEPD_adctosector[152] = 9;
    sEPD_adctosector[153] = 9;
    sEPD_adctosector[154] = 9;
    sEPD_adctosector[155] = 9;
    sEPD_adctosector[156] = 9;
    sEPD_adctosector[157] = 9;
    sEPD_adctosector[158] = 9;
    sEPD_adctosector[159] = 9;
    sEPD_adctosector[160] = 10;
    sEPD_adctosector[161] = 10;
    sEPD_adctosector[162] = 10;
    sEPD_adctosector[163] = 10;
    sEPD_adctosector[164] = 10;
    sEPD_adctosector[165] = 10;
    sEPD_adctosector[166] = 10;
    sEPD_adctosector[167] = 10;
    sEPD_adctosector[168] = 10;
    sEPD_adctosector[169] = 10;
    sEPD_adctosector[170] = 10;
    sEPD_adctosector[171] = 10;
    sEPD_adctosector[172] = 10;
    sEPD_adctosector[173] = 10;
    sEPD_adctosector[174] = 10;
    sEPD_adctosector[175] = 10;
    sEPD_adctosector[176] = 10;
    sEPD_adctosector[177] = 10;
    sEPD_adctosector[178] = 10;
    sEPD_adctosector[179] = 10;
    sEPD_adctosector[180] = 10;
    sEPD_adctosector[181] = 10;
    sEPD_adctosector[182] = 10;
    sEPD_adctosector[183] = 10;
    sEPD_adctosector[184] = 10;
    sEPD_adctosector[185] = 10;
    sEPD_adctosector[186] = 10;
    sEPD_adctosector[187] = 10;
    sEPD_adctosector[188] = 10;
    sEPD_adctosector[189] = 10;
    sEPD_adctosector[190] = 10;
    sEPD_adctosector[191] = 10;
    sEPD_adctosector[192] = 4;
    sEPD_adctosector[193] = 4;
    sEPD_adctosector[194] = 4;
    sEPD_adctosector[195] = 4;
    sEPD_adctosector[196] = 4;
    sEPD_adctosector[197] = 4;
    sEPD_adctosector[198] = 4;
    sEPD_adctosector[199] = 4;
    sEPD_adctosector[200] = 4;
    sEPD_adctosector[201] = 4;
    sEPD_adctosector[202] = 4;
    sEPD_adctosector[203] = 4;
    sEPD_adctosector[204] = 4;
    sEPD_adctosector[205] = 4;
    sEPD_adctosector[206] = 4;
    sEPD_adctosector[207] = 4;
    sEPD_adctosector[208] = 4;
    sEPD_adctosector[209] = 4;
    sEPD_adctosector[210] = 4;
    sEPD_adctosector[211] = 4;
    sEPD_adctosector[212] = 4;
    sEPD_adctosector[213] = 4;
    sEPD_adctosector[214] = 4;
    sEPD_adctosector[215] = 4;
    sEPD_adctosector[216] = 4;
    sEPD_adctosector[217] = 4;
    sEPD_adctosector[218] = 4;
    sEPD_adctosector[219] = 4;
    sEPD_adctosector[220] = 4;
    sEPD_adctosector[221] = 4;
    sEPD_adctosector[222] = 4;
    sEPD_adctosector[223] = 4;
    sEPD_adctosector[224] = 3;
    sEPD_adctosector[225] = 3;
    sEPD_adctosector[226] = 3;
    sEPD_adctosector[227] = 3;
    sEPD_adctosector[228] = 3;
    sEPD_adctosector[229] = 3;
    sEPD_adctosector[230] = 3;
    sEPD_adctosector[231] = 3;
    sEPD_adctosector[232] = 3;
    sEPD_adctosector[233] = 3;
    sEPD_adctosector[234] = 3;
    sEPD_adctosector[235] = 3;
    sEPD_adctosector[236] = 3;
    sEPD_adctosector[237] = 3;
    sEPD_adctosector[238] = 3;
    sEPD_adctosector[239] = 3;
    sEPD_adctosector[240] = 3;
    sEPD_adctosector[241] = 3;
    sEPD_adctosector[242] = 3;
    sEPD_adctosector[243] = 3;
    sEPD_adctosector[244] = 3;
    sEPD_adctosector[245] = 3;
    sEPD_adctosector[246] = 3;
    sEPD_adctosector[247] = 3;
    sEPD_adctosector[248] = 3;
    sEPD_adctosector[249] = 3;
    sEPD_adctosector[250] = 3;
    sEPD_adctosector[251] = 3;
    sEPD_adctosector[252] = 3;
    sEPD_adctosector[253] = 3;
    sEPD_adctosector[254] = 3;
    sEPD_adctosector[255] = 3;
    sEPD_adctosector[256] = 0;
    sEPD_adctosector[257] = 0;
    sEPD_adctosector[258] = 0;
    sEPD_adctosector[259] = 0;
    sEPD_adctosector[260] = 0;
    sEPD_adctosector[261] = 0;
    sEPD_adctosector[262] = 0;
    sEPD_adctosector[263] = 0;
    sEPD_adctosector[264] = 0;
    sEPD_adctosector[265] = 0;
    sEPD_adctosector[266] = 0;
    sEPD_adctosector[267] = 0;
    sEPD_adctosector[268] = 0;
    sEPD_adctosector[269] = 0;
    sEPD_adctosector[270] = 0;
    sEPD_adctosector[271] = 0;
    sEPD_adctosector[272] = 0;
    sEPD_adctosector[273] = 0;
    sEPD_adctosector[274] = 0;
    sEPD_adctosector[275] = 0;
    sEPD_adctosector[276] = 0;
    sEPD_adctosector[277] = 0;
    sEPD_adctosector[278] = 0;
    sEPD_adctosector[279] = 0;
    sEPD_adctosector[280] = 0;
    sEPD_adctosector[281] = 0;
    sEPD_adctosector[282] = 0;
    sEPD_adctosector[283] = 0;
    sEPD_adctosector[284] = 0;
    sEPD_adctosector[285] = 0;
    sEPD_adctosector[286] = 0;
    sEPD_adctosector[287] = 0;
    sEPD_adctosector[288] = 11;
    sEPD_adctosector[289] = 11;
    sEPD_adctosector[290] = 11;
    sEPD_adctosector[291] = 11;
    sEPD_adctosector[292] = 11;
    sEPD_adctosector[293] = 11;
    sEPD_adctosector[294] = 11;
    sEPD_adctosector[295] = 11;
    sEPD_adctosector[296] = 11;
    sEPD_adctosector[297] = 11;
    sEPD_adctosector[298] = 11;
    sEPD_adctosector[299] = 11;
    sEPD_adctosector[300] = 11;
    sEPD_adctosector[301] = 11;
    sEPD_adctosector[302] = 11;
    sEPD_adctosector[303] = 11;
    sEPD_adctosector[304] = 11;
    sEPD_adctosector[305] = 11;
    sEPD_adctosector[306] = 11;
    sEPD_adctosector[307] = 11;
    sEPD_adctosector[308] = 11;
    sEPD_adctosector[309] = 11;
    sEPD_adctosector[310] = 11;
    sEPD_adctosector[311] = 11;
    sEPD_adctosector[312] = 11;
    sEPD_adctosector[313] = 11;
    sEPD_adctosector[314] = 11;
    sEPD_adctosector[315] = 11;
    sEPD_adctosector[316] = 11;
    sEPD_adctosector[317] = 11;
    sEPD_adctosector[318] = 11;
    sEPD_adctosector[319] = 11;
    sEPD_adctosector[320] = 2;
    sEPD_adctosector[321] = 2;
    sEPD_adctosector[322] = 2;
    sEPD_adctosector[323] = 2;
    sEPD_adctosector[324] = 2;
    sEPD_adctosector[325] = 2;
    sEPD_adctosector[326] = 2;
    sEPD_adctosector[327] = 2;
    sEPD_adctosector[328] = 2;
    sEPD_adctosector[329] = 2;
    sEPD_adctosector[330] = 2;
    sEPD_adctosector[331] = 2;
    sEPD_adctosector[332] = 2;
    sEPD_adctosector[333] = 2;
    sEPD_adctosector[334] = 2;
    sEPD_adctosector[335] = 2;
    sEPD_adctosector[336] = 2;
    sEPD_adctosector[337] = 2;
    sEPD_adctosector[338] = 2;
    sEPD_adctosector[339] = 2;
    sEPD_adctosector[340] = 2;
    sEPD_adctosector[341] = 2;
    sEPD_adctosector[342] = 2;
    sEPD_adctosector[343] = 2;
    sEPD_adctosector[344] = 2;
    sEPD_adctosector[345] = 2;
    sEPD_adctosector[346] = 2;
    sEPD_adctosector[347] = 2;
    sEPD_adctosector[348] = 2;
    sEPD_adctosector[349] = 2;
    sEPD_adctosector[350] = 2;
    sEPD_adctosector[351] = 2;
    sEPD_adctosector[352] = 1;
    sEPD_adctosector[353] = 1;
    sEPD_adctosector[354] = 1;
    sEPD_adctosector[355] = 1;
    sEPD_adctosector[356] = 1;
    sEPD_adctosector[357] = 1;
    sEPD_adctosector[358] = 1;
    sEPD_adctosector[359] = 1;
    sEPD_adctosector[360] = 1;
    sEPD_adctosector[361] = 1;
    sEPD_adctosector[362] = 1;
    sEPD_adctosector[363] = 1;
    sEPD_adctosector[364] = 1;
    sEPD_adctosector[365] = 1;
    sEPD_adctosector[366] = 1;
    sEPD_adctosector[367] = 1;
    sEPD_adctosector[368] = 1;
    sEPD_adctosector[369] = 1;
    sEPD_adctosector[370] = 1;
    sEPD_adctosector[371] = 1;
    sEPD_adctosector[372] = 1;
    sEPD_adctosector[373] = 1;
    sEPD_adctosector[374] = 1;
    sEPD_adctosector[375] = 1;
    sEPD_adctosector[376] = 1;
    sEPD_adctosector[377] = 1;
    sEPD_adctosector[378] = 1;
    sEPD_adctosector[379] = 1;
    sEPD_adctosector[380] = 1;
    sEPD_adctosector[381] = 1;
    sEPD_adctosector[382] = 1;
    sEPD_adctosector[383] = 1;
    sEPD_adctosector[384] = 2;
    sEPD_adctosector[385] = 2;
    sEPD_adctosector[386] = 2;
    sEPD_adctosector[387] = 2;
    sEPD_adctosector[388] = 2;
    sEPD_adctosector[389] = 2;
    sEPD_adctosector[390] = 2;
    sEPD_adctosector[391] = 2;
    sEPD_adctosector[392] = 2;
    sEPD_adctosector[393] = 2;
    sEPD_adctosector[394] = 2;
    sEPD_adctosector[395] = 2;
    sEPD_adctosector[396] = 2;
    sEPD_adctosector[397] = 2;
    sEPD_adctosector[398] = 2;
    sEPD_adctosector[399] = 2;
    sEPD_adctosector[400] = 2;
    sEPD_adctosector[401] = 2;
    sEPD_adctosector[402] = 2;
    sEPD_adctosector[403] = 2;
    sEPD_adctosector[404] = 2;
    sEPD_adctosector[405] = 2;
    sEPD_adctosector[406] = 2;
    sEPD_adctosector[407] = 2;
    sEPD_adctosector[408] = 2;
    sEPD_adctosector[409] = 2;
    sEPD_adctosector[410] = 2;
    sEPD_adctosector[411] = 2;
    sEPD_adctosector[412] = 2;
    sEPD_adctosector[413] = 2;
    sEPD_adctosector[414] = 2;
    sEPD_adctosector[415] = 2;
    sEPD_adctosector[416] = 1;
    sEPD_adctosector[417] = 1;
    sEPD_adctosector[418] = 1;
    sEPD_adctosector[419] = 1;
    sEPD_adctosector[420] = 1;
    sEPD_adctosector[421] = 1;
    sEPD_adctosector[422] = 1;
    sEPD_adctosector[423] = 1;
    sEPD_adctosector[424] = 1;
    sEPD_adctosector[425] = 1;
    sEPD_adctosector[426] = 1;
    sEPD_adctosector[427] = 1;
    sEPD_adctosector[428] = 1;
    sEPD_adctosector[429] = 1;
    sEPD_adctosector[430] = 1;
    sEPD_adctosector[431] = 1;
    sEPD_adctosector[432] = 1;
    sEPD_adctosector[433] = 1;
    sEPD_adctosector[434] = 1;
    sEPD_adctosector[435] = 1;
    sEPD_adctosector[436] = 1;
    sEPD_adctosector[437] = 1;
    sEPD_adctosector[438] = 1;
    sEPD_adctosector[439] = 1;
    sEPD_adctosector[440] = 1;
    sEPD_adctosector[441] = 1;
    sEPD_adctosector[442] = 1;
    sEPD_adctosector[443] = 1;
    sEPD_adctosector[444] = 1;
    sEPD_adctosector[445] = 1;
    sEPD_adctosector[446] = 1;
    sEPD_adctosector[447] = 1;
    sEPD_adctosector[448] = 6;
    sEPD_adctosector[449] = 6;
    sEPD_adctosector[450] = 6;
    sEPD_adctosector[451] = 6;
    sEPD_adctosector[452] = 6;
    sEPD_adctosector[453] = 6;
    sEPD_adctosector[454] = 6;
    sEPD_adctosector[455] = 6;
    sEPD_adctosector[456] = 6;
    sEPD_adctosector[457] = 6;
    sEPD_adctosector[458] = 6;
    sEPD_adctosector[459] = 6;
    sEPD_adctosector[460] = 6;
    sEPD_adctosector[461] = 6;
    sEPD_adctosector[462] = 6;
    sEPD_adctosector[463] = 6;
    sEPD_adctosector[464] = 6;
    sEPD_adctosector[465] = 6;
    sEPD_adctosector[466] = 6;
    sEPD_adctosector[467] = 6;
    sEPD_adctosector[468] = 6;
    sEPD_adctosector[469] = 6;
    sEPD_adctosector[470] = 6;
    sEPD_adctosector[471] = 6;
    sEPD_adctosector[472] = 6;
    sEPD_adctosector[473] = 6;
    sEPD_adctosector[474] = 6;
    sEPD_adctosector[475] = 6;
    sEPD_adctosector[476] = 6;
    sEPD_adctosector[477] = 6;
    sEPD_adctosector[478] = 6;
    sEPD_adctosector[479] = 6;
    sEPD_adctosector[480] = 7;
    sEPD_adctosector[481] = 7;
    sEPD_adctosector[482] = 7;
    sEPD_adctosector[483] = 7;
    sEPD_adctosector[484] = 7;
    sEPD_adctosector[485] = 7;
    sEPD_adctosector[486] = 7;
    sEPD_adctosector[487] = 7;
    sEPD_adctosector[488] = 7;
    sEPD_adctosector[489] = 7;
    sEPD_adctosector[490] = 7;
    sEPD_adctosector[491] = 7;
    sEPD_adctosector[492] = 7;
    sEPD_adctosector[493] = 7;
    sEPD_adctosector[494] = 7;
    sEPD_adctosector[495] = 7;
    sEPD_adctosector[496] = 7;
    sEPD_adctosector[497] = 7;
    sEPD_adctosector[498] = 7;
    sEPD_adctosector[499] = 7;
    sEPD_adctosector[500] = 7;
    sEPD_adctosector[501] = 7;
    sEPD_adctosector[502] = 7;
    sEPD_adctosector[503] = 7;
    sEPD_adctosector[504] = 7;
    sEPD_adctosector[505] = 7;
    sEPD_adctosector[506] = 7;
    sEPD_adctosector[507] = 7;
    sEPD_adctosector[508] = 7;
    sEPD_adctosector[509] = 7;
    sEPD_adctosector[510] = 7;
    sEPD_adctosector[511] = 7;
    sEPD_adctosector[512] = 4;
    sEPD_adctosector[513] = 4;
    sEPD_adctosector[514] = 4;
    sEPD_adctosector[515] = 4;
    sEPD_adctosector[516] = 4;
    sEPD_adctosector[517] = 4;
    sEPD_adctosector[518] = 4;
    sEPD_adctosector[519] = 4;
    sEPD_adctosector[520] = 4;
    sEPD_adctosector[521] = 4;
    sEPD_adctosector[522] = 4;
    sEPD_adctosector[523] = 4;
    sEPD_adctosector[524] = 4;
    sEPD_adctosector[525] = 4;
    sEPD_adctosector[526] = 4;
    sEPD_adctosector[527] = 4;
    sEPD_adctosector[528] = 4;
    sEPD_adctosector[529] = 4;
    sEPD_adctosector[530] = 4;
    sEPD_adctosector[531] = 4;
    sEPD_adctosector[532] = 4;
    sEPD_adctosector[533] = 4;
    sEPD_adctosector[534] = 4;
    sEPD_adctosector[535] = 4;
    sEPD_adctosector[536] = 4;
    sEPD_adctosector[537] = 4;
    sEPD_adctosector[538] = 4;
    sEPD_adctosector[539] = 4;
    sEPD_adctosector[540] = 4;
    sEPD_adctosector[541] = 4;
    sEPD_adctosector[542] = 4;
    sEPD_adctosector[543] = 4;
    sEPD_adctosector[544] = 3;
    sEPD_adctosector[545] = 3;
    sEPD_adctosector[546] = 3;
    sEPD_adctosector[547] = 3;
    sEPD_adctosector[548] = 3;
    sEPD_adctosector[549] = 3;
    sEPD_adctosector[550] = 3;
    sEPD_adctosector[551] = 3;
    sEPD_adctosector[552] = 3;
    sEPD_adctosector[553] = 3;
    sEPD_adctosector[554] = 3;
    sEPD_adctosector[555] = 3;
    sEPD_adctosector[556] = 3;
    sEPD_adctosector[557] = 3;
    sEPD_adctosector[558] = 3;
    sEPD_adctosector[559] = 3;
    sEPD_adctosector[560] = 3;
    sEPD_adctosector[561] = 3;
    sEPD_adctosector[562] = 3;
    sEPD_adctosector[563] = 3;
    sEPD_adctosector[564] = 3;
    sEPD_adctosector[565] = 3;
    sEPD_adctosector[566] = 3;
    sEPD_adctosector[567] = 3;
    sEPD_adctosector[568] = 3;
    sEPD_adctosector[569] = 3;
    sEPD_adctosector[570] = 3;
    sEPD_adctosector[571] = 3;
    sEPD_adctosector[572] = 3;
    sEPD_adctosector[573] = 3;
    sEPD_adctosector[574] = 3;
    sEPD_adctosector[575] = 3;
    sEPD_adctosector[576] = 11;
    sEPD_adctosector[577] = 11;
    sEPD_adctosector[578] = 11;
    sEPD_adctosector[579] = 11;
    sEPD_adctosector[580] = 11;
    sEPD_adctosector[581] = 11;
    sEPD_adctosector[582] = 11;
    sEPD_adctosector[583] = 11;
    sEPD_adctosector[584] = 11;
    sEPD_adctosector[585] = 11;
    sEPD_adctosector[586] = 11;
    sEPD_adctosector[587] = 11;
    sEPD_adctosector[588] = 11;
    sEPD_adctosector[589] = 11;
    sEPD_adctosector[590] = 11;
    sEPD_adctosector[591] = 11;
    sEPD_adctosector[592] = 11;
    sEPD_adctosector[593] = 11;
    sEPD_adctosector[594] = 11;
    sEPD_adctosector[595] = 11;
    sEPD_adctosector[596] = 11;
    sEPD_adctosector[597] = 11;
    sEPD_adctosector[598] = 11;
    sEPD_adctosector[599] = 11;
    sEPD_adctosector[600] = 11;
    sEPD_adctosector[601] = 11;
    sEPD_adctosector[602] = 11;
    sEPD_adctosector[603] = 11;
    sEPD_adctosector[604] = 11;
    sEPD_adctosector[605] = 11;
    sEPD_adctosector[606] = 11;
    sEPD_adctosector[607] = 11;
    sEPD_adctosector[608] = 0;
    sEPD_adctosector[609] = 0;
    sEPD_adctosector[610] = 0;
    sEPD_adctosector[611] = 0;
    sEPD_adctosector[612] = 0;
    sEPD_adctosector[613] = 0;
    sEPD_adctosector[614] = 0;
    sEPD_adctosector[615] = 0;
    sEPD_adctosector[616] = 0;
    sEPD_adctosector[617] = 0;
    sEPD_adctosector[618] = 0;
    sEPD_adctosector[619] = 0;
    sEPD_adctosector[620] = 0;
    sEPD_adctosector[621] = 0;
    sEPD_adctosector[622] = 0;
    sEPD_adctosector[623] = 0;
    sEPD_adctosector[624] = 0;
    sEPD_adctosector[625] = 0;
    sEPD_adctosector[626] = 0;
    sEPD_adctosector[627] = 0;
    sEPD_adctosector[628] = 0;
    sEPD_adctosector[629] = 0;
    sEPD_adctosector[630] = 0;
    sEPD_adctosector[631] = 0;
    sEPD_adctosector[632] = 0;
    sEPD_adctosector[633] = 0;
    sEPD_adctosector[634] = 0;
    sEPD_adctosector[635] = 0;
    sEPD_adctosector[636] = 0;
    sEPD_adctosector[637] = 0;
    sEPD_adctosector[638] = 0;
    sEPD_adctosector[639] = 0;
    sEPD_adctosector[640] = 8;
    sEPD_adctosector[641] = 8;
    sEPD_adctosector[642] = 8;
    sEPD_adctosector[643] = 8;
    sEPD_adctosector[644] = 8;
    sEPD_adctosector[645] = 8;
    sEPD_adctosector[646] = 8;
    sEPD_adctosector[647] = 8;
    sEPD_adctosector[648] = 8;
    sEPD_adctosector[649] = 8;
    sEPD_adctosector[650] = 8;
    sEPD_adctosector[651] = 8;
    sEPD_adctosector[652] = 8;
    sEPD_adctosector[653] = 8;
    sEPD_adctosector[654] = 8;
    sEPD_adctosector[655] = 8;
    sEPD_adctosector[656] = 8;
    sEPD_adctosector[657] = 8;
    sEPD_adctosector[658] = 8;
    sEPD_adctosector[659] = 8;
    sEPD_adctosector[660] = 8;
    sEPD_adctosector[661] = 8;
    sEPD_adctosector[662] = 8;
    sEPD_adctosector[663] = 8;
    sEPD_adctosector[664] = 8;
    sEPD_adctosector[665] = 8;
    sEPD_adctosector[666] = 8;
    sEPD_adctosector[667] = 8;
    sEPD_adctosector[668] = 8;
    sEPD_adctosector[669] = 8;
    sEPD_adctosector[670] = 8;
    sEPD_adctosector[671] = 8;
    sEPD_adctosector[672] = 9;
    sEPD_adctosector[673] = 9;
    sEPD_adctosector[674] = 9;
    sEPD_adctosector[675] = 9;
    sEPD_adctosector[676] = 9;
    sEPD_adctosector[677] = 9;
    sEPD_adctosector[678] = 9;
    sEPD_adctosector[679] = 9;
    sEPD_adctosector[680] = 9;
    sEPD_adctosector[681] = 9;
    sEPD_adctosector[682] = 9;
    sEPD_adctosector[683] = 9;
    sEPD_adctosector[684] = 9;
    sEPD_adctosector[685] = 9;
    sEPD_adctosector[686] = 9;
    sEPD_adctosector[687] = 9;
    sEPD_adctosector[688] = 9;
    sEPD_adctosector[689] = 9;
    sEPD_adctosector[690] = 9;
    sEPD_adctosector[691] = 9;
    sEPD_adctosector[692] = 9;
    sEPD_adctosector[693] = 9;
    sEPD_adctosector[694] = 9;
    sEPD_adctosector[695] = 9;
    sEPD_adctosector[696] = 9;
    sEPD_adctosector[697] = 9;
    sEPD_adctosector[698] = 9;
    sEPD_adctosector[699] = 9;
    sEPD_adctosector[700] = 9;
    sEPD_adctosector[701] = 9;
    sEPD_adctosector[702] = 9;
    sEPD_adctosector[703] = 9;
    sEPD_adctosector[704] = 5;
    sEPD_adctosector[705] = 5;
    sEPD_adctosector[706] = 5;
    sEPD_adctosector[707] = 5;
    sEPD_adctosector[708] = 5;
    sEPD_adctosector[709] = 5;
    sEPD_adctosector[710] = 5;
    sEPD_adctosector[711] = 5;
    sEPD_adctosector[712] = 5;
    sEPD_adctosector[713] = 5;
    sEPD_adctosector[714] = 5;
    sEPD_adctosector[715] = 5;
    sEPD_adctosector[716] = 5;
    sEPD_adctosector[717] = 5;
    sEPD_adctosector[718] = 5;
    sEPD_adctosector[719] = 5;
    sEPD_adctosector[720] = 5;
    sEPD_adctosector[721] = 5;
    sEPD_adctosector[722] = 5;
    sEPD_adctosector[723] = 5;
    sEPD_adctosector[724] = 5;
    sEPD_adctosector[725] = 5;
    sEPD_adctosector[726] = 5;
    sEPD_adctosector[727] = 5;
    sEPD_adctosector[728] = 5;
    sEPD_adctosector[729] = 5;
    sEPD_adctosector[730] = 5;
    sEPD_adctosector[731] = 5;
    sEPD_adctosector[732] = 5;
    sEPD_adctosector[733] = 5;
    sEPD_adctosector[734] = 5;
    sEPD_adctosector[735] = 5;
    sEPD_adctosector[736] = 10;
    sEPD_adctosector[737] = 10;
    sEPD_adctosector[738] = 10;
    sEPD_adctosector[739] = 10;
    sEPD_adctosector[740] = 10;
    sEPD_adctosector[741] = 10;
    sEPD_adctosector[742] = 10;
    sEPD_adctosector[743] = 10;
    sEPD_adctosector[744] = 10;
    sEPD_adctosector[745] = 10;
    sEPD_adctosector[746] = 10;
    sEPD_adctosector[747] = 10;
    sEPD_adctosector[748] = 10;
    sEPD_adctosector[749] = 10;
    sEPD_adctosector[750] = 10;
    sEPD_adctosector[751] = 10;
    sEPD_adctosector[752] = 10;
    sEPD_adctosector[753] = 10;
    sEPD_adctosector[754] = 10;
    sEPD_adctosector[755] = 10;
    sEPD_adctosector[756] = 10;
    sEPD_adctosector[757] = 10;
    sEPD_adctosector[758] = 10;
    sEPD_adctosector[759] = 10;
    sEPD_adctosector[760] = 10;
    sEPD_adctosector[761] = 10;
    sEPD_adctosector[762] = 10;
    sEPD_adctosector[763] = 10;
    sEPD_adctosector[764] = 10;
    sEPD_adctosector[765] = 10;
    sEPD_adctosector[766] = 10;
    sEPD_adctosector[767] = 10;
    return sEPD_adctosector[ch];
}

int SepdMonDraw::returnTile(int ch){
    int sEPD_adctotile[768];
    sEPD_adctotile[0] = 1;
    sEPD_adctotile[1] = 31;
    sEPD_adctotile[2] = 27;
    sEPD_adctotile[3] = 29;
    sEPD_adctotile[4] = 23;
    sEPD_adctotile[5] = 25;
    sEPD_adctotile[6] = 19;
    sEPD_adctotile[7] = 21;
    sEPD_adctotile[8] = 15;
    sEPD_adctotile[9] = 17;
    sEPD_adctotile[10] = 11;
    sEPD_adctotile[11] = 13;
    sEPD_adctotile[12] = 7;
    sEPD_adctotile[13] = 9;
    sEPD_adctotile[14] = 3;
    sEPD_adctotile[15] = 5;
    sEPD_adctotile[16] = 28;
    sEPD_adctotile[17] = 30;
    sEPD_adctotile[18] = 24;
    sEPD_adctotile[19] = 26;
    sEPD_adctotile[20] = 20;
    sEPD_adctotile[21] = 22;
    sEPD_adctotile[22] = 16;
    sEPD_adctotile[23] = 18;
    sEPD_adctotile[24] = 12;
    sEPD_adctotile[25] = 14;
    sEPD_adctotile[26] = 8;
    sEPD_adctotile[27] = 10;
    sEPD_adctotile[28] = 4;
    sEPD_adctotile[29] = 6;
    sEPD_adctotile[30] = 0;
    sEPD_adctotile[31] = 2;
    sEPD_adctotile[32] = 27;
    sEPD_adctotile[33] = 29;
    sEPD_adctotile[34] = 23;
    sEPD_adctotile[35] = 25;
    sEPD_adctotile[36] = 19;
    sEPD_adctotile[37] = 21;
    sEPD_adctotile[38] = 15;
    sEPD_adctotile[39] = 17;
    sEPD_adctotile[40] = 11;
    sEPD_adctotile[41] = 13;
    sEPD_adctotile[42] = 7;
    sEPD_adctotile[43] = 9;
    sEPD_adctotile[44] = 3;
    sEPD_adctotile[45] = 5;
    sEPD_adctotile[46] = 31;
    sEPD_adctotile[47] = 1;
    sEPD_adctotile[48] = 28;
    sEPD_adctotile[49] = 30;
    sEPD_adctotile[50] = 24;
    sEPD_adctotile[51] = 26;
    sEPD_adctotile[52] = 20;
    sEPD_adctotile[53] = 22;
    sEPD_adctotile[54] = 16;
    sEPD_adctotile[55] = 18;
    sEPD_adctotile[56] = 12;
    sEPD_adctotile[57] = 14;
    sEPD_adctotile[58] = 8;
    sEPD_adctotile[59] = 10;
    sEPD_adctotile[60] = 4;
    sEPD_adctotile[61] = 6;
    sEPD_adctotile[62] = 0;
    sEPD_adctotile[63] = 2;
    sEPD_adctotile[64] = 27;
    sEPD_adctotile[65] = 29;
    sEPD_adctotile[66] = 23;
    sEPD_adctotile[67] = 25;
    sEPD_adctotile[68] = 19;
    sEPD_adctotile[69] = 21;
    sEPD_adctotile[70] = 15;
    sEPD_adctotile[71] = 17;
    sEPD_adctotile[72] = 11;
    sEPD_adctotile[73] = 13;
    sEPD_adctotile[74] = 7;
    sEPD_adctotile[75] = 9;
    sEPD_adctotile[76] = 3;
    sEPD_adctotile[77] = 5;
    sEPD_adctotile[78] = 31;
    sEPD_adctotile[79] = 1;
    sEPD_adctotile[80] = 28;
    sEPD_adctotile[81] = 30;
    sEPD_adctotile[82] = 24;
    sEPD_adctotile[83] = 26;
    sEPD_adctotile[84] = 20;
    sEPD_adctotile[85] = 22;
    sEPD_adctotile[86] = 16;
    sEPD_adctotile[87] = 18;
    sEPD_adctotile[88] = 12;
    sEPD_adctotile[89] = 14;
    sEPD_adctotile[90] = 8;
    sEPD_adctotile[91] = 10;
    sEPD_adctotile[92] = 4;
    sEPD_adctotile[93] = 6;
    sEPD_adctotile[94] = 0;
    sEPD_adctotile[95] = 2;
    sEPD_adctotile[96] = 27;
    sEPD_adctotile[97] = 29;
    sEPD_adctotile[98] = 23;
    sEPD_adctotile[99] = 25;
    sEPD_adctotile[100] = 19;
    sEPD_adctotile[101] = 21;
    sEPD_adctotile[102] = 15;
    sEPD_adctotile[103] = 17;
    sEPD_adctotile[104] = 11;
    sEPD_adctotile[105] = 13;
    sEPD_adctotile[106] = 7;
    sEPD_adctotile[107] = 9;
    sEPD_adctotile[108] = 3;
    sEPD_adctotile[109] = 5;
    sEPD_adctotile[110] = 31;
    sEPD_adctotile[111] = 1;
    sEPD_adctotile[112] = 28;
    sEPD_adctotile[113] = 30;
    sEPD_adctotile[114] = 24;
    sEPD_adctotile[115] = 26;
    sEPD_adctotile[116] = 20;
    sEPD_adctotile[117] = 22;
    sEPD_adctotile[118] = 16;
    sEPD_adctotile[119] = 18;
    sEPD_adctotile[120] = 12;
    sEPD_adctotile[121] = 14;
    sEPD_adctotile[122] = 8;
    sEPD_adctotile[123] = 10;
    sEPD_adctotile[124] = 4;
    sEPD_adctotile[125] = 6;
    sEPD_adctotile[126] = 0;
    sEPD_adctotile[127] = 2;
    sEPD_adctotile[128] = 27;
    sEPD_adctotile[129] = 29;
    sEPD_adctotile[130] = 23;
    sEPD_adctotile[131] = 25;
    sEPD_adctotile[132] = 19;
    sEPD_adctotile[133] = 21;
    sEPD_adctotile[134] = 15;
    sEPD_adctotile[135] = 17;
    sEPD_adctotile[136] = 11;
    sEPD_adctotile[137] = 13;
    sEPD_adctotile[138] = 7;
    sEPD_adctotile[139] = 9;
    sEPD_adctotile[140] = 3;
    sEPD_adctotile[141] = 5;
    sEPD_adctotile[142] = 31;
    sEPD_adctotile[143] = 1;
    sEPD_adctotile[144] = 28;
    sEPD_adctotile[145] = 30;
    sEPD_adctotile[146] = 24;
    sEPD_adctotile[147] = 26;
    sEPD_adctotile[148] = 20;
    sEPD_adctotile[149] = 22;
    sEPD_adctotile[150] = 16;
    sEPD_adctotile[151] = 18;
    sEPD_adctotile[152] = 12;
    sEPD_adctotile[153] = 14;
    sEPD_adctotile[154] = 8;
    sEPD_adctotile[155] = 10;
    sEPD_adctotile[156] = 4;
    sEPD_adctotile[157] = 6;
    sEPD_adctotile[158] = 0;
    sEPD_adctotile[159] = 2;
    sEPD_adctotile[160] = 27;
    sEPD_adctotile[161] = 29;
    sEPD_adctotile[162] = 23;
    sEPD_adctotile[163] = 25;
    sEPD_adctotile[164] = 19;
    sEPD_adctotile[165] = 21;
    sEPD_adctotile[166] = 15;
    sEPD_adctotile[167] = 17;
    sEPD_adctotile[168] = 11;
    sEPD_adctotile[169] = 13;
    sEPD_adctotile[170] = 7;
    sEPD_adctotile[171] = 9;
    sEPD_adctotile[172] = 3;
    sEPD_adctotile[173] = 5;
    sEPD_adctotile[174] = 31;
    sEPD_adctotile[175] = 1;
    sEPD_adctotile[176] = 28;
    sEPD_adctotile[177] = 30;
    sEPD_adctotile[178] = 24;
    sEPD_adctotile[179] = 26;
    sEPD_adctotile[180] = 20;
    sEPD_adctotile[181] = 22;
    sEPD_adctotile[182] = 16;
    sEPD_adctotile[183] = 18;
    sEPD_adctotile[184] = 12;
    sEPD_adctotile[185] = 14;
    sEPD_adctotile[186] = 8;
    sEPD_adctotile[187] = 10;
    sEPD_adctotile[188] = 4;
    sEPD_adctotile[189] = 6;
    sEPD_adctotile[190] = 0;
    sEPD_adctotile[191] = 2;
    sEPD_adctotile[192] = 27;
    sEPD_adctotile[193] = 29;
    sEPD_adctotile[194] = 23;
    sEPD_adctotile[195] = 25;
    sEPD_adctotile[196] = 19;
    sEPD_adctotile[197] = 21;
    sEPD_adctotile[198] = 15;
    sEPD_adctotile[199] = 17;
    sEPD_adctotile[200] = 11;
    sEPD_adctotile[201] = 13;
    sEPD_adctotile[202] = 7;
    sEPD_adctotile[203] = 9;
    sEPD_adctotile[204] = 3;
    sEPD_adctotile[205] = 5;
    sEPD_adctotile[206] = 31;
    sEPD_adctotile[207] = 1;
    sEPD_adctotile[208] = 28;
    sEPD_adctotile[209] = 30;
    sEPD_adctotile[210] = 24;
    sEPD_adctotile[211] = 26;
    sEPD_adctotile[212] = 20;
    sEPD_adctotile[213] = 22;
    sEPD_adctotile[214] = 16;
    sEPD_adctotile[215] = 18;
    sEPD_adctotile[216] = 12;
    sEPD_adctotile[217] = 14;
    sEPD_adctotile[218] = 8;
    sEPD_adctotile[219] = 10;
    sEPD_adctotile[220] = 4;
    sEPD_adctotile[221] = 6;
    sEPD_adctotile[222] = 0;
    sEPD_adctotile[223] = 2;
    sEPD_adctotile[224] = 27;
    sEPD_adctotile[225] = 29;
    sEPD_adctotile[226] = 23;
    sEPD_adctotile[227] = 25;
    sEPD_adctotile[228] = 19;
    sEPD_adctotile[229] = 21;
    sEPD_adctotile[230] = 15;
    sEPD_adctotile[231] = 17;
    sEPD_adctotile[232] = 11;
    sEPD_adctotile[233] = 13;
    sEPD_adctotile[234] = 7;
    sEPD_adctotile[235] = 9;
    sEPD_adctotile[236] = 3;
    sEPD_adctotile[237] = 5;
    sEPD_adctotile[238] = 31;
    sEPD_adctotile[239] = 1;
    sEPD_adctotile[240] = 28;
    sEPD_adctotile[241] = 30;
    sEPD_adctotile[242] = 24;
    sEPD_adctotile[243] = 26;
    sEPD_adctotile[244] = 20;
    sEPD_adctotile[245] = 22;
    sEPD_adctotile[246] = 16;
    sEPD_adctotile[247] = 18;
    sEPD_adctotile[248] = 12;
    sEPD_adctotile[249] = 14;
    sEPD_adctotile[250] = 8;
    sEPD_adctotile[251] = 10;
    sEPD_adctotile[252] = 4;
    sEPD_adctotile[253] = 6;
    sEPD_adctotile[254] = 0;
    sEPD_adctotile[255] = 2;
    sEPD_adctotile[256] = 27;
    sEPD_adctotile[257] = 29;
    sEPD_adctotile[258] = 23;
    sEPD_adctotile[259] = 25;
    sEPD_adctotile[260] = 19;
    sEPD_adctotile[261] = 21;
    sEPD_adctotile[262] = 15;
    sEPD_adctotile[263] = 17;
    sEPD_adctotile[264] = 11;
    sEPD_adctotile[265] = 13;
    sEPD_adctotile[266] = 7;
    sEPD_adctotile[267] = 9;
    sEPD_adctotile[268] = 3;
    sEPD_adctotile[269] = 5;
    sEPD_adctotile[270] = 31;
    sEPD_adctotile[271] = 1;
    sEPD_adctotile[272] = 28;
    sEPD_adctotile[273] = 30;
    sEPD_adctotile[274] = 24;
    sEPD_adctotile[275] = 26;
    sEPD_adctotile[276] = 20;
    sEPD_adctotile[277] = 22;
    sEPD_adctotile[278] = 16;
    sEPD_adctotile[279] = 18;
    sEPD_adctotile[280] = 12;
    sEPD_adctotile[281] = 14;
    sEPD_adctotile[282] = 8;
    sEPD_adctotile[283] = 10;
    sEPD_adctotile[284] = 4;
    sEPD_adctotile[285] = 6;
    sEPD_adctotile[286] = 0;
    sEPD_adctotile[287] = 2;
    sEPD_adctotile[288] = 27;
    sEPD_adctotile[289] = 29;
    sEPD_adctotile[290] = 23;
    sEPD_adctotile[291] = 25;
    sEPD_adctotile[292] = 19;
    sEPD_adctotile[293] = 21;
    sEPD_adctotile[294] = 15;
    sEPD_adctotile[295] = 17;
    sEPD_adctotile[296] = 11;
    sEPD_adctotile[297] = 13;
    sEPD_adctotile[298] = 7;
    sEPD_adctotile[299] = 9;
    sEPD_adctotile[300] = 3;
    sEPD_adctotile[301] = 5;
    sEPD_adctotile[302] = 31;
    sEPD_adctotile[303] = 1;
    sEPD_adctotile[304] = 28;
    sEPD_adctotile[305] = 30;
    sEPD_adctotile[306] = 24;
    sEPD_adctotile[307] = 26;
    sEPD_adctotile[308] = 20;
    sEPD_adctotile[309] = 22;
    sEPD_adctotile[310] = 16;
    sEPD_adctotile[311] = 18;
    sEPD_adctotile[312] = 12;
    sEPD_adctotile[313] = 14;
    sEPD_adctotile[314] = 8;
    sEPD_adctotile[315] = 10;
    sEPD_adctotile[316] = 4;
    sEPD_adctotile[317] = 6;
    sEPD_adctotile[318] = 0;
    sEPD_adctotile[319] = 2;
    sEPD_adctotile[320] = 27;
    sEPD_adctotile[321] = 29;
    sEPD_adctotile[322] = 23;
    sEPD_adctotile[323] = 25;
    sEPD_adctotile[324] = 19;
    sEPD_adctotile[325] = 21;
    sEPD_adctotile[326] = 15;
    sEPD_adctotile[327] = 17;
    sEPD_adctotile[328] = 11;
    sEPD_adctotile[329] = 13;
    sEPD_adctotile[330] = 7;
    sEPD_adctotile[331] = 9;
    sEPD_adctotile[332] = 3;
    sEPD_adctotile[333] = 5;
    sEPD_adctotile[334] = 31;
    sEPD_adctotile[335] = 1;
    sEPD_adctotile[336] = 28;
    sEPD_adctotile[337] = 30;
    sEPD_adctotile[338] = 24;
    sEPD_adctotile[339] = 26;
    sEPD_adctotile[340] = 20;
    sEPD_adctotile[341] = 22;
    sEPD_adctotile[342] = 16;
    sEPD_adctotile[343] = 18;
    sEPD_adctotile[344] = 12;
    sEPD_adctotile[345] = 14;
    sEPD_adctotile[346] = 8;
    sEPD_adctotile[347] = 10;
    sEPD_adctotile[348] = 4;
    sEPD_adctotile[349] = 6;
    sEPD_adctotile[350] = 0;
    sEPD_adctotile[351] = 2;
    sEPD_adctotile[352] = 27;
    sEPD_adctotile[353] = 29;
    sEPD_adctotile[354] = 23;
    sEPD_adctotile[355] = 25;
    sEPD_adctotile[356] = 19;
    sEPD_adctotile[357] = 21;
    sEPD_adctotile[358] = 15;
    sEPD_adctotile[359] = 17;
    sEPD_adctotile[360] = 11;
    sEPD_adctotile[361] = 13;
    sEPD_adctotile[362] = 7;
    sEPD_adctotile[363] = 9;
    sEPD_adctotile[364] = 3;
    sEPD_adctotile[365] = 5;
    sEPD_adctotile[366] = 31;
    sEPD_adctotile[367] = 1;
    sEPD_adctotile[368] = 28;
    sEPD_adctotile[369] = 30;
    sEPD_adctotile[370] = 24;
    sEPD_adctotile[371] = 26;
    sEPD_adctotile[372] = 20;
    sEPD_adctotile[373] = 22;
    sEPD_adctotile[374] = 16;
    sEPD_adctotile[375] = 18;
    sEPD_adctotile[376] = 12;
    sEPD_adctotile[377] = 14;
    sEPD_adctotile[378] = 8;
    sEPD_adctotile[379] = 10;
    sEPD_adctotile[380] = 4;
    sEPD_adctotile[381] = 6;
    sEPD_adctotile[382] = 0;
    sEPD_adctotile[383] = 2;
    sEPD_adctotile[384] = 27;
    sEPD_adctotile[385] = 29;
    sEPD_adctotile[386] = 23;
    sEPD_adctotile[387] = 25;
    sEPD_adctotile[388] = 19;
    sEPD_adctotile[389] = 21;
    sEPD_adctotile[390] = 15;
    sEPD_adctotile[391] = 17;
    sEPD_adctotile[392] = 11;
    sEPD_adctotile[393] = 13;
    sEPD_adctotile[394] = 7;
    sEPD_adctotile[395] = 9;
    sEPD_adctotile[396] = 3;
    sEPD_adctotile[397] = 5;
    sEPD_adctotile[398] = 31;
    sEPD_adctotile[399] = 1;
    sEPD_adctotile[400] = 28;
    sEPD_adctotile[401] = 30;
    sEPD_adctotile[402] = 24;
    sEPD_adctotile[403] = 26;
    sEPD_adctotile[404] = 20;
    sEPD_adctotile[405] = 22;
    sEPD_adctotile[406] = 16;
    sEPD_adctotile[407] = 18;
    sEPD_adctotile[408] = 12;
    sEPD_adctotile[409] = 14;
    sEPD_adctotile[410] = 8;
    sEPD_adctotile[411] = 10;
    sEPD_adctotile[412] = 4;
    sEPD_adctotile[413] = 6;
    sEPD_adctotile[414] = 0;
    sEPD_adctotile[415] = 2;
    sEPD_adctotile[416] = 27;
    sEPD_adctotile[417] = 29;
    sEPD_adctotile[418] = 23;
    sEPD_adctotile[419] = 25;
    sEPD_adctotile[420] = 19;
    sEPD_adctotile[421] = 21;
    sEPD_adctotile[422] = 15;
    sEPD_adctotile[423] = 17;
    sEPD_adctotile[424] = 11;
    sEPD_adctotile[425] = 13;
    sEPD_adctotile[426] = 7;
    sEPD_adctotile[427] = 9;
    sEPD_adctotile[428] = 3;
    sEPD_adctotile[429] = 5;
    sEPD_adctotile[430] = 31;
    sEPD_adctotile[431] = 1;
    sEPD_adctotile[432] = 28;
    sEPD_adctotile[433] = 30;
    sEPD_adctotile[434] = 24;
    sEPD_adctotile[435] = 26;
    sEPD_adctotile[436] = 20;
    sEPD_adctotile[437] = 22;
    sEPD_adctotile[438] = 16;
    sEPD_adctotile[439] = 18;
    sEPD_adctotile[440] = 12;
    sEPD_adctotile[441] = 14;
    sEPD_adctotile[442] = 8;
    sEPD_adctotile[443] = 10;
    sEPD_adctotile[444] = 4;
    sEPD_adctotile[445] = 6;
    sEPD_adctotile[446] = 0;
    sEPD_adctotile[447] = 2;
    sEPD_adctotile[448] = 27;
    sEPD_adctotile[449] = 29;
    sEPD_adctotile[450] = 23;
    sEPD_adctotile[451] = 25;
    sEPD_adctotile[452] = 19;
    sEPD_adctotile[453] = 21;
    sEPD_adctotile[454] = 15;
    sEPD_adctotile[455] = 17;
    sEPD_adctotile[456] = 11;
    sEPD_adctotile[457] = 13;
    sEPD_adctotile[458] = 7;
    sEPD_adctotile[459] = 9;
    sEPD_adctotile[460] = 3;
    sEPD_adctotile[461] = 5;
    sEPD_adctotile[462] = 31;
    sEPD_adctotile[463] = 1;
    sEPD_adctotile[464] = 28;
    sEPD_adctotile[465] = 30;
    sEPD_adctotile[466] = 24;
    sEPD_adctotile[467] = 26;
    sEPD_adctotile[468] = 20;
    sEPD_adctotile[469] = 22;
    sEPD_adctotile[470] = 16;
    sEPD_adctotile[471] = 18;
    sEPD_adctotile[472] = 12;
    sEPD_adctotile[473] = 14;
    sEPD_adctotile[474] = 8;
    sEPD_adctotile[475] = 10;
    sEPD_adctotile[476] = 4;
    sEPD_adctotile[477] = 6;
    sEPD_adctotile[478] = 0;
    sEPD_adctotile[479] = 2;
    sEPD_adctotile[480] = 27;
    sEPD_adctotile[481] = 29;
    sEPD_adctotile[482] = 23;
    sEPD_adctotile[483] = 25;
    sEPD_adctotile[484] = 19;
    sEPD_adctotile[485] = 21;
    sEPD_adctotile[486] = 15;
    sEPD_adctotile[487] = 17;
    sEPD_adctotile[488] = 11;
    sEPD_adctotile[489] = 13;
    sEPD_adctotile[490] = 7;
    sEPD_adctotile[491] = 9;
    sEPD_adctotile[492] = 3;
    sEPD_adctotile[493] = 5;
    sEPD_adctotile[494] = 31;
    sEPD_adctotile[495] = 1;
    sEPD_adctotile[496] = 28;
    sEPD_adctotile[497] = 30;
    sEPD_adctotile[498] = 24;
    sEPD_adctotile[499] = 26;
    sEPD_adctotile[500] = 20;
    sEPD_adctotile[501] = 22;
    sEPD_adctotile[502] = 16;
    sEPD_adctotile[503] = 18;
    sEPD_adctotile[504] = 12;
    sEPD_adctotile[505] = 14;
    sEPD_adctotile[506] = 8;
    sEPD_adctotile[507] = 10;
    sEPD_adctotile[508] = 4;
    sEPD_adctotile[509] = 6;
    sEPD_adctotile[510] = 0;
    sEPD_adctotile[511] = 2;
    sEPD_adctotile[512] = 27;
    sEPD_adctotile[513] = 29;
    sEPD_adctotile[514] = 23;
    sEPD_adctotile[515] = 25;
    sEPD_adctotile[516] = 19;
    sEPD_adctotile[517] = 21;
    sEPD_adctotile[518] = 15;
    sEPD_adctotile[519] = 17;
    sEPD_adctotile[520] = 11;
    sEPD_adctotile[521] = 13;
    sEPD_adctotile[522] = 7;
    sEPD_adctotile[523] = 9;
    sEPD_adctotile[524] = 3;
    sEPD_adctotile[525] = 5;
    sEPD_adctotile[526] = 31;
    sEPD_adctotile[527] = 1;
    sEPD_adctotile[528] = 28;
    sEPD_adctotile[529] = 30;
    sEPD_adctotile[530] = 24;
    sEPD_adctotile[531] = 26;
    sEPD_adctotile[532] = 20;
    sEPD_adctotile[533] = 22;
    sEPD_adctotile[534] = 16;
    sEPD_adctotile[535] = 18;
    sEPD_adctotile[536] = 12;
    sEPD_adctotile[537] = 14;
    sEPD_adctotile[538] = 8;
    sEPD_adctotile[539] = 10;
    sEPD_adctotile[540] = 4;
    sEPD_adctotile[541] = 6;
    sEPD_adctotile[542] = 0;
    sEPD_adctotile[543] = 2;
    sEPD_adctotile[544] = 27;
    sEPD_adctotile[545] = 29;
    sEPD_adctotile[546] = 23;
    sEPD_adctotile[547] = 25;
    sEPD_adctotile[548] = 19;
    sEPD_adctotile[549] = 21;
    sEPD_adctotile[550] = 15;
    sEPD_adctotile[551] = 17;
    sEPD_adctotile[552] = 11;
    sEPD_adctotile[553] = 13;
    sEPD_adctotile[554] = 7;
    sEPD_adctotile[555] = 9;
    sEPD_adctotile[556] = 3;
    sEPD_adctotile[557] = 5;
    sEPD_adctotile[558] = 31;
    sEPD_adctotile[559] = 1;
    sEPD_adctotile[560] = 28;
    sEPD_adctotile[561] = 30;
    sEPD_adctotile[562] = 24;
    sEPD_adctotile[563] = 26;
    sEPD_adctotile[564] = 20;
    sEPD_adctotile[565] = 22;
    sEPD_adctotile[566] = 16;
    sEPD_adctotile[567] = 18;
    sEPD_adctotile[568] = 12;
    sEPD_adctotile[569] = 14;
    sEPD_adctotile[570] = 8;
    sEPD_adctotile[571] = 10;
    sEPD_adctotile[572] = 4;
    sEPD_adctotile[573] = 6;
    sEPD_adctotile[574] = 0;
    sEPD_adctotile[575] = 2;
    sEPD_adctotile[576] = 27;
    sEPD_adctotile[577] = 29;
    sEPD_adctotile[578] = 23;
    sEPD_adctotile[579] = 25;
    sEPD_adctotile[580] = 19;
    sEPD_adctotile[581] = 21;
    sEPD_adctotile[582] = 15;
    sEPD_adctotile[583] = 17;
    sEPD_adctotile[584] = 11;
    sEPD_adctotile[585] = 13;
    sEPD_adctotile[586] = 7;
    sEPD_adctotile[587] = 9;
    sEPD_adctotile[588] = 3;
    sEPD_adctotile[589] = 5;
    sEPD_adctotile[590] = 31;
    sEPD_adctotile[591] = 1;
    sEPD_adctotile[592] = 28;
    sEPD_adctotile[593] = 30;
    sEPD_adctotile[594] = 24;
    sEPD_adctotile[595] = 26;
    sEPD_adctotile[596] = 20;
    sEPD_adctotile[597] = 22;
    sEPD_adctotile[598] = 16;
    sEPD_adctotile[599] = 18;
    sEPD_adctotile[600] = 12;
    sEPD_adctotile[601] = 14;
    sEPD_adctotile[602] = 8;
    sEPD_adctotile[603] = 10;
    sEPD_adctotile[604] = 4;
    sEPD_adctotile[605] = 6;
    sEPD_adctotile[606] = 0;
    sEPD_adctotile[607] = 2;
    sEPD_adctotile[608] = 27;
    sEPD_adctotile[609] = 29;
    sEPD_adctotile[610] = 23;
    sEPD_adctotile[611] = 25;
    sEPD_adctotile[612] = 19;
    sEPD_adctotile[613] = 21;
    sEPD_adctotile[614] = 15;
    sEPD_adctotile[615] = 17;
    sEPD_adctotile[616] = 11;
    sEPD_adctotile[617] = 13;
    sEPD_adctotile[618] = 7;
    sEPD_adctotile[619] = 9;
    sEPD_adctotile[620] = 3;
    sEPD_adctotile[621] = 5;
    sEPD_adctotile[622] = 31;
    sEPD_adctotile[623] = 1;
    sEPD_adctotile[624] = 28;
    sEPD_adctotile[625] = 30;
    sEPD_adctotile[626] = 24;
    sEPD_adctotile[627] = 26;
    sEPD_adctotile[628] = 20;
    sEPD_adctotile[629] = 22;
    sEPD_adctotile[630] = 16;
    sEPD_adctotile[631] = 18;
    sEPD_adctotile[632] = 12;
    sEPD_adctotile[633] = 14;
    sEPD_adctotile[634] = 8;
    sEPD_adctotile[635] = 10;
    sEPD_adctotile[636] = 4;
    sEPD_adctotile[637] = 6;
    sEPD_adctotile[638] = 0;
    sEPD_adctotile[639] = 2;
    sEPD_adctotile[640] = 27;
    sEPD_adctotile[641] = 29;
    sEPD_adctotile[642] = 23;
    sEPD_adctotile[643] = 25;
    sEPD_adctotile[644] = 19;
    sEPD_adctotile[645] = 21;
    sEPD_adctotile[646] = 15;
    sEPD_adctotile[647] = 17;
    sEPD_adctotile[648] = 11;
    sEPD_adctotile[649] = 13;
    sEPD_adctotile[650] = 7;
    sEPD_adctotile[651] = 9;
    sEPD_adctotile[652] = 3;
    sEPD_adctotile[653] = 5;
    sEPD_adctotile[654] = 31;
    sEPD_adctotile[655] = 1;
    sEPD_adctotile[656] = 28;
    sEPD_adctotile[657] = 30;
    sEPD_adctotile[658] = 24;
    sEPD_adctotile[659] = 26;
    sEPD_adctotile[660] = 20;
    sEPD_adctotile[661] = 22;
    sEPD_adctotile[662] = 16;
    sEPD_adctotile[663] = 18;
    sEPD_adctotile[664] = 12;
    sEPD_adctotile[665] = 14;
    sEPD_adctotile[666] = 8;
    sEPD_adctotile[667] = 10;
    sEPD_adctotile[668] = 4;
    sEPD_adctotile[669] = 6;
    sEPD_adctotile[670] = 0;
    sEPD_adctotile[671] = 2;
    sEPD_adctotile[672] = 27;
    sEPD_adctotile[673] = 29;
    sEPD_adctotile[674] = 23;
    sEPD_adctotile[675] = 25;
    sEPD_adctotile[676] = 19;
    sEPD_adctotile[677] = 21;
    sEPD_adctotile[678] = 15;
    sEPD_adctotile[679] = 17;
    sEPD_adctotile[680] = 11;
    sEPD_adctotile[681] = 13;
    sEPD_adctotile[682] = 7;
    sEPD_adctotile[683] = 9;
    sEPD_adctotile[684] = 3;
    sEPD_adctotile[685] = 5;
    sEPD_adctotile[686] = 31;
    sEPD_adctotile[687] = 1;
    sEPD_adctotile[688] = 28;
    sEPD_adctotile[689] = 30;
    sEPD_adctotile[690] = 24;
    sEPD_adctotile[691] = 26;
    sEPD_adctotile[692] = 20;
    sEPD_adctotile[693] = 22;
    sEPD_adctotile[694] = 16;
    sEPD_adctotile[695] = 18;
    sEPD_adctotile[696] = 12;
    sEPD_adctotile[697] = 14;
    sEPD_adctotile[698] = 8;
    sEPD_adctotile[699] = 10;
    sEPD_adctotile[700] = 4;
    sEPD_adctotile[701] = 6;
    sEPD_adctotile[702] = 0;
    sEPD_adctotile[703] = 2;
    sEPD_adctotile[704] = 27;
    sEPD_adctotile[705] = 29;
    sEPD_adctotile[706] = 23;
    sEPD_adctotile[707] = 25;
    sEPD_adctotile[708] = 19;
    sEPD_adctotile[709] = 21;
    sEPD_adctotile[710] = 15;
    sEPD_adctotile[711] = 17;
    sEPD_adctotile[712] = 11;
    sEPD_adctotile[713] = 13;
    sEPD_adctotile[714] = 7;
    sEPD_adctotile[715] = 9;
    sEPD_adctotile[716] = 3;
    sEPD_adctotile[717] = 5;
    sEPD_adctotile[718] = 31;
    sEPD_adctotile[719] = 1;
    sEPD_adctotile[720] = 28;
    sEPD_adctotile[721] = 30;
    sEPD_adctotile[722] = 24;
    sEPD_adctotile[723] = 26;
    sEPD_adctotile[724] = 20;
    sEPD_adctotile[725] = 22;
    sEPD_adctotile[726] = 16;
    sEPD_adctotile[727] = 18;
    sEPD_adctotile[728] = 12;
    sEPD_adctotile[729] = 14;
    sEPD_adctotile[730] = 8;
    sEPD_adctotile[731] = 10;
    sEPD_adctotile[732] = 4;
    sEPD_adctotile[733] = 6;
    sEPD_adctotile[734] = 0;
    sEPD_adctotile[735] = 2;
    sEPD_adctotile[736] = 27;
    sEPD_adctotile[737] = 29;
    sEPD_adctotile[738] = 23;
    sEPD_adctotile[739] = 25;
    sEPD_adctotile[740] = 19;
    sEPD_adctotile[741] = 21;
    sEPD_adctotile[742] = 15;
    sEPD_adctotile[743] = 17;
    sEPD_adctotile[744] = 11;
    sEPD_adctotile[745] = 13;
    sEPD_adctotile[746] = 7;
    sEPD_adctotile[747] = 9;
    sEPD_adctotile[748] = 3;
    sEPD_adctotile[749] = 5;
    sEPD_adctotile[750] = 31;
    sEPD_adctotile[751] = 1;
    sEPD_adctotile[752] = 28;
    sEPD_adctotile[753] = 30;
    sEPD_adctotile[754] = 24;
    sEPD_adctotile[755] = 26;
    sEPD_adctotile[756] = 20;
    sEPD_adctotile[757] = 22;
    sEPD_adctotile[758] = 16;
    sEPD_adctotile[759] = 18;
    sEPD_adctotile[760] = 12;
    sEPD_adctotile[761] = 14;
    sEPD_adctotile[762] = 8;
    sEPD_adctotile[763] = 10;
    sEPD_adctotile[764] = 4;
    sEPD_adctotile[765] = 6;
    sEPD_adctotile[766] = 0;
    sEPD_adctotile[767] = 2;
    return sEPD_adctotile[ch];
}

int SepdMonDraw::DrawServerStats()
{
  int canvasindex=6;
  OnlMonClient* cl = OnlMonClient::instance();
  if (!gROOT->FindObject("SepdServerStats"))
  {
    MakeCanvas("SepdServerStats");
  }
  TC[canvasindex]->Clear("D");
  TC[canvasindex]->SetEditable(true);
  transparent[canvasindex]->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  PrintRun.SetTextSize(0.04);
  PrintRun.SetTextColor(1);
  PrintRun.DrawText(0.5, 0.99, "Server Statistics");

  PrintRun.SetTextSize(0.02);
  double vdist = 0.05;
  double vpos = 0.9;
  time_t clienttime = time(nullptr);
  for (const auto& server : m_ServerSet)
  {
    std::ostringstream txt;
    auto servermapiter = cl->GetServerMap(server);
    if (servermapiter == cl->GetServerMapEnd())
    {
      txt << "Server " << server
          << " is dead ";
      PrintRun.SetTextColor(kRed);
    }
    else
    {
      int gl1counts = std::get<4>(servermapiter->second);
      time_t currtime = std::get<3>(servermapiter->second);
      txt << "Server " << server
          << ", run number: " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second);
      if (gl1counts >= 0)
	{
          txt << ", gl1 count: " << gl1counts;
	}
        txt  << ", current Event time: " << ctime(&currtime);
	if (isHtml())
	  {
	    clienttime = currtime; // just prevent the font from getting red
	  }
	else // print time diff only for live display
	  {
        txt  << ", minutes since last evt: " << (clienttime - currtime)/60;
          }
	if (std::get<0>(servermapiter->second) && ((clienttime - currtime)/60) < 10)
      {
        PrintRun.SetTextColor(kGray + 2);
      }
      else
      {
        PrintRun.SetTextColor(kRed);
      }
    }
    PrintRun.DrawText(0.5, vpos, txt.str().c_str());
    vpos -= vdist;
  }
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);

  return 0;
}

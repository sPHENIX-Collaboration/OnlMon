#include "HcalMonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <TAxis.h>  // for TAxis
#include <TButton.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TProfile.h>
#include <TQObject.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TFile.h>
#include <TFrame.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

HcalMonDraw::HcalMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
  // if name start with O prefix = "OHCALMON"
  // if name start with I prefix = "IHCALMON"
  if (ThisName[0] == 'O')
  {
    prefix = "OHCALMON";
  }
  else if (ThisName[0] == 'I')
  {
    prefix = "IHCALMON";
  }
  else
  {
    std::cout << "HcalMonDraw::HcalMonDraw() ERROR: name does not start with O or I " << ThisName << std::endl;
    exit(1);
  }
  return;
}

int HcalMonDraw::Init()
{
  hcalStyle = new TStyle("hcalStyle", "hcalStyle");

  Int_t font = 42;  // Helvetica
  hcalStyle->SetLabelFont(font, "x");
  hcalStyle->SetTitleFont(font, "x");
  hcalStyle->SetLabelFont(font, "y");
  hcalStyle->SetTitleFont(font, "y");
  hcalStyle->SetLabelFont(font, "z");
  hcalStyle->SetTitleFont(font, "z");
  hcalStyle->SetOptStat(0);
  hcalStyle->SetPadTickX(1);
  hcalStyle->SetPadTickY(1);
  gROOT->SetStyle("hcalStyle");
  gROOT->ForceStyle();
  char TEMPFILENAME[100];
  const char *hcalcalib = getenv("HCALCALIB");

  sprintf(TEMPFILENAME, "%s/%s_40747.root",hcalcalib, prefix.c_str());

  TFile* tempfile = new TFile(TEMPFILENAME, "READ");
  if (!tempfile->IsOpen())
  {
    std::cout << "HcalMonDraw::Init() ERROR: Could not open file " << TEMPFILENAME << std::endl;
    exit(1);
  }
  h2_mean_template = (TH2F*) tempfile->Get("h2_hcal_hits_template");

  if (!h2_mean_template)
  {
    std::cout << "HcalMonDraw::Init() ERROR: Could not find histogram h2_mean_template in file " << TEMPFILENAME << std::endl;
    exit(1);
  }
  h1_zs = new TH1F("h1_zs", "unsuppressed rate ", 100, 0, 1.1);
  h1_zs_low = new TH1F("h1_zs_low", "unsuppressed rate ", 100, 0, 1.1);
  h1_zs_high = new TH1F("h1_zs_high", "unsuppressed rate ", 100, 0, 1.1);

  MakeZSPalette();
  return 0;
}

int HcalMonDraw::MakeCanvas(const std::string& name)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "HcalMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "Hcal Towers", xsize / 3, 0, xsize / 3, ysize * 0.9);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("hist", "On the top", 0., 0.2, 1., 1.);
    Pad[0]->Draw();
    // Pad[1]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();

    // warning
    warning[0] = new TPad("warning0", "this does not show", 0, 0, 0.9, 0.2);
    warning[0]->SetFillStyle(4000);
    warning[0]->Draw();

    TC[0]->SetEditable(false);
  }
  else if (name == "HcalMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "Hcal Sector Running Mean", xsize / 3, 0, xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[2] = new TPad("hcalpad3", "who needs this?", 0.0, 0.0, 1, 0.92, 0);
    // Pad[3] = new TPad("hcalpad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[2]->Draw();
    // Pad[3]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "HcalMon3")
  {
    TC[3] = new TCanvas(name.c_str(), "HcalMon3 Waveform Info", xsize / 3, 0, xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[6] = new TPad("hcalpad6", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
    Pad[7] = new TPad("hcalpad7", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
    Pad[8] = new TPad("hcalpad8", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
    Pad[6]->Draw();
    Pad[7]->Draw();
    Pad[8]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(false);
  }
  else if (name == "HcalMon4")
  {
    // xpos negative: do not draw menu bar
    TC[5] = new TCanvas(name.c_str(), "HCAL Packet Information", 2 * xsize / 3, 0, 2 * xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[10] = new TPad("pad10", "packet event check", 0.0, 0.6, 1.0 / 2, 0.95, 0);
    Pad[11] = new TPad("pad11", "packet size", 0.0, 0.3, 1.0 / 2, 0.6, 0);
    Pad[12] = new TPad("pad12", "packet channels", 0.0, 0.0, 1.0 / 2, 0.3, 0);
    Pad[13] = new TPad("pad13", "event number offset", 0.5, 0.6, 1.0, 0.95, 0);
    // pad 14 and 15 side by side from left to right for correlation
    Pad[14] = new TPad("pad14", "correlation0", 0.5, 0.3, 0.75, 0.6, 0);
    Pad[15] = new TPad("pad15", "correlation1", 0.75, 0.3, 1.0, 0.6, 0);

    Pad[10]->Draw();
    Pad[11]->Draw();
    Pad[12]->Draw();
    Pad[13]->Draw();
    Pad[14]->Draw();
    Pad[15]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[5] = new TPad("transparent1", "this does not show", 0, 0, 1., 1);
    transparent[5]->SetFillStyle(4000);
    transparent[5]->Draw();

    // packet warnings
    warning[1] = new TPad("warning1", "packet warnings", 0.5, 0, 1, 0.2);
    warning[1]->SetFillStyle(4000);
    warning[1]->Draw();
    TC[5]->SetEditable(false);
  }
  else if (name == "HcalMon5")
  {
    TC[6] = new TCanvas(name.c_str(), "Expert: Trigger Info", xsize / 2, 0, xsize / 2, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[16] = new TPad("hcalpad16", "", 0.0, 0.6, 0.5, 0.95, 0);
    Pad[17] = new TPad("hcalpad17", "", 0.5, 0.6, 1.0, 0.95, 0);
    Pad[19] = new TPad("hcalpad19", "", 0.0, 0.3, 0.5, 0.6, 0);
    Pad[20] = new TPad("hcalpad20", "", 0.5, 0.3, 1.0, 0.6, 0);
    Pad[18] = new TPad("hcalpad18", "", 0.0, 0.0, 1.0, 0.2, 0);
    Pad[16]->Draw();
    Pad[17]->Draw();
    Pad[18]->Draw();
    Pad[19]->Draw();
    Pad[20]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
    transparent[6]->SetFillStyle(4000);
    transparent[6]->Draw();
    TC[6]->SetEditable(false);
  }
  else if(name == "HcalMon6")
  {
    TC[7] = new TCanvas(name.c_str(), "Expert: Tower Information", xsize , 0, xsize, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[21] = new TPad("hcalpad21", "", 0.0, 0.5, 0.5, 0.95, 0);
    Pad[22] = new TPad("hcalpad22", "", 0.5, 0.5, 1.0, 0.95, 0);
    Pad[23] = new TPad("hcalpad23", "", 0.0, 0.0, 0.5, 0.5, 0);

    Pad[21]->Draw();
    Pad[22]->Draw();
    Pad[23]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
    transparent[7]->SetFillStyle(4000);
    transparent[7]->Draw();
    TC[7]->SetEditable(false);
  }
  else if (name == "HcalMon7"){
    TC[9] = new TCanvas(name.c_str(),"Channel unsuppressed event fraction ", -xsize/2 , 0, xsize/2, ysize*0.9);
    gSystem->ProcessEvents();
    Pad[24]=new TPad("hcalpad24","who needs this?",0.00,0.3,1.00,0.95);
    Pad[24]->SetRightMargin(0.15);
    Pad[24]->Draw();
    Pad[25] = new TPad("hcalpad24", "1d zs rate", 0.0, 0.0, 1.00, 0.3);
    Pad[25]->SetRightMargin(0.15);
    Pad[25]->Draw();
    transparent[9] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
    transparent[9]->SetFillStyle(4000);
    transparent[9]->Draw();
    TC[9]->SetEditable(0);


  }
  else if (name == "HcalPopUp")
  {
    TC[4] = new TCanvas(name.c_str(), "!!!DO NOT CLOSE!!! OR THE CODE WILL CRASH!!!!(Maybe not...)", 2 * xsize / 3, 0, xsize / 2, 2 * ysize / 3);
    gSystem->ProcessEvents();
    Pad[9] = new TPad("hcalpad9", "for single tower running mean", 0.0, 0.0, 1, 0.92, 0);
    Pad[9]->Draw();
    transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(false);
  }
  else if (name == "HcalServerStats")
  {
    TC[8] = new TCanvas(name.c_str(), "HcalMon Server Stats", 2 * xsize / 3, 0, 2 * xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[8] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[8]->Draw();
    transparent[8]->SetFillColor(kGray);
    TC[8]->SetEditable(false);
  }
  return 0;
}

int HcalMonDraw::Draw(const std::string& what)
{
  int iret = 0;
  int idraw = 0;
  int isuccess = 0;
  if (what == "ALL" || what == "FIRST")
  {
    int retcode = DrawFirst(what);
    iret += retcode;
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (what == "ALL" || what == "SECOND")
  {
    int retcode = DrawSecond(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (what == "ALL" || what == "THIRD")
  {
    int retcode = DrawThird(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  /*
  if (what == "ALL" || what == "FOURTH")
  {
    int retcode = DrawFourth(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  */
  if (what == "ALL" || what == "FIFTH")
  {
    int retcode = DrawFifth(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (what == "ALL" || what == "SIXTH")
  {
    int retcode = DrawSixth(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (what == "ALL" || what == "SEVENTH")
  {
    int retcode = DrawSeventh(what);
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (what == "ALL" || what == "SERVERSTATS")
  {
    int retcode = DrawServerStats();
    if (!retcode)
    {
      isuccess++;
    }
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  if (!isuccess)
  {
    return iret;
  }
  else
  {
    return 0;
  }
}

int HcalMonDraw::DrawFirst(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2D* hist1 = (TH2D*) cl->getHisto(HCALMON_0, "h2_hcal_rm");
 
  TH2F* hist1_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_rm");

  if (!gROOT->FindObject("HcalMon1"))
  {
    MakeCanvas("HcalMon1");
  }
  if (!hist1)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    if (isHtml())
    {
      delete TC[0];
      TC[0] = nullptr;
    }
    return -1;
  }



  hist1->Add(hist1_1);
  hist1->Divide(h2_mean_template);
  // h_event->Add(h_event_1);


  // h2_hcal_mean->Scale(1. / h_event->GetEntries());
  // hist1->Divide(h2_hcal_mean);

  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  Pad[0]->cd();

  hist1->GetXaxis()->SetTitle("eta index");
  hist1->GetYaxis()->SetTitle("phi index");
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->CenterTitle();
  hist1->GetXaxis()->SetNdivisions(24);
  hist1->GetYaxis()->SetNdivisions(232);

  float tsize = 0.025;
  hist1->GetXaxis()->SetLabelSize(tsize);
  hist1->GetYaxis()->SetLabelSize(tsize);
  hist1->GetZaxis()->SetLabelSize(tsize);
  hist1->GetXaxis()->SetTitleSize(tsize);
  hist1->GetYaxis()->SetTitleSize(tsize);
  hist1->GetXaxis()->SetTickLength(0.02);

  hist1->GetZaxis()->SetRangeUser(0, 4);

  TLine* line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
  }
  TLine* line_board1 = new TLine(8, 0, 8, 64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(4);
  line_board1->SetLineStyle(1);
  TLine* line_board2 = new TLine(16, 0, 16, 64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(4);
  line_board2->SetLineStyle(1);

  TLine* line_iphi[64];
  for (int i_line = 0; i_line < 64; i_line++)
  {
    line_iphi[i_line] = new TLine(0, (i_line + 1), 24, (i_line + 1));
    line_iphi[i_line]->SetLineColor(1);
    line_iphi[i_line]->SetLineWidth(1);
    line_iphi[i_line]->SetLineStyle(1);
  }
  TLine* line_ieta[64];
  for (int i_line = 0; i_line < 24; i_line++)
  {
    line_ieta[i_line] = new TLine((i_line + 1), 0, (i_line + 1), 64);
    line_ieta[i_line]->SetLineColor(1);
    line_ieta[i_line]->SetLineWidth(1);
    line_ieta[i_line]->SetLineStyle(1);
  }

  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.07);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.11);

  hist1->Draw("colz");
  for (auto& i_line : line_sector)
  {
    i_line->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();

  for (auto& i_line : line_iphi)
  {
    i_line->Draw();
  }
  for (int i_line = 0; i_line < 24; i_line++)
  {
    line_ieta[i_line]->Draw();
  }

  Int_t palette[3] = {1, 8, 2};
  hcalStyle->SetPalette(3, palette);
  gROOT->SetStyle("hcalStyle");
  gROOT->ForceStyle();
  gStyle->SetPalette(3, palette);
  double_t levels[4] = {0, 0.5, 2, 4};
  hist1->SetContour(4, levels);

  FindHotTower(warning[0], hist1);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": tower occupancy(threshold 30ADC) running mean divided by template";
  runnostream2 << "Run" << cl->RunNumber() << ", Time: " << ctime(&evttime);
  transparent[0]->cd();
  runstring = runnostream.str();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());
  /*
  // make a TButton at bottom left to pop up a window with average ADC for all tower
  TButton* but1 = new TButton("Draw Template", "", 0.01, 0.01, 0.5, 0.05);
  but1->SetName("avgenergy");
  TC[0]->cd();
  but1->Draw();
  TButton* but2 = new TButton("Draw Multiplicity", "", 0.51, 0.01, 0.99, 0.05);
  but2->SetName("hitmap");
  but2->Draw();
  TButton* but3 = new TButton("Draw Avg. Time", "", 0.01, 0.06, 0.5, 0.1);
  but3->SetName("avgtime");
  but3->Draw();
  */
  // this connects the clicks on TCavas to the HandleEvent method that makes a pop up window
  // and display the running mean history of the tower correponding to the bin you click on
  TC[0]->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "HcalMonDraw", this,
                 "HandleEvent(int,int,int,TObject*)");

  TC[0]->Update();
  
  TC[0]->Show();

  TC[0]->SetEditable(false);

  // TC[0]->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "TCanvas", TC[0],
  //           "Paint()");
  return 0;
}

int HcalMonDraw::DrawSecond(const std::string& /* what */)
{
  const int Nsector = 32;
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH1F* h_sectorAvg_total = (TH1F*) cl->getHisto(HCALMON_0, "h_sectorAvg_total");
  TH1F* h_event = (TH1F*) cl->getHisto(HCALMON_0, "h_event");
  TH1F* h_sectorAvg_total_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_sectorAvg_total");
  TH1F* h_event_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_event");
  TH1F* h_rm_sectorAvg[Nsector];
  TH1F* h_rm_sectorAvg_1[Nsector];
  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih] = (TH1F*) cl->getHisto(HCALMON_0, Form("h_rm_sectorAvg_s%d", ih));
    h_rm_sectorAvg_1[ih] = (TH1F*) cl->getHisto(HCALMON_1, Form("h_rm_sectorAvg_s%d", ih));
    h_rm_sectorAvg[ih]->Add(h_rm_sectorAvg_1[ih]);
  }

  if (!gROOT->FindObject("HcalMon2"))
  {
    MakeCanvas("HcalMon2");
  }

  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (!h_rm_sectorAvg[0] || !h_event || !h_sectorAvg_total)
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    if (isHtml())
    {
      delete TC[1];
      TC[1] = nullptr;
    }
    return -1;
  }

  h_sectorAvg_total->Scale(1. / h_event->GetEntries());
  h_sectorAvg_total_1->Scale(1. / h_event_1->GetEntries());
  h_sectorAvg_total->Add(h_sectorAvg_total_1);

  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih]->Scale(1. / h_sectorAvg_total->GetBinContent(ih + 1));
    for (int ib = 1; ib < h_rm_sectorAvg[ih]->GetNbinsX(); ib++)
    {
      h_rm_sectorAvg[ih]->SetBinContent(ib, ih + h_rm_sectorAvg[ih]->GetBinContent(ib));
    }
  }

  gStyle->SetTitleFontSize(0.03);

  gStyle->SetOptStat(0);

  TH1F* frame = new TH1F("frame", "", 100, 0, 100);
  frame->Draw("AXIS");
  frame->GetXaxis()->SetTitle("time");
  frame->GetYaxis()->SetTitle("sector running mean / template + sector #");
  frame->GetXaxis()->CenterTitle();
  frame->GetYaxis()->CenterTitle();
  // frame->GetXaxis()->SetNdivisions(20);
  // frame->GetYaxis()->SetNdivisions(232);
  float tsize = 0.03;
  frame->GetXaxis()->SetLabelSize(tsize);
  frame->GetYaxis()->SetLabelSize(tsize);
  frame->GetXaxis()->SetTitleSize(tsize);
  frame->GetYaxis()->SetTitleSize(tsize);

  frame->GetXaxis()->SetRangeUser(0, 100);
  frame->GetYaxis()->SetRangeUser(0, 32.75);

  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetTopMargin(0.01);

  for (auto& ih : h_rm_sectorAvg)
  {
    ih->Draw("same hist ][");
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << "_running mean, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);
  return 0;
}

int HcalMonDraw::DrawThird(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);
  // TH1F* h_waveform_twrAvg = (TH1F*) cl->getHisto(HCALMON_0, "h_waveform_twrAvg");
  TH1F* h_waveform_time = (TH1F*) cl->getHisto(HCALMON_0, "h_waveform_time");
  TH1F* h_waveform_pedestal = (TH1F*) cl->getHisto(HCALMON_0, "h_waveform_pedestal");
  TH2F* h2_hcal_waveform = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_waveform");
  // TH1F* hwaveform_twrAvg_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_waveform_twrAvg");
  TH1F* hwaveform_time_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_waveform_time");
  TH1F* hwaveform_pedestal_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_waveform_pedestal");
  TH2F* h2_hcal_waveform_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_waveform");

  if (!gROOT->FindObject("HcalMon3"))
  {
    MakeCanvas("HcalMon3");
  }

  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  Pad[6]->cd();
  if (!h2_hcal_waveform || !h_waveform_time || !h_waveform_pedestal)
  {
    DrawDeadServer(transparent[3]);
    TC[3]->SetEditable(false);
    if (isHtml())
    {
      delete TC[3];
      TC[3] = nullptr;
    }
    return -1;
  }

  // h_waveform_twrAvg->Add(hwaveform_twrAvg_1);
  h_waveform_time->Add(hwaveform_time_1);
  h_waveform_pedestal->Add(hwaveform_pedestal_1);
  h2_hcal_waveform->Add(h2_hcal_waveform_1);

  //Tprofie of h2_hcal_waveform 
  float ymaxp = h2_hcal_waveform->ProfileX()->GetMaximum();
  TProfile* profile_y = h2_hcal_waveform->ProfileY();
  profile_y->Rebin(5);

    // Define the range
    double x_min = 100;
    double x_max = 7 * ymaxp;
    int n_points_in_range = 0;
    // Loop through the bins of the Profile Y histogram and count the bins within the range
    int n_bins = profile_y->GetNbinsX();
    for (int i = 1; i <= n_bins; ++i) {
        double bin_center = profile_y->GetBinCenter(i);
        if (profile_y->GetBinContent(i) == 0) {
            continue;
        }
        if (bin_center >= x_min && bin_center <= x_max) {
            n_points_in_range++;
        }
    }

    double* x_vals = new double[n_points_in_range];
    double* y_vals = new double[n_points_in_range];

    // Extract the Profile Y values and their corresponding Y positions within the range
    int point_index = 0;
    for (int i = 1; i <= n_bins; ++i) {
        double bin_center = profile_y->GetBinCenter(i);
        if (profile_y->GetBinContent(i) == 0) {
            continue;
        }
        if (bin_center >= x_min && bin_center <= x_max) {
            y_vals[point_index] = bin_center;
            x_vals[point_index] = profile_y->GetBinContent(i);
            point_index++;
        }
    }



 

  Pad[6]->cd();
  gStyle->SetTitleFontSize(0.03);
  
  h2_hcal_waveform->GetYaxis()->SetRangeUser(0, ymaxp * 20);

  h2_hcal_waveform->Draw("colz");

  float tsize = 0.06;
  h2_hcal_waveform->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_waveform->GetXaxis()->SetRangeUser(0, 16);
  h2_hcal_waveform->GetXaxis()->SetTitle("Sample #");
  h2_hcal_waveform->GetYaxis()->SetTitle("Waveform [ADC]");
  h2_hcal_waveform->GetXaxis()->SetLabelSize(tsize);
  h2_hcal_waveform->GetYaxis()->SetLabelSize(tsize);
  h2_hcal_waveform->GetXaxis()->SetTitleSize(tsize);
  h2_hcal_waveform->GetYaxis()->SetTitleSize(tsize);
  h2_hcal_waveform->GetXaxis()->SetTitleOffset(1.2);
  h2_hcal_waveform->GetYaxis()->SetTitleOffset(0.85);
  //over lay the profile draw only the marker
  TGraph* graph = new TGraph(n_points_in_range, x_vals, y_vals);
  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1);
  graph->SetMarkerColor(1);
  graph->Draw("P same");

  //draw two black lines for the okay timing range
  TLine* line1 = new TLine(4.5, 0, 4.5, ymaxp * 20);
  line1->SetLineColor(1);
  line1->SetLineWidth(3);
  line1->SetLineStyle(1);
  line1->Draw();
  TLine* line2 = new TLine(7.5, 0, 7.5, ymaxp * 20);
  line2->SetLineColor(1);
  line2->SetLineWidth(3);
  line2->SetLineStyle(1);
  line2->Draw();

  gPad->SetLogz();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
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
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Pulse fitting, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[3]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  Pad[7]->cd();

  gStyle->SetTitleFontSize(0.06);

  float tsize2 = 0.08;
  h_waveform_time->Draw("hist");
  h_waveform_time->GetXaxis()->SetNdivisions(510, kTRUE);
  h_waveform_time->GetXaxis()->SetRangeUser(0, 16);
  h_waveform_time->GetXaxis()->SetTitle("Sample #");
  h_waveform_time->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_time->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_time->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_time->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_time->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_time->GetXaxis()->SetTitleOffset(1.0);
  h_waveform_time->GetYaxis()->SetTitleOffset(0.85);
  h_waveform_time->SetFillColorAlpha(kBlue, 0.1);
  if(h_waveform_time->GetEntries()){
    h_waveform_time->Scale(1. / h_waveform_time->GetEntries());
  }
  gPad->Update();
  //draw two black lines for the okay timing range
  TLine* line3 = new TLine(4.5, 0, 4.5,  gPad->GetFrame()->GetY2());
  line3->SetLineColor(1);
  line3->SetLineWidth(3);
  line3->SetLineStyle(1);
  line3->Draw();
  TLine* line4 = new TLine(7.5, 0, 7.5,  gPad->GetFrame()->GetY2());
  line4->SetLineColor(1);
  line4->SetLineWidth(3);
  line4->SetLineStyle(1);
  line4->Draw();
  //draw a red line at mean x
  TLine* line5 = new TLine(h_waveform_time->GetMean(), 0, h_waveform_time->GetMean(),  gPad->GetFrame()->GetY2());
  line5->SetLineColor(2);
  line5->SetLineWidth(3);
  line5->SetLineStyle(1);
  line5->Draw();


  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[8]->cd();

  gStyle->SetTitleFontSize(0.06);

  h_waveform_pedestal->Draw("hist");
  h_waveform_pedestal->GetXaxis()->SetNdivisions(505, kTRUE);
  h_waveform_pedestal->GetXaxis()->SetTitle("ADC Pedestal");
  h_waveform_pedestal->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_pedestal->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_pedestal->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_pedestal->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_pedestal->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_pedestal->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_pedestal->GetYaxis()->SetTitleOffset(0.85);
  h_waveform_pedestal->SetFillColorAlpha(kBlue, 0.1);
  if (h_waveform_pedestal->GetEntries())
  {
    h_waveform_pedestal->Scale(1. / h_waveform_pedestal->GetEntries());
  }
  gPad->Update();
  TLine* line6 = new TLine(1000, 0, 1000,  gPad->GetFrame()->GetY2());
  line6->SetLineColor(1);
  line6->SetLineWidth(3);
  line6->SetLineStyle(1);
  line6->Draw();
  TLine* line7 = new TLine(2000, 0, 2000,  gPad->GetFrame()->GetY2());
  line7->SetLineColor(1);
  line7->SetLineWidth(3);
  line7->SetLineStyle(1);
  line7->Draw();


  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetLogy();
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);

  return 0;
}

int HcalMonDraw::DrawFourth(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);
  TH1F* h1_packet_number = (TH1F*) cl->getHisto(HCALMON_0, "h1_packet_number");
  TH1F* h1_packet_length = (TH1F*) cl->getHisto(HCALMON_0, "h1_packet_length");
  TH1F* h1_packet_chans = (TH1F*) cl->getHisto(HCALMON_0, "h1_packet_chans");
  TH1F* h1_packet_event = (TH1F*) cl->getHisto(HCALMON_0, "h1_packet_event");
  TH1F* h_event = (TH1F*) cl->getHisto(HCALMON_0, "h_event");
  TH2F* h2_hcal_correlation = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_correlation");

  TH1F* h1_packet_number_1 = (TH1F*) cl->getHisto(HCALMON_1, "h1_packet_number");
  TH1F* h1_packet_length_1 = (TH1F*) cl->getHisto(HCALMON_1, "h1_packet_length");
  TH1F* h1_packet_chans_1 = (TH1F*) cl->getHisto(HCALMON_1, "h1_packet_chans");
  TH1F* h1_packet_event_1 = (TH1F*) cl->getHisto(HCALMON_1, "h1_packet_event");
  TH2F* h2_hcal_correlation_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_correlation");
  TH1F* h_event_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_event");

  if (!gROOT->FindObject("HcalMon4"))
  {
    MakeCanvas("HcalMon4");
  }

  TC[5]->SetEditable(true);
  TC[5]->Clear("D");

  if (!h1_packet_number || !h1_packet_length || !h1_packet_chans || !h_event || !h1_packet_event || !h2_hcal_correlation)
  {
    // print out which is not found
    if (!h1_packet_number)
    {
      std::cout << "h1_packet_number not found" << std::endl;
    }
    if (!h1_packet_length)
    {
      std::cout << "h1_packet_length not found" << std::endl;
    }
    if (!h1_packet_chans)
    {
      std::cout << "h1_packet_chans not found" << std::endl;
    }
    if (!h_event)
    {
      std::cout << "h_event not found" << std::endl;
    }
    if (!h1_packet_event)
    {
      std::cout << "h1_packet_event not found" << std::endl;
    }
    if (!h2_hcal_correlation)
    {
      std::cout << "h2_hcal_correlation not found" << std::endl;
    }

    DrawDeadServer(transparent[5]);
    TC[5]->SetEditable(false);
    if (isHtml())
    {
      delete TC[5];
      TC[5] = nullptr;
    }
    return -1;
  }
  // h1_packet_number->Scale(1. / h_event->GetEntries());
  // h1_packet_length->Scale(1. / h_event->GetEntries());
  // h1_packet_chans->Scale(1. / h_event->GetEntries());

  // h1_packet_number_1->Scale(1. / h_event_1->GetEntries());
  // h1_packet_length_1->Scale(1. / h_event_1->GetEntries());
  // h1_packet_chans_1->Scale(1. / h_event_1->GetEntries());

  h1_packet_number->Add(h1_packet_number_1);
  h1_packet_length->Add(h1_packet_length_1);
  h1_packet_chans->Add(h1_packet_chans_1);

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
  int maxy1 = h1_packet_event_1->GetMaximum();
  for (int i = 1; i <= h1_packet_event_1->GetNbinsX(); i++)
  {
    if (h1_packet_event_1->GetBinContent(i) != 0)
    {
      h1_packet_event_1->SetBinContent(i, h1_packet_event_1->GetBinContent(i) - maxy1);
    }
  }

  /*
    if (maxbin < 3|| maxbin > 6){
    //substract bin 1,2 ,7 8 by maxy
    h1_packet_event->SetBinContent(1, h1_packet_event->GetBinContent(1) - maxy);
    h1_packet_event->SetBinContent(2, h1_packet_event->GetBinContent(2) - maxy);
    h1_packet_event->SetBinContent(7, h1_packet_event->GetBinContent(7) - maxy);
    h1_packet_event->SetBinContent(8, h1_packet_event->GetBinContent(8) - maxy);

    h1_packet_event_1->SetBinContent(3, h1_packet_event_1->GetBinContent(3) - maxy1);
    h1_packet_event_1->SetBinContent(4, h1_packet_event_1->GetBinContent(4) - maxy1);
    h1_packet_event_1->SetBinContent(5, h1_packet_event_1->GetBinContent(5) - maxy1);
    h1_packet_event_1->SetBinContent(6, h1_packet_event_1->GetBinContent(6) - maxy1);
    }
    else{
    //substract bin 3,4,5,6 by maxy
    h1_packet_event->SetBinContent(3, h1_packet_event->GetBinContent(3) - maxy);
    h1_packet_event->SetBinContent(4, h1_packet_event->GetBinContent(4) - maxy);
    h1_packet_event->SetBinContent(5, h1_packet_event->GetBinContent(5) - maxy);
    h1_packet_event->SetBinContent(6, h1_packet_event->GetBinContent(6) - maxy);

    h1_packet_event_1->SetBinContent(1, h1_packet_event_1->GetBinContent(1) - maxy1);
    h1_packet_event_1->SetBinContent(2, h1_packet_event_1->GetBinContent(2) - maxy1);
    h1_packet_event_1->SetBinContent(7, h1_packet_event_1->GetBinContent(7) - maxy1);
    h1_packet_event_1->SetBinContent(8, h1_packet_event_1->GetBinContent(8) - maxy1);
    }
  */
  h1_packet_event->Add(h1_packet_event_1);

  // find the x range for h1_packet_number
  double xmin = h1_packet_number->GetXaxis()->GetXmin();
  double xmax = h1_packet_number->GetXaxis()->GetXmax();

  TLine* one = new TLine(xmin, 1, xmax, 1);
  one->SetLineStyle(7);

  TLine* goodSize = new TLine(xmin, 5981, xmax, 5981);
  goodSize->SetLineStyle(7);

  TLine* goodChans = new TLine(xmin, 192, xmax, 192);
  goodChans->SetLineStyle(7);

  // float param = 0.75;
  float param = 0.95;

  TLegend* leg = new TLegend(0.3, 0.16, 0.95, 0.4);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  TLine* warnLineOne = new TLine(xmin, param * 1, xmax, param * 1);
  warnLineOne->SetLineStyle(7);
  warnLineOne->SetLineColor(2);

  leg->AddEntry(warnLineOne, "95% Threshold", "l");

  TLine* warnLineSize = new TLine(xmin, param * 5981., xmax, param * 5981.);
  warnLineSize->SetLineStyle(7);
  warnLineSize->SetLineColor(2);

  TLine* warnLineChans = new TLine(xmin, param * 192., xmax, param * 192.);
  warnLineChans->SetLineStyle(7);
  warnLineChans->SetLineColor(2);

  Pad[10]->cd();
  float tsize = 0.08;
  h1_packet_number->GetYaxis()->SetRangeUser(0.0, 1.3);
  h1_packet_number->Draw("hist");
  // std::vector<std::vector<int>> badPackets;
  // badPackets.push_back(getBadPackets(h1_packet_number, 0, param));
  one->Draw("same");
  warnLineOne->Draw("same");
  h1_packet_number->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_number->GetXaxis()->SetTitle("Packet #");
  h1_packet_number->GetYaxis()->SetTitle("% Of Events Present");
  h1_packet_number->GetXaxis()->SetLabelSize(tsize - 0.01);
  h1_packet_number->GetYaxis()->SetLabelSize(tsize - 0.01);
  h1_packet_number->GetXaxis()->SetTitleSize(tsize - 0.01);
  h1_packet_number->GetYaxis()->SetTitleSize(tsize - 0.01);
  h1_packet_number->GetXaxis()->SetTitleOffset(1);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[11]->cd();
  h1_packet_length->Draw("hist");
  h1_packet_length->GetYaxis()->SetRangeUser(0, 6500);
  // badPackets.push_back(getBadPackets(h1_packet_length, 1, param));
  goodSize->Draw("same");
  warnLineSize->Draw("same");
  leg->Draw("same");
  h1_packet_length->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_length->GetXaxis()->SetTitle("Packet #");
  h1_packet_length->GetYaxis()->SetTitle("Average Packet Size");
  h1_packet_length->GetXaxis()->SetLabelSize(tsize - .01);
  h1_packet_length->GetYaxis()->SetLabelSize(tsize);
  h1_packet_length->GetXaxis()->SetTitleSize(tsize - .01);
  h1_packet_length->GetYaxis()->SetTitleSize(tsize);
  h1_packet_length->GetXaxis()->SetTitleOffset(1);
  h1_packet_length->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[12]->cd();
  h1_packet_chans->Draw("hist");
  h1_packet_chans->GetYaxis()->SetRangeUser(0, 212);
  // badPackets.push_back(getBadPackets(h1_packet_chans, 2, param));
  goodChans->Draw("same");
  warnLineChans->Draw("same");
  h1_packet_chans->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_chans->GetXaxis()->SetTitle("Packet #");
  h1_packet_chans->GetYaxis()->SetTitle("Average # of Channels");
  h1_packet_chans->GetXaxis()->SetLabelSize(tsize - .01);
  h1_packet_chans->GetYaxis()->SetLabelSize(tsize);
  h1_packet_chans->GetXaxis()->SetTitleSize(tsize - .01);
  h1_packet_chans->GetYaxis()->SetTitleSize(tsize);
  h1_packet_chans->GetXaxis()->SetTitleOffset(0.8);
  h1_packet_chans->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[13]->cd();
  h1_packet_event->Draw("hist");
  double ymax = h1_packet_event->GetMaximum();
  double ymin = h1_packet_event->GetMinimum();

  h1_packet_event->GetYaxis()->SetRangeUser(ymin - 0.3 * (ymax - ymin + 30), ymax + 0.3 * (ymax - ymin + 30));
  h1_packet_event->GetXaxis()->SetTitle("Packet #");
  h1_packet_event->GetYaxis()->SetTitle("clock offset");
  h1_packet_event->GetXaxis()->SetLabelSize(tsize - .01);
  h1_packet_event->GetYaxis()->SetLabelSize(tsize);
  h1_packet_event->GetXaxis()->SetTitleSize(tsize - .01);
  h1_packet_event->GetYaxis()->SetTitleSize(tsize);
  h1_packet_event->GetXaxis()->SetTitleOffset(0.8);
  h1_packet_event->GetYaxis()->SetTitleOffset(1.2);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[14]->cd();
  h2_hcal_correlation->Draw("colz");
  ymax = h2_hcal_correlation->ProfileX()->GetMaximum();
  xmax = h2_hcal_correlation->ProfileY()->GetMaximum();
  h2_hcal_correlation->GetYaxis()->SetRangeUser(0, ymax * 1.2);
  h2_hcal_correlation->GetXaxis()->SetRangeUser(0, xmax * 1.2);
  h2_hcal_correlation->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_correlation->GetYaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_correlation->GetXaxis()->SetTitle("Total West OHCal Energy[a.u.]");
  h2_hcal_correlation->GetYaxis()->SetTitle("Total West IHCal Energy[a.u.]");
  h2_hcal_correlation->GetXaxis()->SetLabelSize(0);
  h2_hcal_correlation->GetYaxis()->SetLabelSize(0);
  h2_hcal_correlation->GetXaxis()->SetTitleSize(tsize - .02);
  h2_hcal_correlation->GetYaxis()->SetTitleSize(tsize - .02);
  h2_hcal_correlation->GetXaxis()->SetTitleOffset(0.8);
  h2_hcal_correlation->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.03);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[15]->cd();
  h2_hcal_correlation_1->Draw("colz");
  ymax = h2_hcal_correlation_1->ProfileX()->GetMaximum();
  xmax = h2_hcal_correlation_1->ProfileY()->GetMaximum();
  h2_hcal_correlation_1->GetYaxis()->SetRangeUser(0, ymax * 1.2);
  h2_hcal_correlation_1->GetXaxis()->SetRangeUser(0, xmax * 1.2);
  h2_hcal_correlation_1->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_correlation_1->GetYaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_correlation_1->GetXaxis()->SetTitle("Total East OHCal Energy[a.u.]");
  h2_hcal_correlation_1->GetYaxis()->SetTitle("Total East IHCal Energy[a.u.]");
  h2_hcal_correlation_1->GetXaxis()->SetLabelSize(0);
  h2_hcal_correlation_1->GetYaxis()->SetLabelSize(0);
  h2_hcal_correlation_1->GetXaxis()->SetTitleSize(tsize - .02);
  h2_hcal_correlation_1->GetYaxis()->SetTitleSize(tsize - .02);
  h2_hcal_correlation_1->GetXaxis()->SetTitleOffset(0.8);
  h2_hcal_correlation_1->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  // draw the warning here:
  warning[1]->cd();
  std::vector<int> badPackets;
  std::vector<std::string> whatswrong;
  for (int i = 1; i <= 8; i++)
  {
    bool missing = false;
    bool badnumber = false;
    bool badlength = false;
    bool badchans = false;
    if (h1_packet_number->GetBinContent(i) == 0)
    {
      missing = true;
    }
    if (h1_packet_number->GetBinContent(i) < param)
    {
      badnumber = true;
    }
    if (h1_packet_length->GetBinContent(i) < param * 5981.)
    {
      badlength = true;
    }
    if (h1_packet_chans->GetBinContent(i) < param * 192.)
    {
      badchans = true;
    }
    if (badnumber || badlength || badchans || missing)
    {
      badPackets.push_back((int) h1_packet_number->GetBinCenter(i));
      std::string reason = "";
      if (missing)
      {
        reason += "packet lost! ";
      }
      else
      {
        if (badnumber)
        {
          reason += "some events are missing, ";
        }
        if (badlength)
        {
          reason += "too short, ";
        }
        if (badchans)
        {
          reason += "too few channels, ";
        }
        // remove the last two characters
        reason = reason.substr(0, reason.size() - 2);
        reason += ".";
      }
      whatswrong.push_back(reason);
    }
  }
  bool westmismatch = false;
  bool eastmismatch = false;
  for (int i = 1; i <= h1_packet_event->GetNbinsX(); i++)
  {
    if (h1_packet_event->GetBinContent(i) != 0)
    {
      if (i < 3 || i > 6)
      {
        westmismatch = true;
      }
      else
      {
        eastmismatch = true;
      }
    }
  }
  // draw the mismatch warning on the pad
  TText mismatchWarn;
  mismatchWarn.SetTextFont(62);
  mismatchWarn.SetTextSize(0.06);
  mismatchWarn.SetTextColor(2);
  mismatchWarn.SetNDC();
  mismatchWarn.SetTextAlign(23);
  if (westmismatch)
  {
    mismatchWarn.DrawText(0.5, 0.95, "West misaligned!");
  }
  if (eastmismatch)
  {
    mismatchWarn.DrawText(0.5, 0.9, "East misaligned!");
  }
  mismatchWarn.SetTextColor(1);
  mismatchWarn.SetTextSize(0.05);
  if (westmismatch || eastmismatch)
  {
    // mismatchWarn.DrawText(0.5, 0.7, "Restart the run and see if this persists.");
    //  draw a line for seperation
    TLine* line = new TLine(0., 0.8, 1., 0.8);
    line->SetLineColor(1);
    line->SetLineStyle(1);
    line->SetLineWidth(10);
    line->Draw("same");
  }

  // draw the bad packet warning here
  TText PacketWarn;
  PacketWarn.SetTextFont(62);
  PacketWarn.SetTextSize(0.04);
  PacketWarn.SetTextColor(1);
  PacketWarn.SetNDC();
  PacketWarn.SetTextAlign(23);
  PacketWarn.DrawText(0.5, 0.75, "Bad Packets:");
  for (int i = 0; i < (int) badPackets.size(); i++)
  {
    PacketWarn.DrawText(0.5, 0.7 - 0.05 * i, Form("%i: %s", badPackets[i], whatswrong[i].c_str()));
  }
  if ((int) badPackets.size() > 0 && (h_event->GetEntries() > 1000 || h_event_1->GetEntries() > 1000))
  {
    PacketWarn.SetTextSize(0.04);
    PacketWarn.DrawText(0.5, 0.7 - 0.05 * (int) badPackets.size(), "Check with HCal experts.");
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.02);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::ostringstream runnostream2;
  time_t evttime = getTime();
  // fill run number and event time into string

  runnostream << "Packet Information";
  runnostream2 << " Run " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  transparent[5]->cd();

  runstring = runnostream.str();
  PrintRun.DrawText(0.5, .99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, .966, runstring.c_str());
  TC[5]->Update();
  TC[5]->Show();
  TC[5]->SetEditable(false);
  // if (save) TC[5]->SaveAs("plots/packets.pdf");
  return 0;
}

int HcalMonDraw::FindHotTower(TPad* warningpad, TH2* hhit)
{
  int nhott = 0;
  int ndeadt = 0;
  int displaylimit = 15;
  // get histogram
  std::ostringstream hottowerlist;
  std::ostringstream deadtowerlist;
  float hot_threshold = 2;
  float dead_threshold = 0.5;

  for (int ieta = 0; ieta < 24; ieta++)
  {
    for (int iphi = 0; iphi < 64; iphi++)
    {
      double nhit = hhit->GetBinContent(ieta + 1, iphi + 1);

      if (nhit > hot_threshold)
      {
        if (nhott <= displaylimit)
        {
          hottowerlist << " (" << ieta << "," << iphi << ")";
        }
        nhott++;
      }

      if (nhit < dead_threshold)
      {
        if (ndeadt <= displaylimit)
        {
          deadtowerlist << " (" << ieta << "," << iphi << ")";
        }
        ndeadt++;
      }
    }
  }

  if (nhott > displaylimit)
  {
    hottowerlist << "... " << nhott << " total";
  }
  if (ndeadt > displaylimit)
  {
    deadtowerlist << "... " << ndeadt << " total";
  }
  if(nhott ==0){
    hottowerlist << " None";
  }
  if(ndeadt ==0){
    deadtowerlist << " None";
  }

  // draw warning here
  warningpad->cd();
  TText Warn;
  Warn.SetTextFont(62);
  Warn.SetTextSize(0.06);
  Warn.SetTextColor(2);
  Warn.SetNDC();
  Warn.SetTextAlign(23);
  Warn.DrawText(0.5, 1, "Hot towers:");
  Warn.DrawText(0.5, 0.9, hottowerlist.str().c_str());

  Warn.SetTextColor(4);
  Warn.SetTextAlign(22);
  Warn.DrawText(0.5, 0.7, "Dead towers:");
  Warn.DrawText(0.5, 0.6, deadtowerlist.str().c_str());

  warningpad->Update();
  return 0;
}

// int HcalMonDraw::DrawSecond(const std::string & /* what */)
/*
  {
  OnlMonClient *cl = OnlMonClient::instance();
  TH2D* hist1 = (TH2D*)cl->getHisto("h2_hcal_rm");

  if (!gROOT->FindObject("HcalMon2"))
  {
  MakeCanvas("HcalMon2");
  }


  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (!hist1)
  {
  DrawDeadServer(transparent[1]);
  TC[1]->SetEditable(0);
  if (isHtml())
  {
  delete TC[1];
  TC[1] = nullptr;
  }
  return -1;
  }



  gStyle->SetTitleFontSize(0.03);

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);


  hist1->GetXaxis()->SetTitle("ieta");
  hist1->GetYaxis()->SetTitle("iphi");
  hist1->GetXaxis()->SetTitleSize(0.025);
  hist1->GetYaxis()->SetTitleSize(0.025);
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->CenterTitle();
  hist1->GetXaxis()->SetNdivisions(24);
  hist1->GetYaxis()->SetNdivisions(232);
  hist1->GetXaxis()->SetLabelSize(0.02);
  hist1->GetYaxis()->SetLabelSize(0.02);
  hist1->GetZaxis()->SetLabelSize(0.018);


  TLine *line_sector[32];
  for(int i_line=0;i_line<32;i_line++)
  {
  line_sector[i_line] = new TLine(0,(i_line+1)*2,24,(i_line+1)*2);
  line_sector[i_line]->SetLineColor(1);
  line_sector[i_line]->SetLineWidth(1.2);
  line_sector[i_line]->SetLineStyle(1);
  }
  TLine *line_board1 = new TLine(8,0,8,64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(1.2);
  line_board1->SetLineStyle(1);
  TLine *line_board2 = new TLine(16,0,16,64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(1.2);
  line_board2->SetLineStyle(1);

  gPad->SetTopMargin(0.04);
  gPad->SetBottomMargin(0.06);
  gPad->SetLeftMargin(0.06);
  gPad->SetRightMargin(0.11);
  gPad->SetTickx();
  gPad->SetTicky();

  hist1->Draw("colz");
  for(int i_line=0;i_line<32;i_line++)
  {
  line_sector[i_line]->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << "_running mean, Run" << cl->RunNumber()
  << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(0);
  return 0;
  }
*/

int HcalMonDraw::SavePlot(const std::string& what, const std::string& type)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
    return iret;
  }
  int icnt = 0;
  for (TCanvas* canvas : TC)
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

int HcalMonDraw::MakeHtml(const std::string& what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient* cl = OnlMonClient::instance();
  int icnt = 0;
  for (TCanvas* canvas : TC)
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

// this is a method that draw the tower average energy
void HcalMonDraw::DrawTowerAvg()
{
  OnlMonClient* cl = OnlMonClient::instance();
  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2D* h2_hcal_mean = (TH2D*) cl->getHisto(HCALMON_0, "h2_hcal_mean");
  // TH1F* h_event = (TH1F*) cl->getHisto(HCALMON_0, "h_event");
  // TH2D* h2_hcal_mean_1 = (TH2D*) cl->getHisto(HCALMON_1, "h2_hcal_mean");
  // TH1F* h_event_1 = (TH1F*) cl->getHisto(HCALMON_1, "h_event");

  if (!gROOT->FindObject("HcalPopUp"))
  {
    MakeCanvas("HcalPopUp");
  }
  if (!h2_hcal_mean)
  {
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    if (isHtml())
    {
      delete TC[4];
      TC[4] = nullptr;
    }
    return;
  }

  // h2_hcal_mean->Scale(1. / h_event->GetEntries());
  // h2_hcal_mean_1->Scale(1. / h_event_1->GetEntries());
  // h2_hcal_mean->Add(h2_hcal_mean_1);

  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  Pad[9]->cd();

  // gPad->SetLogz();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  h2_hcal_mean->GetXaxis()->SetTitle("eta index");
  h2_hcal_mean->GetYaxis()->SetTitle("phi index");
  h2_hcal_mean->SetTitle("Tower Average Energy[ADC]");
  h2_hcal_mean->Draw("COLZ");
  // lines
  TLine* line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
  }
  TLine* line_board1 = new TLine(8, 0, 8, 64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(4);
  line_board1->SetLineStyle(1);
  TLine* line_board2 = new TLine(16, 0, 16, 64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(4);
  line_board2->SetLineStyle(1);

  for (auto& i_line : line_sector)
  {
    i_line->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();

  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);
}

// this is a method that draw the tower hit map
void HcalMonDraw::DrawHitMap()
{
  OnlMonClient* cl = OnlMonClient::instance();
  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2D* h2_hcal_hits = (TH2D*) cl->getHisto(HCALMON_0, "h2_hcal_hits");

  if (!gROOT->FindObject("HcalPopUp"))
  {
    MakeCanvas("HcalPopUp");
  }
  if (!h2_hcal_hits)
  {
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    if (isHtml())
    {
      delete TC[4];
      TC[4] = nullptr;
    }
    return;
  }

  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  Pad[9]->cd();
  gPad->SetLogz(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  h2_hcal_hits->GetXaxis()->SetTitle("eta index");
  h2_hcal_hits->GetYaxis()->SetTitle("phi index");
  h2_hcal_hits->SetTitle("Average Multiplicity");
  h2_hcal_hits->Draw("COLZ");
  // lines
  TLine* line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
  }
  TLine* line_board1 = new TLine(8, 0, 8, 64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(4);
  line_board1->SetLineStyle(1);
  TLine* line_board2 = new TLine(16, 0, 16, 64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(4);
  line_board2->SetLineStyle(1);

  for (auto& i_line : line_sector)
  {
    i_line->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();
  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);
}

void HcalMonDraw::DrawAvgTime()
{
  OnlMonClient* cl = OnlMonClient::instance();
  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2D* h2_hcal_time = (TH2D*) cl->getHisto(HCALMON_0, "h2_hcal_time");

  if (!gROOT->FindObject("HcalPopUp"))
  {
    MakeCanvas("HcalPopUp");
  }
  if (!h2_hcal_time)
  {
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return;
  }

  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  Pad[9]->cd();
  gPad->SetLogz(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  h2_hcal_time->GetXaxis()->SetTitle("eta index");
  h2_hcal_time->GetYaxis()->SetTitle("phi index");
  h2_hcal_time->SetTitle("Average Time[samples]");
  h2_hcal_time->Draw("COLZ");
  // lines
  TLine* line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
  }
  TLine* line_board1 = new TLine(8, 0, 8, 64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(4);
  line_board1->SetLineStyle(1);
  TLine* line_board2 = new TLine(16, 0, 16, 64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(4);
  line_board2->SetLineStyle(1);

  for (auto& i_line : line_sector)
  {
    i_line->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();
  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);
}

// this is the method to idetify which bin you are clicking on and make the pop up window of TH1
void HcalMonDraw::HandleEvent(int event, int x, int y, TObject* selected)
{
  if (event == 1)
  {
    char HCALMON_0[100];
    sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
    char HCALMON_1[100];
    sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);
    // printf("Canvas %s: event=%d, x=%d, y=%d, selected=%s\n", "a",
    //        event, x, y, selected->IsA()->GetName());
    // std::cout << "selected->GetName()=" << selected->GetName() << std::endl;

    if (strcmp(selected->GetName(), "avgenergy") == 0)
    {
      DrawTowerAvg();
      return;
    }
    if (strcmp(selected->GetName(), "hitmap") == 0)
    {
      DrawHitMap();
      return;
    }
    // avg time
    if (strcmp(selected->GetName(), "avgtime") == 0)
    {
      DrawAvgTime();
      return;
    }

    double xx = Pad[0]->AbsPixeltoX(x);
    double xhis = Pad[0]->PadtoX(xx);
    int binx = (int) xhis;
    double yy = Pad[0]->AbsPixeltoY(y);
    double yhis = Pad[0]->PadtoY(yy);
    int biny = (int) yhis;
    // printf("ieta=%d, iphi=%d \n", binx, biny);
    if (binx < 0 || binx > 23)
    {
      return;
    }
    if (biny < 0 || biny > 63)
    {
      return;
    }

    OnlMonClient* cl = OnlMonClient::instance();

    TH1F* h_rm_tower = (TH1F*) cl->getHisto(HCALMON_0, Form("h_rm_tower_%d_%d", binx, biny));
    TH1F* h_rm_tower_1 = (TH1F*) cl->getHisto(HCALMON_1, Form("h_rm_tower_%d_%d", binx, biny));
    if (!gROOT->FindObject("HcalPopUp"))
    {
      MakeCanvas("HcalPopUp");
    }

    if (!h_rm_tower || !h_rm_tower_1)
    {
      DrawDeadServer(transparent[4]);
      TC[4]->SetEditable(false);
      return;
    }
    h_rm_tower->Add(h_rm_tower_1);
    h_rm_tower->SetXTitle("Time");
    h_rm_tower->SetYTitle("Running Mean");

    TC[4]->SetEditable(true);
    TC[4]->Clear("D");
    Pad[9]->cd();
    gStyle->SetOptStat(0);
    h_rm_tower->Draw();
    TC[4]->Update();
    TC[4]->Show();
    TC[4]->SetEditable(false);
  }
}




int HcalMonDraw::DrawFifth(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2F* h2_hcal_hits_trig1 = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits_trig1");
  TH2F* h2_hcal_hits_trig2 = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits_trig2");
  TH2F* h2_hcal_hits_trig3 = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits_trig3");
  TH2F* h2_hcal_hits_trig4 = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits_trig4");

  TH2F* h2_hcal_hits_trig1_2 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_hits_trig1");
  TH2F* h2_hcal_hits_trig2_2 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_hits_trig2");
  TH2F* h2_hcal_hits_trig3_2 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_hits_trig3");
  TH2F* h2_hcal_hits_trig4_2 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_hits_trig4");
  TH2F* pr_zsFrac_etaphi_1 = (TH2F*) cl->getHisto(HCALMON_1, "pr_zsFrac_etaphi");
  TH2F* pr_zsFrac_etaphi   = (TH2F*) cl->getHisto(HCALMON_0, "pr_zsFrac_etaphi");

  TH2F* h2_hcal_hits = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits");
  TH2F* h_evtRec = (TH2F*) cl->getHisto(HCALMON_0, "h_evtRec");
  TH1F* h_hcal_trig = (TH1F*) cl->getHisto(HCALMON_0, "h_hcal_trig");
  TH2F* h_caloPack_gl1_clock_diff = (TH2F*) cl->getHisto(HCALMON_0, "h_caloPack_gl1_clock_diff");

  if (!gROOT->FindObject("HcalMon5"))
  {
    MakeCanvas("HcalMon5");
  }

  TC[6]->SetEditable(true);
  TC[6]->Clear("D");
  Pad[16]->cd();
  if (!h2_hcal_hits_trig4 || !h2_hcal_hits_trig3 || !h2_hcal_hits_trig1 || !h2_hcal_hits || !h_hcal_trig || !h_caloPack_gl1_clock_diff || !h2_hcal_hits_trig2 || !h_evtRec || !h2_hcal_hits_trig4_2 || !h2_hcal_hits_trig3_2 || !h2_hcal_hits_trig1_2 || !h2_hcal_hits_trig2_2 || !pr_zsFrac_etaphi || !pr_zsFrac_etaphi_1) 
  {
    DrawDeadServer(transparent[6]);
    TC[6]->SetEditable(false);
    if (isHtml())
    {
      delete TC[6];
      TC[6] = nullptr;
    }
    return -1;
  }

  h2_hcal_hits_trig1->Add(h2_hcal_hits_trig1_2);
  h2_hcal_hits_trig2->Add(h2_hcal_hits_trig2_2);
  h2_hcal_hits_trig3->Add(h2_hcal_hits_trig3_2);
  h2_hcal_hits_trig4->Add(h2_hcal_hits_trig4_2);

  pr_zsFrac_etaphi->Add(pr_zsFrac_etaphi_1);

  Pad[16]->cd();
  gStyle->SetTitleFontSize(0.03);

  float tsize = 0.06;
  h2_hcal_hits_trig1->Draw("colz");
  h2_hcal_hits_trig1->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_hits_trig1->GetXaxis()->SetTitle("trig1 req  ieta");
  h2_hcal_hits_trig1->GetYaxis()->SetTitle("iphi");
  h2_hcal_hits_trig1->GetXaxis()->SetLabelSize(tsize);
  h2_hcal_hits_trig1->GetYaxis()->SetLabelSize(tsize);
  h2_hcal_hits_trig1->GetXaxis()->SetTitleSize(tsize);
  h2_hcal_hits_trig1->GetYaxis()->SetTitleSize(tsize);
  h2_hcal_hits_trig1->GetXaxis()->SetTitleOffset(1.2);
  h2_hcal_hits_trig1->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetLogz();
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
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
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Pulse fitting, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[6]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  Pad[17]->cd();

  gStyle->SetTitleFontSize(0.06);

  float tsize2 = 0.06;
  h2_hcal_hits_trig2->Draw("colz");
  h2_hcal_hits_trig2->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_hits_trig2->GetXaxis()->SetTitle("trig 2 req  ieta");
  h2_hcal_hits_trig2->GetYaxis()->SetTitle("iphi");
  h2_hcal_hits_trig2->GetXaxis()->SetLabelSize(tsize2);
  h2_hcal_hits_trig2->GetYaxis()->SetLabelSize(tsize2);
  h2_hcal_hits_trig2->GetXaxis()->SetTitleSize(tsize2);
  h2_hcal_hits_trig2->GetYaxis()->SetTitleSize(tsize2);
  h2_hcal_hits_trig2->GetXaxis()->SetTitleOffset(1.0);
  h2_hcal_hits_trig2->GetYaxis()->SetTitleOffset(0.85);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[19]->cd();

  gStyle->SetTitleFontSize(0.06);

  h2_hcal_hits_trig3->Draw("colz");
  h2_hcal_hits_trig3->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_hits_trig3->GetXaxis()->SetTitle("trig 3 req  ieta");
  h2_hcal_hits_trig3->GetYaxis()->SetTitle("iphi");
  h2_hcal_hits_trig3->GetXaxis()->SetLabelSize(tsize2);
  h2_hcal_hits_trig3->GetYaxis()->SetLabelSize(tsize2);
  h2_hcal_hits_trig3->GetXaxis()->SetTitleSize(tsize2);
  h2_hcal_hits_trig3->GetYaxis()->SetTitleSize(tsize2);
  h2_hcal_hits_trig3->GetXaxis()->SetTitleOffset(1.0);
  h2_hcal_hits_trig3->GetYaxis()->SetTitleOffset(0.85);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[20]->cd();

  gStyle->SetTitleFontSize(0.06);

  //h2_hcal_hits_trig4->Draw("colz");
  //h2_hcal_hits_trig4->GetXaxis()->SetNdivisions(510, kTRUE);
  //h2_hcal_hits_trig4->GetXaxis()->SetTitle("trig 4 req  ieta");
  //h2_hcal_hits_trig4->GetYaxis()->SetTitle("iphi");
  //h2_hcal_hits_trig4->GetXaxis()->SetLabelSize(tsize2);
  //h2_hcal_hits_trig4->GetYaxis()->SetLabelSize(tsize2);
  //h2_hcal_hits_trig4->GetXaxis()->SetTitleSize(tsize2);
  //h2_hcal_hits_trig4->GetYaxis()->SetTitleSize(tsize2);
  //h2_hcal_hits_trig4->GetXaxis()->SetTitleOffset(1.0);
  //h2_hcal_hits_trig4->GetYaxis()->SetTitleOffset(0.85);
  //h2_hcal_hits_trig4->GetXaxis()->SetNdivisions(510, kTRUE);
  pr_zsFrac_etaphi->Draw("COLZ");
  pr_zsFrac_etaphi->GetXaxis()->SetTitle("unsuppressed fraction ieta");
  pr_zsFrac_etaphi->GetYaxis()->SetTitle("iphi");
  pr_zsFrac_etaphi->GetXaxis()->SetLabelSize(tsize2);
  pr_zsFrac_etaphi->GetYaxis()->SetLabelSize(tsize2);
  pr_zsFrac_etaphi->GetXaxis()->SetTitleSize(tsize2);
  pr_zsFrac_etaphi->GetYaxis()->SetTitleSize(tsize2);
  pr_zsFrac_etaphi->GetXaxis()->SetTitleOffset(1.0);
  pr_zsFrac_etaphi->GetYaxis()->SetTitleOffset(0.85);

  pr_zsFrac_etaphi->GetZaxis()->SetRangeUser(0, 1);

  
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.2);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[18]->cd();
  gStyle->SetTitleFontSize(0.06);

  h_hcal_trig->SetTitle(Form("Receiving %0.3f of events from event receiver", h_evtRec->GetBinContent(1)));
  tsize2 = 0.1;
  h_hcal_trig->Draw("hist");
  h_hcal_trig->GetXaxis()->SetNdivisions(510, kTRUE);
  h_hcal_trig->GetXaxis()->SetTitle("trigger index");
  h_hcal_trig->GetYaxis()->SetTitle("events");
  h_hcal_trig->GetXaxis()->SetLabelSize(tsize2);
  h_hcal_trig->GetYaxis()->SetLabelSize(tsize2);
  h_hcal_trig->GetXaxis()->SetTitleSize(tsize2);
  h_hcal_trig->GetYaxis()->SetTitleSize(tsize2);
  h_hcal_trig->GetXaxis()->SetTitleOffset(0.9);
  h_hcal_trig->GetYaxis()->SetTitleOffset(0.85);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.2);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TC[6]->Update();
  TC[6]->Show();
  TC[6]->SetEditable(false);

  return 0;
}


int HcalMonDraw::DrawSixth(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);


  TH2F* h2_hcal_mean = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_mean");
  TH1F* h_event = (TH1F*) cl->getHisto(HCALMON_0, "h_event");
  TH2F* h2_hcal_hits = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_hits");
  TH2F* h2_hcal_time = (TH2F*) cl->getHisto(HCALMON_0, "h2_hcal_time");
  TH2F* h2_hcal_mean_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_mean");
  TH1D* h_event_1 = (TH1D*) cl->getHisto(HCALMON_1, "h_event");
  TH2F* h2_hcal_hits_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_hits");
  TH2F* h2_hcal_time_1 = (TH2F*) cl->getHisto(HCALMON_1, "h2_hcal_time");

  if (!gROOT->FindObject("HcalMon6"))
  {
    MakeCanvas("HcalMon6");
  }
  if (!h2_hcal_mean || !h_event || !h2_hcal_hits || !h2_hcal_time || !h2_hcal_mean_1 || !h_event_1 || !h2_hcal_hits_1 || !h2_hcal_time_1)
  {
    DrawDeadServer(transparent[6]);
    TC[7]->SetEditable(false);
    if (isHtml())
    {
      delete TC[7];
      TC[7] = nullptr;
    }
    return -1;
  }
  if(h_event->GetEntries()){
    h2_hcal_mean->Scale(1. / h_event->GetEntries());
    h2_hcal_hits->Scale(1. / h_event->GetEntries());
    
  }
  if(h_event_1->GetEntries()){
    h2_hcal_mean_1->Scale(1. / h_event_1->GetEntries());
    h2_hcal_hits_1->Scale(1. / h_event_1->GetEntries());
    
  }

  h2_hcal_mean->Add(h2_hcal_mean_1);
  h2_hcal_hits->Add(h2_hcal_hits_1);
  h2_hcal_time->Add(h2_hcal_time_1);
  

  float tsize = 0.06;

  TC[7]->SetEditable(true);
  TC[7]->Clear("D");
  Pad[21]->cd();
  gStyle->SetTitleFontSize(0.06);

  h2_hcal_mean->Draw("colz");
  h2_hcal_mean->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_mean->GetXaxis()->SetTitle("eta index");
  h2_hcal_mean->GetYaxis()->SetTitle("phi index");
  h2_hcal_mean->SetTitle("Tower Average Energy[ADC]");
  h2_hcal_mean->GetXaxis()->SetLabelSize(tsize);
  h2_hcal_mean->GetYaxis()->SetLabelSize(tsize);
  h2_hcal_mean->GetXaxis()->SetTitleSize(tsize);
  h2_hcal_mean->GetYaxis()->SetTitleSize(tsize);
  h2_hcal_mean->GetXaxis()->SetTitleOffset(1.2);
  h2_hcal_mean->GetYaxis()->SetTitleOffset(0.75);

  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.2);
  gPad->SetTopMargin(0.1);
  gPad->SetTicky();
  gPad->SetTickx();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);


  {
     // lines
    TLine* line_sector[32];
    for (int i_line = 0; i_line < 32; i_line++)
    {
     line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
    }
    TLine* line_board1 = new TLine(8, 0, 8, 64);
    line_board1->SetLineColor(1);
    line_board1->SetLineWidth(4);
    line_board1->SetLineStyle(1);
    TLine* line_board2 = new TLine(16, 0, 16, 64);
    line_board2->SetLineColor(1);
    line_board2->SetLineWidth(4);
    line_board2->SetLineStyle(1);

    for (auto& i_line : line_sector)
    {
     i_line->Draw();
    }
    line_board1->Draw();
    line_board2->Draw();
  }


  Pad[22]->cd();
  gStyle->SetTitleFontSize(0.06);

  h2_hcal_hits->Draw("colz");
  h2_hcal_hits->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_hits->GetXaxis()->SetTitle("eta index");
  h2_hcal_hits->GetYaxis()->SetTitle("phi index");
  h2_hcal_hits->SetTitle("Average Occupancy");
  h2_hcal_hits->GetXaxis()->SetLabelSize(tsize);
  h2_hcal_hits->GetYaxis()->SetLabelSize(tsize);
  h2_hcal_hits->GetXaxis()->SetTitleSize(tsize);
  h2_hcal_hits->GetYaxis()->SetTitleSize(tsize);
  h2_hcal_hits->GetXaxis()->SetTitleOffset(1.2);
  h2_hcal_hits->GetYaxis()->SetTitleOffset(0.75); 
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.2);
  gPad->SetTopMargin(0.1);
  gPad->SetTicky();
  gPad->SetTickx();

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);

  {
    // lines
    TLine* line_sector[32];
    for (int i_line = 0; i_line < 32; i_line++)
    {
      line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(4);
      line_sector[i_line]->SetLineStyle(1);
    }
    TLine* line_board1 = new TLine(8, 0, 8, 64);
    line_board1->SetLineColor(1);
    line_board1->SetLineWidth(4);
    line_board1->SetLineStyle(1);
    TLine* line_board2 = new TLine(16, 0, 16, 64);
    line_board2->SetLineColor(1);
    line_board2->SetLineWidth(4);
    line_board2->SetLineStyle(1);

    for (auto& i_line : line_sector)
    {
      i_line->Draw();
    }
    line_board1->Draw();
    line_board2->Draw();
  }

  Pad[23]->cd();
  gStyle->SetTitleFontSize(0.06);

  h2_hcal_time->Draw("colz");
  h2_hcal_time->GetXaxis()->SetNdivisions(510, kTRUE);
  h2_hcal_time->GetXaxis()->SetTitle("eta index");
  h2_hcal_time->GetYaxis()->SetTitle("phi index");
  h2_hcal_time->SetTitle("Average Time[samples]");
  h2_hcal_time->GetXaxis()->SetLabelSize(tsize);
  h2_hcal_time->GetYaxis()->SetLabelSize(tsize);
  h2_hcal_time->GetXaxis()->SetTitleSize(tsize);
  h2_hcal_time->GetYaxis()->SetTitleSize(tsize);
  h2_hcal_time->GetXaxis()->SetTitleOffset(1.2);
  h2_hcal_time->GetYaxis()->SetTitleOffset(0.75);
  h2_hcal_time->GetZaxis()->SetRangeUser(4, 8);

  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.2);
  gPad->SetTopMargin(0.1);
  gPad->SetTicky();
  gPad->SetTickx();

  gStyle->SetOptStat(0);

  {
    // lines
    TLine* line_sector[32];
    for (int i_line = 0; i_line < 32; i_line++)
    {
      line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(4);
      line_sector[i_line]->SetLineStyle(1);
    }
    TLine* line_board1 = new TLine(8, 0, 8, 64);
    line_board1->SetLineColor(1);
    line_board1->SetLineWidth(4);
    line_board1->SetLineStyle(1);
    TLine* line_board2 = new TLine(16, 0, 16, 64);
    line_board2->SetLineColor(1);
    line_board2->SetLineWidth(4);
    line_board2->SetLineStyle(1);

    for (auto& i_line : line_sector)
    {
      i_line->Draw();
    }
    line_board1->Draw();
    line_board2->Draw();
  }
  
  
  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Tower status, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[7]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  


  TC[7]->Update();
  TC[7]->Show();
  TC[7]->SetEditable(false);

  return 0;
}

int HcalMonDraw::DrawSeventh(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();

  char HCALMON_0[100];
  sprintf(HCALMON_0, "%s_%i", prefix.c_str(), 0);
  char HCALMON_1[100];
  sprintf(HCALMON_1, "%s_%i", prefix.c_str(), 1);

  TH2F* pr_zsFrac_etaphi = (TH2F*) cl->getHisto(HCALMON_0, "pr_zsFrac_etaphi");
  TH2F* pr_zsFrac_etaphi_1 = (TH2F*) cl->getHisto(HCALMON_1, "pr_zsFrac_etaphi");

  if (!gROOT->FindObject("HcalMon7"))
  {
    MakeCanvas("HcalMon7");
  }
  if (!pr_zsFrac_etaphi || !pr_zsFrac_etaphi_1)
  {
    DrawDeadServer(transparent[7]);
    TC[9]->SetEditable(false);
    if (isHtml())
    {
      delete TC[9];
      TC[9] = nullptr;
    }
    return -1;
  }

  pr_zsFrac_etaphi->Add(pr_zsFrac_etaphi_1);

  float tsize = 0.04;

  TC[9]->SetEditable(true);
  TC[9]->Clear("D");

  Pad[24]->cd();

  gStyle->SetTitleFontSize(0.06);
 
  pr_zsFrac_etaphi->Draw("colz");
  //find the average z for all bins
  double sum = 0;
  int count = 0;
  h1_zs->Reset();
  h1_zs_low->Reset();
  h1_zs_high->Reset();
  for (int i = 0; i < pr_zsFrac_etaphi->GetNbinsX(); i++)
  {
    for (int j = 0; j < pr_zsFrac_etaphi->GetNbinsY(); j++)
    {
      float rate = pr_zsFrac_etaphi->GetBinContent(i+1, j+1);
      if(rate <=0.04){
        h1_zs_low->Fill(rate);
      }
      else if (rate > 0.2){
        h1_zs_high->Fill(rate);
      }
      else{
        h1_zs->Fill(rate);
      }
      sum += pr_zsFrac_etaphi->GetBinContent(i+1, j+1);
      count++;
      
    }
  }
  double maxx = (sum/count)*5 > 1.1 ? 1.1 : (sum/count)*5;
  h1_zs->GetXaxis()->SetRangeUser(0, maxx);
  double averagezs = sum / count*100;
  
  
  pr_zsFrac_etaphi->GetXaxis()->SetNdivisions(510, kTRUE);
  pr_zsFrac_etaphi->GetXaxis()->SetTitle("eta index");
  pr_zsFrac_etaphi->GetYaxis()->SetTitle("phi index");
  pr_zsFrac_etaphi->SetTitle(Form("Average unsuppressed rate = %0.3f%%", averagezs));
  pr_zsFrac_etaphi->GetXaxis()->SetLabelSize(tsize);
  pr_zsFrac_etaphi->GetYaxis()->SetLabelSize(tsize);
  pr_zsFrac_etaphi->GetXaxis()->SetTitleSize(tsize);
  pr_zsFrac_etaphi->GetYaxis()->SetTitleSize(tsize);
  pr_zsFrac_etaphi->GetXaxis()->SetTitleOffset(1.2);
  pr_zsFrac_etaphi->GetYaxis()->SetTitleOffset(0.9);
  pr_zsFrac_etaphi->GetZaxis()->SetRangeUser(0, 1);

  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.2);
  gPad->SetTopMargin(0.1);
  gPad->SetTicky();
  gPad->SetTickx();
  //gStyle->SetPalette(57);
  gStyle->SetPalette(255, ZSPalette);
  gStyle->SetNumberContours(255);
  gStyle->SetOptStat(0);
  
  {
    // lines
    TLine* line_sector[32];
    for (int i_line = 0; i_line < 32; i_line++)
    {
      line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(4);
      line_sector[i_line]->SetLineStyle(1);
    }
    TLine* line_board1 = new TLine(8, 0, 8, 64);
    line_board1->SetLineColor(1);
    line_board1->SetLineWidth(4);
    line_board1->SetLineStyle(1);
    TLine* line_board2 = new TLine(16, 0, 16, 64);
    line_board2->SetLineColor(1);
    line_board2->SetLineWidth(4);
    line_board2->SetLineStyle(1);

    for (auto& i_line : line_sector)
    {
      i_line->Draw();
    }
    line_board1->Draw();
    line_board2->Draw();
  }

  Pad[25]->cd();
  gStyle->SetTitleFontSize(0.06);
  tsize = 0.08;
  h1_zs->Draw();
  h1_zs->GetXaxis()->SetTitle("unsuppressed fraction");
  h1_zs->GetYaxis()->SetTitle("towers");
  h1_zs->GetXaxis()->SetLabelSize(tsize);
  h1_zs->GetYaxis()->SetLabelSize(tsize);
  h1_zs->GetXaxis()->SetTitleSize(tsize);
  h1_zs->GetYaxis()->SetTitleSize(tsize);
  h1_zs->GetXaxis()->SetTitleOffset(0.9);
  h1_zs->GetYaxis()->SetTitleOffset(0.85);
  h1_zs->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_zs->SetFillColorAlpha(kBlue, 0.1);
  h1_zs_low->SetFillColorAlpha(kRed, 0.1);
  h1_zs_high->SetFillColorAlpha(kYellow , 0.1);
  h1_zs_low->Draw("same");
  h1_zs_high->Draw("same");
   gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.15);
  gPad->SetTopMargin(0.1);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();





  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Unsuppressed event fraction, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[9]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  TC[9]->Update();
  TC[9]->Show();
  TC[9]->SetEditable(false);
  
  return 0;

}


  
time_t HcalMonDraw::getTime()
{
  OnlMonClient* cl = OnlMonClient::instance();
  time_t currtime = cl->EventTime("CURRENT");
  return currtime;
}

int HcalMonDraw::DrawServerStats()
{
  OnlMonClient* cl = OnlMonClient::instance();
  if (!gROOT->FindObject("HcalServerStats"))
  {
    MakeCanvas("HcalServerStats");
  }
  TC[8]->Clear("D");
  TC[8]->SetEditable(true);
  transparent[8]->cd();
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
      txt << "Server " << server
	  << ", run number " << std::get<1>(servermapiter->second)
	  << ", event count: " << std::get<2>(servermapiter->second)
	  << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
      if (std::get<0>(servermapiter->second))
      {
	PrintRun.SetTextColor(kGray+2);
      }
      else
      {
	PrintRun.SetTextColor(kRed);
      }
    }
    PrintRun.DrawText(0.5, vpos, txt.str().c_str());
    vpos -= vdist;
  }
  TC[8]->Update();
  TC[8]->Show();
  TC[8]->SetEditable(false);

  return 0;
}

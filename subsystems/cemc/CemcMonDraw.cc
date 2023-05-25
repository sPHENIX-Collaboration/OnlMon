#include "CemcMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>
#include <TStyle.h>
#include <TLine.h>
#include <TPaveText.h>
#include <TLegend.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

CemcMonDraw::CemcMonDraw(const std::string &name)
: OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  return;
}

int CemcMonDraw::Init()
{
  std::cout << "initializing" << std::endl;

  cemcStyle = new TStyle("cemcStyle","cemcStyle");

  Int_t font=42; // Helvetica
  cemcStyle->SetLabelFont(font,"x");
  cemcStyle->SetTitleFont(font,"x");
  cemcStyle->SetLabelFont(font,"y");
  cemcStyle->SetTitleFont(font,"y");
  cemcStyle->SetLabelFont(font,"z");
  cemcStyle->SetTitleFont(font,"z");
  cemcStyle->SetOptStat(0);
  cemcStyle->SetPadTickX(1);
  cemcStyle->SetPadTickY(1);

  gROOT->SetStyle("cemcStyle");
  gROOT->ForceStyle();

  return 0;
}


int CemcMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "CemcMon1")
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name.c_str(), "CemcMon Tower Hits",  xsize/3 , 0, xsize/3, ysize*0.9);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[0] = new TPad("cemcpad0","hit map",0.,0.2,1.,1.);
      Pad[0]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
    
      // warning 
      warning[0] = new TPad("warning0", "hot tower warnings", 0, 0, 1, 0.2);
      warning[0]->SetFillStyle(4000);
      warning[0]->Draw();
      TC[0]->SetEditable(0);
    }
  else if (name == "CemcMon2")
    {
      // xpos negative: do not draw menu bar
      TC[1] = new TCanvas(name.c_str(), "CemcMon2 Packet Information",  2*xsize /3, 0,2*xsize / 3, ysize*0.9);
      gSystem->ProcessEvents();
      Pad[1] = new TPad("cemcpad1", "packet event check", 0.0, 0.6, 1.0/2, 0.95, 0);
      Pad[2] = new TPad("cemcpad2", "packet size", 0.0, 0.3, 1.0/2, 0.6, 0);
      Pad[3] = new TPad("cemcpad3", "packet channels", 0.0, 0.0, 1.0/2, 0.3, 0);
      Pad[1]->Draw();
      Pad[2]->Draw();
      Pad[3]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1., 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      
      //packet warnings
      warning[1] = new TPad("warning1","packet warnings",0.5,0,1,1);
      warning[1] -> SetFillStyle(4000);
      warning[1] -> Draw();
      TC[1]->SetEditable(0);
    }
  else if (name == "CemcMon3")
    {
      TC[2] = new TCanvas(name.c_str(), "CemcMon Waveform Processing", xsize/3 , 0, xsize/3, ysize*0.9);
      gSystem->ProcessEvents();
      Pad[4] = new TPad("cemcpad4", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
      Pad[5] = new TPad("cemcpad5", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
      Pad[6] = new TPad("cemcpad6", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
      Pad[4]->Draw();
      Pad[5]->Draw();
      Pad[6]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[2] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
      transparent[2]->SetFillStyle(4000);
      transparent[2]->Draw();
      TC[2]->SetEditable(0);
    }
  else if (name == "CemcMon4")
    {
      TC[3] = new TCanvas(name.c_str(), "CemcMon Waveform Processing Expert", xsize / 3, 0, xsize / 3, ysize*0.9);
      gSystem->ProcessEvents();
      Pad[7] = new TPad("cemcpad7", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
      Pad[8] = new TPad("cemcpad8", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
      Pad[9] = new TPad("cemcpad9", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
      Pad[7]->Draw();
      Pad[8]->Draw();
      Pad[9]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
      transparent[3]->SetFillStyle(4000);
      transparent[3]->Draw();
      TC[3]->SetEditable(0);
    }
  return 0;
}



int CemcMonDraw::Draw(const std::string &what)
{
  std::cout << what.c_str()  << " should say ALL "<< std::endl;
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



int CemcMonDraw::DrawFirst(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2D* hist1 = (TH2D*)cl->getHisto("CEMCMON_0","h2_cemc_rm");
  TH2D* h2_cemc_mean = (TH2D*)cl->getHisto("CEMCMON_0","h2_cemc_mean");
  TH1F* h_event = (TH1F*)cl->getHisto("CEMCMON_0","h1_event");
  TH1F* adcCount = (TH1F*)cl->getHisto("CEMCMON_0","h1_cemc_adc");
  
  if (!gROOT->FindObject("CemcMon1"))
    {
      MakeCanvas("CemcMon1");
    }
  if (!hist1)
    {
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(0);
      return -1;
    }
  h2_cemc_mean->Scale(1./h_event->GetEntries());
  
  h2_cemc_mean->Scale(1./adcCount->GetMean());
  
  hist1 -> Scale(1./adcCount->GetMean());
  
  for(int i = 0; i < nTowersEta; i++)
    {
      for(int j = 0; j < nTowersPhi; j++)
	{
	  if(i < 8) continue;
	  if(h2_cemc_mean-> GetBinContent(i+1, j+1) < 0.75 && hist1 -> GetBinContent(i+1, j+1) < 0.75) hist1->SetBinContent(i+1, j+1, h2_cemc_mean-> GetBinContent(i+1, j+1));
	  else hist1->SetBinContent(i+1, j+1, hist1->GetBinContent(i+1,j+1)/h2_cemc_mean->GetBinContent(i+1,j+1));
	}
    } 
  
  //hist1 = (TH2D*)hist1 -> Divide(h2_cemc_mean);
  //TH1F *temp = (TH1F*)h2_cemc_mean -> Clone();
  //temp -> GetXaxis() -> SetRangeUser(8,temp -> GetNbinsX());//accounts for the fact that we're missing
                                                            //adc boards on the south side. 

  
  

  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  Pad[0]->cd();
 
  hist1->GetXaxis()->SetTitle("eta index");
  hist1->GetYaxis()->SetTitle("phi index");
  hist1->GetZaxis()->SetTitle("Tower Running Mean/ Histogram Running Mean");
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->CenterTitle();
  hist1->GetZaxis()->CenterTitle();
  hist1->GetXaxis()->SetNdivisions(12,kFALSE);
  hist1->GetYaxis()->SetNdivisions(32,kFALSE);

  float tsize = 0.03;
  hist1->GetXaxis()->SetLabelSize(tsize);
  hist1->GetYaxis()->SetLabelSize(tsize);
  hist1->GetYaxis()->SetTitleOffset(1.4);
  hist1->GetZaxis()->SetLabelSize(tsize);
  hist1->GetXaxis()->SetTitleSize(tsize);
  hist1->GetYaxis()->SetTitleSize(tsize);
  hist1->GetXaxis()->SetTickLength(0.02);

  //hist1->GetZaxis()->SetRangeUser(0,2);
  
  TLine *line_sector[32];
  for(int i_line=0;i_line<32;i_line++)
    {
      line_sector[i_line] = new TLine(0,(i_line+1)*8,96,(i_line+1)*8);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(1.2);
      line_sector[i_line]->SetLineStyle(1);
    }


  const int numVertDiv = 12;
  int dEI = 96/numVertDiv;
  TLine *l_board[numVertDiv-1];
  for(int il=1; il<numVertDiv; il++){
    l_board[il-1] = new TLine(dEI*il,0,dEI*il,256);
    l_board[il-1]->SetLineColor(1);
    l_board[il-1]->SetLineWidth(1.2);
    l_board[il-1]->SetLineStyle(1);
    if(il==6) l_board[il-1]->SetLineWidth(2);
  }
  
  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.07);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.12);
  
  hist1->Draw("colz");
  //h2_cemc_mean->Draw("colz");
  for(int i_line=0;i_line<32;i_line++) line_sector[i_line]->Draw();
  for(int il=0; il<numVertDiv-1; il++) l_board[il]->Draw();

  // modify palette to black, green, and red
  Int_t palette[3] = {kGray+2,8,2};
  cemcStyle->SetPalette(3,palette);
  gROOT->SetStyle("cemcStyle");
  gROOT->ForceStyle();
  gStyle->SetPalette(3,palette);
  double_t levels[4] = {0,0.75,1.25,2};
  hist1 -> GetZaxis() -> SetRangeUser(0,2);
  hist1->SetContour(4,levels);
  
  FindHotTower(warning[0],hist1);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream  << "Running mean of Tower Hits Normalized by All Towers " ;
  runnostream2 << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  transparent[0]->cd();
  runstring = runnostream.str();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());

  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(0);
  if(save)TC[0] -> SaveAs("plots/towerHits.pdf");
  return 0;
}



int CemcMonDraw::DrawSecond(const std::string & /* what */)
{
  
  OnlMonClient *cl = OnlMonClient::instance();
  TH1F* h1_packet_number = (TH1F*) cl->getHisto("CEMCMON_0","h1_packet_number");
  TH1F* h1_packet_length = (TH1F*) cl->getHisto("CEMCMON_0","h1_packet_length");
  TH1F* h1_packet_chans = (TH1F*) cl->getHisto("CEMCMON_0","h1_packet_chans");
  TH1F* h_event = (TH1F*) cl->getHisto("CEMCMON_0","h1_event");
  
  h1_packet_number -> Scale(1./h_event -> GetEntries());
  h1_packet_length -> Scale(1./h_event -> GetEntries());
  h1_packet_chans -> Scale(1./h_event -> GetEntries());

 
  if (!gROOT->FindObject("CemcMon2"))
    {
      MakeCanvas("CemcMon2");
    }
  
  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  
  TLine *one = new TLine(6000.5,1,6128.5,1);
  one -> SetLineStyle(7);
  
  TLine *goodSize = new TLine(6000.5,5981,6128.5,5981);
  goodSize -> SetLineStyle(7);
  
  TLine *goodChans = new TLine(6000.5,192,6128.5,192);
  goodChans -> SetLineStyle(7);

  float param = 0.75;
  //float param = 0.99;
  
  TLegend *leg = new TLegend(0.3,0.16,0.95,0.4);
  leg -> SetFillStyle(0);
  leg -> SetBorderSize(0);
  
  TLine *warnLineOne = new TLine(6000.5,param*1,6128.5,param*1);
  warnLineOne -> SetLineStyle(7);
  warnLineOne -> SetLineColor(2);
  
  leg -> AddEntry(warnLineOne,"75% Threshold","l");
  
  TLine *warnLineOneS = new TLine(6000.5,param*1,6128.5,param*1);
  warnLineOneS -> SetLineStyle(10);
  warnLineOneS -> SetLineColor(2);

  leg -> AddEntry(warnLineOneS,"75% Threshold, High Eta, South","l");

  TLine *warnLineSize = new TLine(6000.5,param*5981.,6128.5,param*5981.);
  warnLineSize -> SetLineStyle(7);
  warnLineSize -> SetLineColor(2);

  TLine *warnLineSizeS = new TLine(6000.5,param*3991.,6128.5,param*3991.);
  warnLineSizeS -> SetLineStyle(10);
  warnLineSizeS -> SetLineColor(2);

  TLine *warnLineChans = new TLine(6000.5,param*192.,6128.5,param*192.);
  warnLineChans -> SetLineStyle(7);
  warnLineChans -> SetLineColor(2);

  TLine *warnLineChansS = new TLine(6000.5,param*128.,6128.5,param*128.);
  warnLineChansS -> SetLineStyle(10);
  warnLineChansS -> SetLineColor(2);

  Pad[1]->cd();
  float tsize = 0.08;
  h1_packet_number -> GetYaxis() -> SetRangeUser(0.0,1.3);
  h1_packet_number -> Draw("hist");
  std::vector<std::vector<int>> badPackets;
  badPackets.push_back(getBadPackets(h1_packet_number,0,param));
  one -> Draw("same");
  warnLineOne -> Draw("same");
  h1_packet_number->GetXaxis()->SetNdivisions(510,kTRUE);
  h1_packet_number->GetXaxis()->SetTitle("packet #");
  h1_packet_number->GetYaxis()->SetTitle("% Of Events Present");
  h1_packet_number->GetXaxis()->SetLabelSize(tsize-0.01);
  h1_packet_number->GetYaxis()->SetLabelSize(tsize-0.01);
  h1_packet_number->GetXaxis()->SetTitleSize(tsize-0.01);
  h1_packet_number->GetYaxis()->SetTitleSize(tsize-0.01);
  h1_packet_number->GetXaxis()->SetTitleOffset(1);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[2]->cd();
  h1_packet_length -> Draw("hist");
  h1_packet_length -> GetYaxis() -> SetRangeUser(0,6500);
  badPackets.push_back(getBadPackets(h1_packet_length,1,param));
  goodSize -> Draw("same");
  warnLineSize -> Draw("same");
  warnLineSizeS -> Draw("same");
  leg -> Draw("same");
  h1_packet_length->GetXaxis()->SetNdivisions(510,kTRUE);
  h1_packet_length->GetXaxis()->SetTitle("packet #");
  h1_packet_length->GetYaxis()->SetTitle("Average Packet Size");
  h1_packet_length->GetXaxis()->SetLabelSize(tsize-.01);
  h1_packet_length->GetYaxis()->SetLabelSize(tsize);
  h1_packet_length->GetXaxis()->SetTitleSize(tsize-.01);
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


  Pad[3]->cd();
  h1_packet_chans -> Draw("hist");
  h1_packet_chans -> GetYaxis() -> SetRangeUser(0,212);
  badPackets.push_back(getBadPackets(h1_packet_chans,2,param));
  goodChans -> Draw("same");
  warnLineChans -> Draw("same");
  warnLineChansS -> Draw("same");
  h1_packet_chans->GetXaxis()->SetNdivisions(510,kTRUE);
  h1_packet_chans->GetXaxis()->SetTitle("packet #");
  h1_packet_chans->GetYaxis()->SetTitle("Average # of Channels");
  h1_packet_chans->GetXaxis()->SetLabelSize(tsize-.01);
  h1_packet_chans->GetYaxis()->SetLabelSize(tsize);
  h1_packet_chans->GetXaxis()->SetTitleSize(tsize-.01);
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
  
  warning[1] -> cd();
  TLegend *badPacks = new TLegend(0,1/4.,1,0.9);
  TPaveText *title = new TPaveText(0,0.9,1,0.95);
  title -> AddText("Bad Packets");
  badPacks -> SetNColumns(8);
  badPacks -> SetTextSize(0.03);
  float badboys = 0;
  for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < (int)badPackets[i].size(); j++)
	{
	  //there's most certainly a better way to do this but it's 5:00 on day 5 of owl shift
	  //just want to prevent a packet showing up multiple times and crowding the screen
	  if(badPackets[i][j] == 0) continue;//need this to prevent seg faulting
	  
	  if(i == 0)
	    {
	      badPacks -> AddEntry("",Form("%d",badPackets[i][j]),"");
	      badboys++;
	    }
	  else if(i == 1)  
	    {
	      if(!(std::count(badPackets[i-1].begin(),badPackets[i-1].end(),badPackets[i][j])))
		{
		  badPacks -> AddEntry("",Form("%d",badPackets[i].at(j)),"");
		  badboys++;
		}
	    }
	  else if(i == 2)
	    {
	      if(!(std::count(badPackets[i-1].begin(),badPackets[i-1].end(),badPackets[i][j]))) 
		{
		  badPacks -> AddEntry("",Form("%d",badPackets[i].at(j)),"");
		  badboys++;
		}
	      if(!(std::count(badPackets[i-2].begin(),badPackets[i-2].end(),badPackets[i][j])) && !(std::count(badPackets[i-1].begin(),badPackets[i-1].end(),badPackets[i][j])))
		{
		  badPacks -> AddEntry("",Form("%d",badPackets[i].at(j)),"");
		  badboys++;
		}
	    }
	}
    }
  badPacks -> Draw();
  title -> Draw();
  TPaveText *desc = new TPaveText(0,0,1,0.25);
  desc -> AddText(Form("Currently %.2g%% of packets are reporting a problem",badboys/128.*100));
  desc -> AddText("Packets will be reported bad as above for the following reasons:" );
  desc -> AddText(Form("A packet appears in less than %g %% of events",param*100));
  desc -> AddText(Form("A packet is less than %g %% of size 5981",param*100));
  desc -> AddText(Form("A packet sees fewer than %g %% of 192 channels",param*100));
  desc -> Draw();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.02);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::ostringstream runnostream2;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  
  runnostream << "Packet Information";
  runnostream2 << " Run " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  transparent[1]->cd();

  runstring = runnostream.str();
  PrintRun.DrawText(0.5,.99, runstring.c_str());
  
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, .966, runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(0);
  if(save)TC[1] -> SaveAs("plots/packets.pdf");
  return 0;
}



int CemcMonDraw::DrawThird(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1F* h1_waveform_twrAvg = (TH1F*) cl->getHisto("CEMCMON_0","h1_waveform_twrAvg");
  TH1F* h1_waveform_time = (TH1F*) cl->getHisto("CEMCMON_0","h1_waveform_time");
  TH1F* h1_waveform_pedestal = (TH1F*) cl->getHisto("CEMCMON_0","h1_waveform_pedestal");

  if (!gROOT->FindObject("CemcMon3"))
    {
      MakeCanvas("CemcMon3");
    }
  
  TC[2]->SetEditable(1);
  TC[2]->Clear("D");
  Pad[4]->cd();
  if (!h1_waveform_twrAvg  || !h1_waveform_time || !h1_waveform_pedestal)
    {
      DrawDeadServer(transparent[2]);
      TC[3]->SetEditable(0);
      return -1;
    }

  gStyle->SetTitleFontSize(0.03);
  h1_waveform_twrAvg->Draw("hist");

  float tsize = 0.06;
  float tsize2 = 0.08;

  h1_waveform_twrAvg->GetXaxis()->SetNdivisions(16);
  h1_waveform_twrAvg->GetXaxis()->SetTitle("sample #");
  h1_waveform_twrAvg->GetYaxis()->SetTitle("Waveform ADC in latest event [ADC]");
  h1_waveform_twrAvg->GetXaxis()->SetLabelSize(tsize-.01);
  h1_waveform_twrAvg->GetYaxis()->SetLabelSize(tsize);
  h1_waveform_twrAvg->GetXaxis()->SetTitleSize(tsize-.01);
  h1_waveform_twrAvg->GetYaxis()->SetTitleSize(tsize);
  h1_waveform_twrAvg->GetXaxis()->SetTitleOffset(1.);
  h1_waveform_twrAvg->GetYaxis()->SetTitleOffset(1.2);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Waveform fitting";
  runnostream2 <<"Run " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  
  transparent[2]->cd();

  runstring = runnostream.str();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());
  
  Pad[5]->cd();

  gStyle->SetTitleFontSize(0.06);

  h1_waveform_time->Draw("hist");
  h1_waveform_time->GetXaxis()->SetNdivisions(16);
  h1_waveform_time->GetXaxis()->SetTitle("waveform peak position [sample #]");
  h1_waveform_time->GetYaxis()->SetTitle("Fraction of Towers");
  h1_waveform_time->GetXaxis()->SetLabelSize(tsize2);
  h1_waveform_time->GetYaxis()->SetLabelSize(tsize2);
  h1_waveform_time->GetXaxis()->SetTitleSize(tsize2);
  h1_waveform_time->GetYaxis()->SetTitleSize(tsize2);
  h1_waveform_time->GetXaxis()->SetTitleOffset(1.0);
  h1_waveform_time->GetYaxis()->SetTitleOffset(.85);
  h1_waveform_time -> Scale(1./h1_waveform_time->GetEntries());
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[6]->cd();

  gStyle->SetTitleFontSize(0.06);
  gStyle->SetOptStat(0);

  h1_waveform_pedestal->Draw("hist");
  h1_waveform_pedestal->GetXaxis()->SetNdivisions(8);
  h1_waveform_pedestal->GetXaxis()->SetTitle("ADC Pedestal");
  h1_waveform_pedestal->GetYaxis()->SetTitle("Fraction of Towers");
  h1_waveform_pedestal->GetXaxis()->SetLabelSize(tsize2);
  h1_waveform_pedestal->GetYaxis()->SetLabelSize(tsize2);
  h1_waveform_pedestal->GetXaxis()->SetTitleSize(tsize2);
  h1_waveform_pedestal->GetYaxis()->SetTitleSize(tsize2);
  h1_waveform_pedestal->GetXaxis()->SetTitleOffset(1);
  h1_waveform_pedestal->GetYaxis()->SetTitleOffset(0.85);
  h1_waveform_pedestal -> Scale(1./h1_waveform_pedestal->GetEntries());
  //h1_waveform_pedestal -> GetXaxis() -> SetRangeUser(1000,2000);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(0);
  if(save)TC[2] -> SaveAs("plots/waveform.pdf");
  return 0;
}

int CemcMonDraw::DrawFourth(const std::string & /* what */)
{

  OnlMonClient *cl = OnlMonClient::instance();
  
  TH1F *h_waveform_sigDiff = (TH1F*)cl -> getHisto("CEMCMON_0","h1_fitting_sigDiff");
  TH1F *h_waveform_pedDiff = (TH1F*)cl -> getHisto("CEMCMON_0","h1_fitting_pedDiff");
  TH1F *h_waveform_timeDiff = (TH1F*)cl -> getHisto("CEMCMON_0","h1_fitting_timeDiff");

  if(!gROOT -> FindObject("CemcMon4"))
    {
      MakeCanvas("CemcMon4");
    }
  
  TC[3]->SetEditable(1);
  TC[3]->Clear("D");
  if(!h_waveform_sigDiff || !h_waveform_pedDiff || !h_waveform_timeDiff)
    {
      DrawDeadServer(transparent[3]);
      TC[4]->SetEditable(0);
      return -1;
    }
  
  gStyle->SetTitleFontSize(0.03);
    
  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Waveform Template vs. Fast Fitting";
  runnostream2 << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  
  transparent[3]->cd();

  runstring = runnostream.str();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  
  
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());
  
  
  Pad[7]->cd();

  gStyle->SetTitleFontSize(0.06);
  
  float tsize2 = 0.08;
  h_waveform_sigDiff->Draw("hist");
  h_waveform_sigDiff->GetXaxis()->SetNdivisions(16);
  h_waveform_sigDiff->GetXaxis()->SetTitle("Fast ADC/Template ADC");
  h_waveform_sigDiff->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_sigDiff->GetXaxis()->SetLabelSize(tsize2-.01);
  h_waveform_sigDiff->GetYaxis()->SetLabelSize(tsize2-.01);
  h_waveform_sigDiff->GetXaxis()->SetTitleSize(tsize2-.01);
  h_waveform_sigDiff->GetYaxis()->SetTitleSize(tsize2-.01);
  h_waveform_sigDiff->GetXaxis()->SetTitleOffset(.9);
  h_waveform_sigDiff->GetYaxis()->SetTitleOffset(1.1);
  h_waveform_sigDiff -> Scale(1./h_waveform_sigDiff -> GetEntries());
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[8]->cd();

  //gStyle->SetTitleFontSize(0.06);
  
  h_waveform_pedDiff->Draw("hist");
  h_waveform_pedDiff->GetXaxis()->SetNdivisions(16);
  h_waveform_pedDiff->GetXaxis()->SetTitle("Fast Pedestal/Template Pedestal");
  h_waveform_pedDiff->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_pedDiff->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_pedDiff->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_pedDiff->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_pedDiff->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_pedDiff->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_pedDiff->GetYaxis()->SetTitleOffset(0.9);
  h_waveform_pedDiff -> Scale(1./ h_waveform_pedDiff -> GetEntries());
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[9]->cd();

  h_waveform_timeDiff->Draw("hist");
  h_waveform_timeDiff->GetXaxis()->SetNdivisions(16);
  h_waveform_timeDiff->GetXaxis()->SetTitle("Fast Peak Time - Template Peak Time");
  h_waveform_timeDiff->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_timeDiff->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_timeDiff->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_timeDiff->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_timeDiff->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_timeDiff->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_timeDiff->GetYaxis()->SetTitleOffset(0.9);
  h_waveform_timeDiff -> Scale(1./ h_waveform_timeDiff->GetEntries());
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();


  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(0);
  gStyle->SetOptStat(0);
  if(save)TC[3] -> SaveAs("plots/waveformExpert.pdf");
  return 0;
}


int CemcMonDraw::FindHotTower(TPad *warningpad,TH2D* hhit){
  float nhott = 0;
  float ndeadt = 0;
  float hot_threshold  = 1.25;
  float dead_threshold = 0.75;
  float nTowerTotal = 24576.-2048.;//-2048 to account for the non-functioning towers at the edge of the south
  for(int ieta=0; ieta<nTowersEta; ieta++){
    for(int iphi=0; iphi<nTowersPhi; iphi++){
      
      if(ieta < 8) continue;
      
      double nhit = hhit->GetBinContent(ieta+1, iphi+1);
	
      if(nhit > hot_threshold)	nhott++;
      
	
      if(nhit < dead_threshold)	ndeadt++;
      
    }
  }

  //draw warning here
  warningpad->cd();
  TPaveText *dead = new TPaveText(0.01,0.7,0.33,1);
  dead -> SetFillColor(kGray+2);
  dead -> SetTextColor(kWhite);
  dead -> AddText(Form("Dead towers: %.3g%%",100*ndeadt/nTowerTotal));
  if(100*ndeadt/nTowerTotal > 2.5)
    {
      dead -> AddText("CALL EMCAL EXPERT");
    }
  TPaveText *good = new TPaveText(0.33,0.7,0.66,1);
  good -> SetFillColor(kGreen+1);
  good -> AddText(Form("Good towers: %.3g%%",100*(nTowerTotal-ndeadt-nhott)/nTowerTotal));
  TPaveText *hot = new TPaveText(0.66,0.7,1,1);
  hot -> SetFillColor(kRed-9);
  hot -> AddText(Form("Hot towers: %.3g%%",100*nhott/nTowerTotal));
  if(100*nhott/nTowerTotal > 3.5)
    {
      hot -> AddText("CALL EMCAL EXPERT");
    }
  TPaveText *warn = new TPaveText(0.01,0.1,1,0.7);
  warn -> SetTextSize(0.1);
  warn -> AddText("Helpful Numbers: 1 Box = Interface Board (IB)");
  warn -> AddText("3 Boxes (horiz) = 1 Packet; 6 Boxes (horiz) = 1 Sector");
  warn -> AddText("Inform Expert if Number of Dead Towers Exceeds: 2.5%");
  warn -> AddText("Inform Expert if Number of Hot Towers Exceeds: 3.5%");
  dead -> Draw();
  good -> Draw();

  hot -> Draw();
  warn -> Draw();
  warningpad->Update();
  return 0;
}



int CemcMonDraw::MakePS(const std::string &what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  std::ostringstream filename;
  int iret = Draw(what);
  if (iret)  // on error no ps files please
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_2_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  return 0;
}

int CemcMonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  std::string pngfile = cl->htmlRegisterPage(*this, "First Canvas", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);

  // idem for 2nd canvas.
  pngfile = cl->htmlRegisterPage(*this, "Second Canvas", "2", "png");
  cl->CanvasToPng(TC[1], pngfile);
  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
  std::ofstream out(logfile.c_str());
  out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
      << "</TITLE></HEAD>" << std::endl;
  out << "<P>Some log file output would go here." << std::endl;
  out.close();

  std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
  std::ofstream out2(status.c_str());
  out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
       << "</TITLE></HEAD>" << std::endl;
  out2 << "<P>Some status output would go here." << std::endl;
  out2.close();
  cl->SaveLogFile(*this);
  return 0;
}


int CemcMonDraw::DrawHistory(const std::string & /* what */)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  std::vector<float> var;
  std::vector<float> varerr;
  std::vector<time_t> timestamp;
  std::vector<int> runnumber;
  std::string varname = "cemcmondummy";
  // this sets the time range from whihc values should be returned
  time_t begin = 0;            // begin of time (1.1.1970)
  time_t end = time(nullptr);  // current time (right NOW)
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
    {
      std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
      return iret;
    }
  if (!gROOT->FindObject("CemcMon3"))
    {
      MakeCanvas("CemcMon3");
    }
  // timestamps come sorted in ascending order
  float *x = new float[var.size()];
  float *y = new float[var.size()];
  float *ex = new float[var.size()];
  float *ey = new float[var.size()];
  int n = var.size();
  for (unsigned int i = 0; i < var.size(); i++)
    {
      //       std::cout << "timestamp: " << ctime(&timestamp[i])
      // 	   << ", run: " << runnumber[i]
      // 	   << ", var: " << var[i]
      // 	   << ", varerr: " << varerr[i]
      // 	   << std::endl;
      x[i] = timestamp[i] - TimeOffsetTicks;
      y[i] = var[i];
      ex[i] = 0;
      ey[i] = varerr[i];
    }
  Pad[4]->cd();
  if (gr[0])
    {
      delete gr[0];
    }
  gr[0] = new TGraphErrors(n, x, y, ex, ey);
  gr[0]->SetMarkerColor(4);
  gr[0]->SetMarkerStyle(21);
  gr[0]->Draw("ALP");
  gr[0]->GetXaxis()->SetTimeDisplay(1);
  gr[0]->GetXaxis()->SetLabelSize(0.03);
  // the x axis labeling looks like crap
  // please help me with this, the SetNdivisions
  // don't do the trick
  gr[0]->GetXaxis()->SetNdivisions(-1006);
  gr[0]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
  gr[0]->GetXaxis()->SetTimeFormat("%Y/%m/%d %H:%M");
  delete[] x;
  delete[] y;
  delete[] ex;
  delete[] ey;

  varname = "cemcmoncount";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
    {
      std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
      return iret;
    }
  x = new float[var.size()];
  y = new float[var.size()];
  ex = new float[var.size()];
  ey = new float[var.size()];
  n = var.size();
  for (unsigned int i = 0; i < var.size(); i++)
    {
      //       std::cout << "timestamp: " << ctime(&timestamp[i])
      // 	   << ", run: " << runnumber[i]
      // 	   << ", var: " << var[i]
      // 	   << ", varerr: " << varerr[i]
      // 	   << std::endl;
      x[i] = timestamp[i] - TimeOffsetTicks;
      y[i] = var[i];
      ex[i] = 0;
      ey[i] = varerr[i];
    }
  Pad[5]->cd();
  if (gr[1])
    {
      delete gr[1];
    }
  gr[1] = new TGraphErrors(n, x, y, ex, ey);
  gr[1]->SetMarkerColor(4);
  gr[1]->SetMarkerStyle(21);
  gr[1]->Draw("ALP");
  gr[1]->GetXaxis()->SetTimeDisplay(1);
  // TC[2]->Update();
  //    h1->GetXaxis()->SetTimeDisplay(1);
  //    h1->GetXaxis()->SetLabelSize(0.03);
  gr[1]->GetXaxis()->SetLabelSize(0.03);
  gr[1]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
  gr[1]->GetXaxis()->SetTimeFormat("%Y/%m/%d %H:%M");
  //    h1->Draw();
  delete[] x;
  delete[] y;
  delete[] ex;
  delete[] ey;

  TC[2]->Update();
  return 0;
}

std::vector<int> CemcMonDraw::getBadPackets(TH1 *hist, int what, float cutoff)
{
  float params[3] = {1., 5981., 192.};
  float params2[3] = {1., 3991., 128.};
  //float cutoff = 0.75;
  std::vector<int> badpacks = {0};
  
  for(int i = 1; i < hist -> GetNbinsX(); i++)
    {
      if(hist -> GetBinContent(i) < params[what]*cutoff &&  !(((6000 + i - 2.)/4.) == floor(((6000 + i - 2)/4))) ) badpacks.push_back(i+6000);
      
      else if(hist -> GetBinContent(i) < params2[what]*cutoff &&  (((6000 + i - 2.)/4.) == floor(((6000 + i - 2)/4))) ) badpacks.push_back(i+6000);
    }
  
  return badpacks;
}

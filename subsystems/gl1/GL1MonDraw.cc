#include "GL1MonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/RunDBodbc.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TFrame.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

GL1MonDraw::GL1MonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

GL1MonDraw::~GL1MonDraw()
{
  delete gl1Style;
}

int GL1MonDraw::Init()
{
  TStyle *oldStyle = gStyle;
  gl1Style = new TStyle("gl1Style", "gl1Style");
  gl1Style->SetFrameBorderMode(0);
  gl1Style->SetCanvasColor(0);
  gl1Style->SetPadBorderMode(0);
  gl1Style->SetPadBottomMargin(0.15);
  gl1Style->SetCanvasBorderMode(0);
  oldStyle->cd();
  m_RunDB = new RunDBodbc();
  reject_graph_good.resize(8, nullptr);
  reject_graph_bad.resize(8, nullptr);
  rejection_limit.resize(8);

// from /home/repo/Debian/bin/ll1TriggerControl.py
// photon_triggers_rejection_ranges = [[19, 27], [50, 70]]
// self.rej_ranges = [[560,840],[3760,5640],[21440,40160],[70000,130000],[65,85],    [240,360],[1120,1680],[4000,6000]]

  // rejection_limit[0] = std::make_pair(19,27);
  // rejection_limit[1] = std::make_pair(50,70);
  // rejection_limit[2] = std::make_pair(50,70);
  // rejection_limit[3] = std::make_pair(50,70);
  // rejection_limit[4] = std::make_pair(50,70);
  // rejection_limit[5] = std::make_pair(50,70);
  // rejection_limit[6] = std::make_pair(50,70);
  // rejection_limit[7] = std::make_pair(50,70);
  rejection_limit[0] = std::make_pair(560,840);
  rejection_limit[1] = std::make_pair(3760,5640);
  rejection_limit[2] = std::make_pair(21440,40160);
  rejection_limit[3] = std::make_pair(70000,130000);
  rejection_limit[4] = std::make_pair(65,85);
  rejection_limit[5] = std::make_pair(240,360);
  rejection_limit[6] = std::make_pair(1120,1680);
  rejection_limit[7] = std::make_pair(4000,6000);
  
  return 0;
}

int GL1MonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle *oldStyle = gStyle;

  gl1Style->cd();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "GL1MonScaled")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "GL1 Scaled Triggers", -1, 0, xsize, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for (int i = 0; i < 4; i++)
    {
      double xlow = 0.75 - (0.25 * i);
      double xhigh = xlow + 0.25;
      for (int j = 0; j < 7; j++)
      {
        double ylow = 0.0 + (0.13 * j);
        double yhigh = ylow + 0.13;
        int padindex = 27 - (i + 4 * j);  // make it start from the top of the plot
        // std::cout << "idx: " << padindex << " pad: xl: " << xlow << ", xh: " << xhigh
        //  << " pad: yl: " << ylow << ", yh: " << yhigh
        //  	  << std::endl;
        std::string padname = "gl1pad_" + std::to_string(padindex);
        ScalePad[padindex] = new TPad(padname.c_str(), "who needs this?", xlow, ylow, xhigh, yhigh, 0);
        ScalePad[padindex]->Draw();
      }
    }
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  if (name == "GL1MonLive")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "GL1 Live Triggers", -1, 0, xsize, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for (int i = 0; i < 4; i++)
    {
      double xlow = 0.75 - (0.25 * i);
      double xhigh = xlow + 0.25;
      for (int j = 0; j < 7; j++)
      {
        double ylow = 0.0 + (0.13 * j);
        double yhigh = ylow + 0.13;
        int padindex = 27 - (i + 4 * j);  // make it start from the top of the plot
        // std::cout << "idx: " << padindex << "pad: xl: " << xlow << ", xh: " << xhigh
        // << "pad: yl: " << ylow << ", yh: " << yhigh
        //  	  << std::endl;
        std::string padname = "gl1pad_" + std::to_string(padindex);
        LivePad[padindex] = new TPad(padname.c_str(), "who needs this?", xlow, ylow, xhigh, yhigh, 0);
        LivePad[padindex]->Draw();
      }
    }
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "GL1ServerStats")
  {
    int canvasindex = 2;
    TC[canvasindex] = new TCanvas(name.c_str(), "GL1Mon Server Stats", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->Draw();
    transparent[canvasindex]->SetFillColor(kGray);
    TC[canvasindex]->SetEditable(false);
  }
  if (name == "GL1MonRejection")
  {
    int canvasindex = 3;
    // xpos (-1) negative: do not draw menu bar
    TC[canvasindex] = new TCanvas(name.c_str(), "GL1 Rejection", -1, 0, xsize, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for (int i = 0; i < 2; i++)
    {
      double xlow = 0.5 - (0.5 *i);
      double xhigh = xlow + 0.5;
      for (int j = 0; j < 4; j++)
      {
	double ylow = 0.0 + (0.23 * j);
	double yhigh = ylow + 0.23;
	int padindex = 7 -  (i + 2 * j);  // make it start from the top of the plot
	// std::cout << "idx: " << padindex << "pad: xl: " << xlow << ", xh: " << xhigh
	// 	  << "pad: yl: " << ylow << ", yh: " << yhigh
	// 	  << std::endl;
	std::string padname = "gl1pad_" + std::to_string(padindex);
	RejPad[padindex] = new TPad(padname.c_str(), "who needs this?", xlow, ylow, xhigh, yhigh, 0);
	RejPad[padindex]->Draw();
      }
    }
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  oldStyle->cd();
  return 0;
}

int GL1MonDraw::Draw(const std::string &what)
{
  int iret = 0;
  int idraw = 0;
  OnlMonClient *cl = OnlMonClient::instance();
  if (m_CurrentRunnumber != cl->RunNumber())
  {
    m_CurrentRunnumber = cl->RunNumber();
    FetchTriggerNames();
  }
  if (what == "ALL" || what == "SCALED")
  {
    iret += DrawScaled(what);
    idraw++;
  }
  if (what == "ALL" || what == "LIVE")
  {
    iret += DrawLive(what);
    idraw++;
  }
  if (what == "ALL" || what == "REJECTION")
  {
    iret += DrawRejection();
    idraw++;
  }
  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int GL1MonDraw::DrawScaled(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("GL1MonScaled"))
  {
    MakeCanvas("GL1MonScaled");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  TText title;
  title.SetNDC();
  title.SetTextAlign(23);
  title.SetTextColor(4);
  title.SetTextSize(0.1);
  TText agap;
  agap.SetTextAlign(21);
  agap.SetTextSize(0.055);
  TLine *line = new TLine();

  int ipad = 0;
  for (int i = 0; i < 64; i++)
  {
	
    std::string hname = "gl1_scaledtrigger_" + std::to_string(i);
    TH1 *hist1 = cl->getHisto("GL1MON_0", hname);
    if (!hist1)
    {
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(false);
      return -1;
    }
    if (hist1->GetMaximum() > 0)
    {
      // std::cout << "ipad: " << ipad << " trigger no: " << i << " trigger: "
      // 		  << m_TrignameArray[i] << std::endl;
      if (ipad > 27)
	{
	  std::cout << "ipad: " << ipad << " trigger: "
		    << m_TrignameArray[i] << std::endl;
	  ipad++;
	  continue;
	}
      TH1 *abortgap = (TH1 *) hist1->Clone();
      TH1 *forbidden = (TH1 *) hist1->Clone();
      abortgap->SetFillColor(6);
      forbidden->SetFillColor(2);
      for (int j = 0; j < 112; j++)
      {
        abortgap->SetBinContent(j, 0);
        forbidden->SetBinContent(j, 0);
      }
      for (int j = 112; j < 121; j++)
      {
        forbidden->SetBinContent(j, 0);
      }
      for (int j = 121; j < 130; j++)
      {
        abortgap->SetBinContent(j, 0);
      }
      ScalePad[ipad]->cd();
      ScalePad[ipad]->SetLogy();
      hist1->SetStats(0);
      std::string htitle = m_TrignameArray[i];
      //      std::cout << "index " << i << " title: " << htitle << std::endl;
      hist1->SetFillColor(3);
      hist1->SetXTitle("Bunch Crossing");
      hist1->SetYTitle("Events");
      hist1->GetXaxis()->SetLabelSize(0.06);
      hist1->GetXaxis()->SetTitleSize(0.055);
      hist1->GetXaxis()->SetTitleOffset(1.1);

      hist1->GetYaxis()->SetLabelSize(0.06);
      hist1->GetYaxis()->SetTitleSize(0.06);
      hist1->GetYaxis()->SetTitleOffset(0.4);
      hist1->SetTitle("");
      hist1->DrawCopy();
      if (htitle.find("Clock") == std::string::npos)
      {
        abortgap->DrawCopy("same");
      }
      forbidden->DrawCopy("same");
      title.SetTextColor(4);
      title.SetTextSize(0.1);
      title.DrawText(0.5, 0.99, htitle.c_str());
      delete abortgap;
      delete forbidden;
      line->SetLineColor(6);
      line->SetLineWidth(2);
      line->SetLineStyle(2);  // dashed
      ScalePad[ipad]->Update();
      // whoopee - this is how to get the top of the pad in a log y scale
      // only after the TPad::Update() where the Pad figures out its dimensions
      line->DrawLine(119.5, std::pow(10, ScalePad[ipad]->GetFrame()->GetY1()), 119.5, std::pow(10, ScalePad[ipad]->GetFrame()->GetY2()));
      if (htitle.find("Clock") == std::string::npos)
      {
        line->DrawLine(110.5, std::pow(10, ScalePad[ipad]->GetFrame()->GetY1()), 110.5, std::pow(10, ScalePad[ipad]->GetFrame()->GetY2()));
        agap.DrawText(115, std::pow(10, ScalePad[ipad]->GetFrame()->GetY2()), "Abort Gap");
      }
      agap.DrawText(125, std::pow(10, ScalePad[ipad]->GetFrame()->GetY2()), "Forbidden");
      ipad++;
    }
  }
  delete line;
  // else
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t, int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "GL1 Scaled Trigger Run:" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);
  return 0;
}

int GL1MonDraw::DrawLive(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("GL1MonLive"))
  {
    MakeCanvas("GL1MonLive");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  TText title;
  title.SetNDC();
  title.SetTextAlign(23);
  title.SetTextColor(4);
  title.SetTextSize(0.1);
  TText agap;
  agap.SetTextAlign(21);
  agap.SetTextSize(0.055);

  TLine *line = new TLine();
  int ipad = 0;
  int icnt = 0;
  for (int i = 0; i < 64; i++)
  {
    std::string hname = "gl1_livetrigger_" + std::to_string(i);
    TH1 *hist1 = cl->getHisto("GL1MON_0", hname);
    if (!hist1)
    {
      DrawDeadServer(transparent[1]);
      TC[1]->SetEditable(false);
      return -1;
    }
    if (i != 0
	&& i != 1
	&& i != 12
	&& i != 14
	&& i != 16
	&& i != 17
	&& i != 18
	&& i != 19
	&& i != 20
	&& i != 21
	&& i != 22
	&& i != 23
	&& i != 24
	&& i != 25
	&& i != 26
	&& i != 27
	&& i != 28
	&& i != 29
	&& i != 30
	&& i != 31
	&& i != 32
	&& i != 33
	&& i != 34
	&& i != 35
	&& i != 36
	&& i != 37
	&& i != 38)
      {
	continue;
      }
    if (hist1->GetMaximum() > 0 && ipad < 28)
    {
      // std::cout << "ipad: " << ipad << " trigger no: " << i << " trigger: "
      // 		  << m_TrignameArray[i] << std::endl;
      icnt++;
      TH1 *abortgap = (TH1 *) hist1->Clone();
      TH1 *forbidden = (TH1 *) hist1->Clone();
      abortgap->SetFillColor(6);
      forbidden->SetFillColor(2);
      for (int j = 0; j < 112; j++)
      {
        abortgap->SetBinContent(j, 0);
        forbidden->SetBinContent(j, 0);
      }
      for (int j = 112; j < 121; j++)
      {
        forbidden->SetBinContent(j, 0);
      }
      for (int j = 121; j < 130; j++)
      {
        abortgap->SetBinContent(j, 0);
      }
      LivePad[ipad]->cd();
      LivePad[ipad]->SetLogy();
      hist1->SetStats(0);
      std::string htitle = m_TrignameArray[i];
      //      std::cout << "index " << i << " title: " << htitle << std::endl;
      hist1->SetFillColor(3);
      hist1->SetXTitle("Bunch Crossing");
      hist1->SetYTitle("Events");
      hist1->GetXaxis()->SetLabelSize(0.06);
      hist1->GetXaxis()->SetTitleSize(0.055);
      hist1->GetXaxis()->SetTitleOffset(1.1);

      hist1->GetYaxis()->SetLabelSize(0.06);
      hist1->GetYaxis()->SetTitleSize(0.06);
      hist1->GetYaxis()->SetTitleOffset(0.4);
      hist1->SetTitle("");
      hist1->DrawCopy();
      if (htitle.find("Clock") == std::string::npos)
      {
        abortgap->DrawCopy("same");
      }
      forbidden->DrawCopy("same");
      title.SetTextColor(4);
      title.SetTextSize(0.1);
      title.DrawText(0.5, 0.99, htitle.c_str());
      delete abortgap;
      delete forbidden;
      line->SetLineColor(6);
      line->SetLineWidth(2);
      line->SetLineStyle(2);  // dashed
      LivePad[ipad]->Update();
      // whoopee - this is how to get the top of the pad in a log y scale
      // only after the TPad::Update() where the Pad figures out its dimensions
      line->DrawLine(119.5, std::pow(10, LivePad[ipad]->GetFrame()->GetY1()), 119.5, std::pow(10, LivePad[ipad]->GetFrame()->GetY2()));
      if (htitle.find("Clock") == std::string::npos)
      {
        line->DrawLine(110.5, std::pow(10, LivePad[ipad]->GetFrame()->GetY1()), 110.5, std::pow(10, LivePad[ipad]->GetFrame()->GetY2()));
        agap.DrawText(115, std::pow(10, LivePad[ipad]->GetFrame()->GetY2()), "Abort Gap");
      }
      agap.DrawText(125, std::pow(10, LivePad[ipad]->GetFrame()->GetY2()), "Forbidden");
      ipad++;
    }
  }
  delete line;
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t, int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "GL1 Live Trigger Run:" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);
  return 0;
}

int GL1MonDraw::DrawRejection()
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("GL1MonRejection"))
  {
    MakeCanvas("GL1MonRejection");
  }
  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  TText title;
  title.SetNDC();
  title.SetTextAlign(23);
  title.SetTextColor(4);
  title.SetTextSize(0.1);
  TLine *tl = new TLine();
  tl->SetLineWidth(4);
  tl->SetLineColor(7);
  tl->SetLineStyle(2);
  std::vector<int> remap_draw_idx = {0,2,4,6,1,3,5,7};
  for (int i = 0; i < 8; i++)
  {
    std::string hname = "gl1_reject_" + std::to_string(i);
    TH1 *hist1 = cl->getHisto("GL1MON_0", hname);
    int trigno = std::stoi(hist1->GetTitle());
    if (!hist1)
    {
      DrawDeadServer(transparent[3]);
      TC[3]->SetEditable(false);
      return -1;
    }
    int ipad = remap_draw_idx[i];
    int nEntries = hist1->GetEntries();
    if (nEntries > 0)
    {
      RejPad[ipad]->cd();
      RejPad[ipad]->SetGridy();
      float *x_good = new float[nEntries];
      float *y_good = new float[nEntries];
      float *x_bad = new float[nEntries];
      float *y_bad = new float[nEntries];
      int igood_bin = 0;
      int ibad_bin = 0;
      float xmax {0.};
      float ymax {0.};
      float ymin {1000000.};
      for (int ibin = 1; ibin <= nEntries; ibin++)
      {
        float y = hist1->GetBinContent(ibin);
        if (y >= rejection_limit[i].first && y <= rejection_limit[i].second)
        {
          x_good[igood_bin] = hist1->GetBinError(ibin);
          y_good[igood_bin] = y;
          xmax = std::max(xmax,x_good[igood_bin]);
          ymax = std::max(ymax,y_good[igood_bin]);
          ymin = std::min(ymin,y_good[igood_bin]);
          igood_bin++;
        }
        else
        {
          x_bad[ibad_bin] = hist1->GetBinError(ibin);
          y_bad[ibad_bin] = y;
          xmax = std::max(xmax,x_bad[ibad_bin]);
          ymax = std::max(ymax,y_bad[ibad_bin]);
          ymin = std::min(ymin,y_bad[ibad_bin]);
          ibad_bin++;
	      }
      }
      delete reject_graph_good[i];
      delete reject_graph_bad[i];
      reject_graph_good[i] = nullptr;
      reject_graph_bad[i] = nullptr;
      if (igood_bin > 0)
      {
	      reject_graph_good[i] = new TGraph(igood_bin, x_good, y_good);
      }
      if (ibad_bin > 0)
      {
	      reject_graph_bad[i] = new TGraph(ibad_bin, x_bad, y_bad);
      }
      TH2 *h2 = new TH2F("h2", Form("%s (%d)" , m_TrignameArray[trigno].c_str(), trigno ) , 1, 0, xmax + 50, 1, 0, ymax+ymax/5.);
      h2->SetStats(0);
      h2->SetXTitle("time in Run");
      h2->SetYTitle("Rejection over MB");
      h2->GetXaxis()->SetLabelSize(0.06);
      h2->GetXaxis()->SetTitleSize(0.055);
      h2->GetXaxis()->SetTitleOffset(1.1);

      h2->GetYaxis()->SetLabelSize(0.06);
      h2->GetYaxis()->SetTitleSize(0.06);
      h2->GetYaxis()->SetTitleOffset(0.45);
      h2->SetTitle("");
      h2->DrawCopy();
      if (reject_graph_good[i])
      {
        reject_graph_good[i]->SetMarkerStyle(20);
        reject_graph_good[i]->SetMarkerSize(2.);
        reject_graph_good[i]->SetMarkerColor(3);

        reject_graph_good[i]->Draw("p same");
      }
      if (reject_graph_bad[i])
      {
        reject_graph_bad[i]->SetMarkerStyle(20);
        reject_graph_bad[i]->SetMarkerSize(2.);
        reject_graph_bad[i]->SetMarkerColor(2);
        reject_graph_bad[i]->SetMarkerColor(3);

        reject_graph_bad[i]->Draw("p same");
      }
      // tl->DrawLine(0,rejection_limit[i].first, xmax + 50,rejection_limit[i].first);
      // tl->DrawLine(0,rejection_limit[i].second, xmax + 50,rejection_limit[i].second);
      tl->DrawLine(0,ymin-ymin/10., xmax + 50,ymin-ymin/10.);
      tl->DrawLine(0,ymax+ymax/10., xmax + 50,ymax+ymax/10.);
      delete[] x_good;
      delete[] y_good;
      delete[] x_bad;
      delete[] y_bad;
      delete h2;
      title.SetTextColor(4);
      title.SetTextSize(0.1);
      title.DrawText(0.5, 0.99, Form("%s", m_TrignameArray[trigno].c_str()));
    }
  }
  delete tl;
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t, int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "GL1 Trigger Rejection Run:" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[3]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);
  return 0;
}

int GL1MonDraw::SavePlot(const std::string &what, const std::string &type)
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

int GL1MonDraw::MakeHtml(const std::string &what)
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

int GL1MonDraw::FetchTriggerNames()
{
  m_TrignameArray.fill("");
  m_RunDB->GetTriggerNames(m_TrignameArray, m_CurrentRunnumber);
  return 0;
}

int GL1MonDraw::DrawServerStats()
{
  int canvasindex = 2;
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("GL1ServerStats"))
  {
    MakeCanvas("GL1ServerStats");
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
  for (const auto &server : m_ServerSet)
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
      int errorcounts = -1;
      TH1 *gl1_stats = cl->getHisto("GL1MON_0", "gl1_stats");
      if (gl1_stats)
      {
        errorcounts = gl1_stats->GetBinContent(1);
      }
      int gl1counts = std::get<4>(servermapiter->second);
      time_t currtime = std::get<3>(servermapiter->second);
      txt << "Server " << server
          << ", run number: " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second);
      if (gl1counts >= 0)
      {
        txt << ", gl1 count: " << gl1counts;
      }
      txt << ", errors: " << errorcounts;
      txt << ", current Event time: " << ctime(&currtime);
      if (isHtml())
      {
        clienttime = currtime;  // just prevent the font from getting red
      }
      else  // print time diff only for live display
      {
        txt << ", minutes since last evt: " << (clienttime - currtime) / 60;
      }
      if (std::get<0>(servermapiter->second) && ((clienttime - currtime) / 60) < 10)
      {
        PrintRun.SetTextColor(kGray + 2);
      }
      else
      {
        PrintRun.SetTextColor(kRed);
      }
      if (errorcounts > 0)
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

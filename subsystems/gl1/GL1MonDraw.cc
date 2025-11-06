#include "GL1MonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/RunDBodbc.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TH1.h>
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
  gl1Style->SetCanvasBorderMode(0);
  oldStyle->cd();
  m_RunDB = new RunDBodbc();
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
    TC[0] = new TCanvas(name.c_str(), "GL1 Scaled Triggers", -1, 0, xsize , ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for (int i = 0; i < 2; i++)
    {
      double xlow = (0.5*i);
      double xhigh = xlow+0.5;
      for (int j = 0; j < 5; j++)
      {
	double ylow = 0.0+(0.19*j);
	double yhigh = ylow + 0.19;
	int padindex = 9 - (i + 2*j); // make it start from the top of the plot
	// std::cout << "idx: " << padindex << "pad: xl: " << xlow << ", xh: " << xhigh
	// << "pad: yl: " << ylow << ", yh: " << yhigh
	// 	  << std::endl;
	std::string padname = "gl1pad_" + std::to_string(padindex); 
	Pad[padindex] = new TPad(padname.c_str(), "who needs this?", xlow, ylow, xhigh, yhigh, 0);
	Pad[padindex]->Draw();
      }
    }
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "GL1Mon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "GL1Mon2 Example Monitor", -xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[2] = new TPad("mypad3", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[3] = new TPad("mypad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[2]->Draw();
    Pad[3]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "GL1Mon3")
  {
    TC[2] = new TCanvas(name.c_str(), "GL1Mon3 Example Monitor", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[4] = new TPad("mypad5", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[5] = new TPad("mypad6", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[4]->Draw();
    Pad[5]->Draw();
    // this one is used to plot the run number on the canvas
    //        transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    //        transparent[2]->SetFillStyle(4000);
    //        transparent[2]->Draw();
    //      TC[2]->SetEditable(0);
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
//    iret += DrawLive(what);
    idraw++;
  }
  if (what == "ALL" || what == "RAW")
  {
//    iret += DrawRaw(what);
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
  int ipad = 0;
  for (int i=0; i<64; i++)
  {
    std::string hname = "gl1_scaledtrigger_" + std::to_string(i);
    TH1 *hist1 = cl->getHisto("GL1MON_0",hname);
    if (!hist1)
    {
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(false);
      return -1;
    }
    if (hist1->GetMaximum() > 0)
    {
      Pad[ipad]->cd();
      Pad[ipad]->SetLogy();
      hist1->SetStats(0);
      std::string htitle = m_TrignameArray[i];
      std::cout << "index " << i << " title: " << htitle << std::endl;
      hist1->SetXTitle("Bunch Crossing");
      hist1->GetXaxis()->SetLabelSize(0.05);
      hist1->SetTitle("");
      hist1->DrawCopy();
      title.SetTextColor(4);
      title.SetTextSize(0.08);
      title.DrawText(0.5, 0.99, htitle.c_str());
      ipad++;
    }
  // else
  // {
  //   std::cout << "histogram " << hname << " is empty" << std::endl;
  // }
  }
  // else
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
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
  return 0;
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *mymon_hist1 = cl->getHisto("MYMON_0","mymon_hist2");
  TH1 *mymon_hist2 = cl->getHisto("MYMON_1","mymon_hist2");
  if (!gROOT->FindObject("GL1Mon2"))
  {
    MakeCanvas("GL1Mon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (mymon_hist1)
  {
    mymon_hist1->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    return -1;
  }
  Pad[3]->cd();
  if (mymon_hist2)
  {
    mymon_hist2->DrawCopy();
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
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

int GL1MonDraw::DrawRaw(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *mymon_hist1 = cl->getHisto("MYMON_0","mymon_hist2");
  TH1 *mymon_hist2 = cl->getHisto("MYMON_1","mymon_hist2");
  if (!gROOT->FindObject("GL1Mon2"))
  {
    MakeCanvas("GL1Mon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (mymon_hist1)
  {
    mymon_hist1->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    return -1;
  }
  Pad[3]->cd();
  if (mymon_hist2)
  {
    mymon_hist2->DrawCopy();
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
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

int GL1MonDraw::FetchTriggerNames()
{
  m_TrignameArray.fill("");
  m_RunDB->GetTriggerNames(m_TrignameArray,m_CurrentRunnumber);
  return 0;
}

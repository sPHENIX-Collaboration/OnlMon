#include "SepdMonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

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
  if (name == "SepdMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "SepdMon Example Monitor", 1200,600);
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
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "SepdMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "SepdMon2 Example Monitor", 1200,600);
    gSystem->ProcessEvents();
    Pad[2] = new TPad("sepdpad2", "Left", 0., 0., 0.5, 1);
    Pad[3] = new TPad("sepdpad3", "Right", 0.5, 0., 1, 1);
    Pad[2]->Draw();
    Pad[3]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "SepdMon3")
  {
    TC[2] = new TCanvas(name.c_str(), "SepdMon3 Example Monitor", 1200,600);
    gSystem->ProcessEvents();
    Pad[4] = new TPad("sepdpad4", "Left", 0., 0., 0.5, 1);
    Pad[5] = new TPad("sepdpad5", "Right", 0.5, 0., 1, 1);
    Pad[4]->Draw();
    Pad[5]->Draw();
    // this one is used to plot the run number on the canvas
    //        transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    //        transparent[2]->SetFillStyle(4000);
    //        transparent[2]->Draw();
    //      TC[2]->SetEditable(0);
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
  /*
  if (what == "ALL" || what == "THIRD")
  {
    iret += DrawThird(what);
    idraw++;
  }
 
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
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_ADC0_s  = (TH2*) cl->getHisto("SEPDMON_0","h_ADC0_s");
  TH2 *h_ADC0_n  = (TH2*) cl->getHisto("SEPDMON_0","h_ADC0_n");
  TH2 *h_ADC_s   = (TH2*) cl->getHisto("SEPDMON_0","h_ADC_s");
  TH2 *h_ADC_n   = (TH2*) cl->getHisto("SEPDMON_0","h_ADC_n");
  TH2 *h_hits0_s = (TH2*) cl->getHisto("SEPDMON_0","h_hits0_s");
  TH2 *h_hits0_n = (TH2*) cl->getHisto("SEPDMON_0","h_hits0_n");
  TH2 *h_hits_s  = (TH2*) cl->getHisto("SEPDMON_0","h_hits_s");
  TH2 *h_hits_n  = (TH2*) cl->getHisto("SEPDMON_0","h_hits_n");
  time_t evttime = cl->EventTime("SEPDMON_0","CURRENT");
  
  if (!gROOT->FindObject("SepdMon1"))
  {
    MakeCanvas("SepdMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  
  if(!h_ADC0_s)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  

  int nbinsx0 = h_ADC0_s->GetNbinsX();
  int nbinsy0 = h_ADC0_s->GetNbinsY(); 
  int nbinsx = h_ADC_s->GetNbinsX();
  int nbinsy = h_ADC_s->GetNbinsY(); 

  for(int ibx = 0; ibx<nbinsx0; ibx++){
    for(int iby = 0; iby<nbinsy0; iby++){
      double con = h_ADC0_s->GetBinContent(ibx+1,iby+1);
      double div = h_hits0_s->GetBinContent(ibx+1,iby+1);
      h_ADC0_s->SetBinContent(ibx+1,iby+1,con/div);
      con =  h_ADC0_n->GetBinContent(ibx+1,iby+1);
      div = h_hits0_n->GetBinContent(ibx+1,iby+1);
      h_ADC0_n->SetBinContent(ibx+1,iby+1,con/div);
    }
  }

  for(int ibx = 0; ibx<nbinsx; ibx++){
    for(int iby = 0; iby<nbinsy; iby++){
      double con = h_ADC_s->GetBinContent(ibx+1,iby+1);
      double div = h_hits_s->GetBinContent(ibx+1,iby+1);
      h_ADC_s->SetBinContent(ibx+1,iby+1,con/div);
      con =  h_ADC_n->GetBinContent(ibx+1,iby+1);
      div = h_hits_n->GetBinContent(ibx+1,iby+1);
      h_ADC_n->SetBinContent(ibx+1,iby+1,con/div);
    }
  }

  Pad[0]->cd();
  h_ADC0_s->Draw("COLZPOL");
  h_ADC_s->Draw("COLZPOL same");
  Pad[1]->cd();
  h_ADC0_n->Draw("COLZPOL");
  h_ADC_n->Draw("COLZPOL same");
  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);
  return 0;
}

int SepdMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_hits0_s = (TH2*) cl->getHisto("SEPDMON_0","h_hits0_s");
  TH2 *h_hits0_n = (TH2*) cl->getHisto("SEPDMON_0","h_hits0_n");
  TH2 *h_hits_s  = (TH2*) cl->getHisto("SEPDMON_0","h_hits_s");
  TH2 *h_hits_n  = (TH2*) cl->getHisto("SEPDMON_0","h_hits_n");

  TH1 *h_event = cl->getHisto("SEPDMON_0","h_event");
  time_t evttime = cl->EventTime("SEPDMON_0","CURRENT");

  if (!gROOT->FindObject("SepdMon2"))
  {
    MakeCanvas("SepdMon2");
  }
  if(!h_hits0_s)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  int nevt = h_event->GetEntries();
  h_hits0_s->Scale(1./nevt);
  h_hits_s->Scale(1./nevt);
  h_hits0_n->Scale(1./nevt);
  h_hits_n->Scale(1./nevt);

  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[2]->cd();
  h_hits0_s->Draw("COLZPOL");
  h_hits_s->Draw("COLZPOL same");
  Pad[3]->cd();
  h_hits0_n->Draw("COLZPOL");
  h_hits_n->Draw("COLZPOL same");
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);
  return 0;
}

int SepdMonDraw::DrawThird(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_ADC_corr = (TH2*) cl->getHisto("SEPDMON_0","h_ADC_corr");
  TH2 *h_hits_corr = (TH2*) cl->getHisto("SEPDMON_0","h_hits_corr");
  time_t evttime = cl->EventTime("SEPDMON_0","CURRENT");
  if (!gROOT->FindObject("SepdMon3"))
  {
    MakeCanvas("SepdMon3");
  }
  if(!h_ADC_corr)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  TC[2]->SetEditable(true);
  TC[2]->Clear("D");
  Pad[4]->cd();
  h_ADC_corr->Draw("COLZ");
  Pad[5]->cd();
  h_hits_corr->Draw("COLZ");
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(false);
  return 0;
}

int SepdMonDraw::MakePS(const std::string &what)
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

int SepdMonDraw::MakeHtml(const std::string &what)
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

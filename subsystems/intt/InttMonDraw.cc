#include "InttMonDraw.h"

#include <TPolyLine.h>

InttMonDraw::InttMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
}

InttMonDraw::~InttMonDraw()
{
}

int InttMonDraw::Init()
{
  return 0;
}

int InttMonDraw::Draw(const std::string& what)
{
  int idraw = 0;
  int iret = 0;
  int icnvs = 0;  // So each draws to the canvas at TC[icnvs]

  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "chip_hitmap")
  {
    iret += DrawHitMap(icnvs);
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "bco_diff") // Expert
  {
    iret += DrawFelixBcoFphxBco(icnvs);
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "peaks")
  {
    iret += DrawPeaks(icnvs);
    ++idraw;
  }
  ++icnvs;

  if(what == "ALL" || what == "hitrates") // Expert
  {
	  iret += DrawHitRates(icnvs);
	  ++idraw;
  }
  ++icnvs;

  if(!idraw) {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tUnimplemented drawing option \"" << what << "\"" << std::endl;
  }

  return iret;
}

int InttMonDraw::MakeHtml(const std::string& what)
{
  if (Draw(what))
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
    return 1;
  }

  OnlMonClient* cl = OnlMonClient::instance();

  //  this code must not be modified
  Draw("SERVERSTATS");

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

  return 0;
}

int InttMonDraw::SavePlot(std::string const& what, std::string const& type)
{
  if (Draw(what))
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
    return 1;
  }

  OnlMonClient* cl = OnlMonClient::instance();

  int icnt = 0;
  for (TCanvas* canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    ++icnt;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" + std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}

int InttMonDraw::MakeCanvas(const std::string& name)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "InttMonServerStats")
  {
    TC[0] = new TCanvas(name.c_str(), "InttMon Server Stats", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    transparent[0] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillColor(kGray);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
    TC[0]->SetTopMargin(0.05);
    TC[0]->SetBottomMargin(0.05);
  }
  return 0;
}

int InttMonDraw::DrawServerStats()
{
  OnlMonClient* cl = OnlMonClient::instance();
  if (!gROOT->FindObject("InttMonServerStats"))
  {
    MakeCanvas("InttMonServerStats");
  }
  TC[0]->Clear("D");
  TC[0]->SetEditable(true);
  transparent[0]->cd();
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
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);

  return 0;
}

void InttMonDraw::DrawPad(
    TPad* b,
    TPad* p)
{
  if (!b) return;
  if (!p) return;

  p->SetFillStyle(4000);  // transparent
  p->Range(0.0, 0.0, 1.0, 1.0);

  CdPad(b);
  p->Draw();
  CdPad(p);
}

void InttMonDraw::CdPad(
    TPad* p)
{
  if (!p) return;

  p->cd();
  gROOT->SetSelectedPad(p);  // So TObject::DrawClone draws where you expect
}

Color_t
InttMonDraw::GetFeeColor(
    int const& fee)
{
  switch (fee % 7)
  {
  case 0:
    return (fee / 7) ? kGray + 3 : kBlack;
  case 1:
    return kRed + 3 * (fee / 7);
  case 2:
    return kYellow + 3 * (fee / 7);
  case 3:
    return kGreen + 3 * (fee / 7);
  case 4:
    return kCyan + 3 * (fee / 7);
  case 5:
    return kBlue + 3 * (fee / 7);
  case 6:
    return kMagenta + 3 * (fee / 7);
  }
  return kBlack;
}

int InttMonDraw::Draw_DispPad (
	int icnvs,
	std::string const& m_name
) {
  std::string name;

  name = Form("%s_disp_pad", m_name.c_str());
  TPad* disp_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if(!disp_pad) {
 	disp_pad = new TPad (
      name.c_str(), name.c_str(),
      0.0, 1.0 - m_disp_frac, // Southwest x, y
      1.0, 1.0                // Northeast x, y
    );
    DrawPad(TC[icnvs], disp_pad);
  }
  CdPad(disp_pad);

  // Display info
  name = Form("%s_disp_text", m_name.c_str());
  TText* disp_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if(!disp_text) {
 	disp_text = new TText(0.5, 0.5, name.c_str());
	disp_text->SetName(name.c_str());
    disp_text->SetTextAlign(22);
    disp_text->SetTextSize(m_disp_text_size);
    disp_text->Draw();
  }

  // Disclaimer if there are not enough events
  name = Form("%s_warn_text", m_name.c_str());
  TText* warn_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if(!warn_text) {
    warn_text = new TText(0.5, 0.25, name.c_str());
    warn_text->SetName(name.c_str());
    warn_text->SetTextAlign(22);
    warn_text->SetTextSize(m_warn_text_size);
    warn_text->SetTextColor(kRed);
    warn_text->Draw();
  }

  // Title
  name = Form("%s_title_text", m_name.c_str());
  TText* title_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if(!title_text)
  {
    title_text = new TText(0.5, 0.75, name.c_str());
    title_text->SetName(name.c_str());
    title_text->SetTextAlign(22);
    title_text->SetTextSize(m_disp_text_size);
    title_text->Draw();
    name = Form("%s", m_name.c_str());
    title_text->SetTitle(name.c_str());
  }

  // Update after making
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttEvtHist";
  TH1D* evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", 0), name);
  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
    return 1;
  }

  // Update display text
  std::time_t t = cl->EventTime("CURRENT");  // BOR, CURRENT, or EOR
  struct tm* ts = std::localtime(&t);
  name = Form (
    "Run: %08d, Events: %d, Date: %02d/%02d/%4d", //
    cl->RunNumber(), //
    (int) evt_hist->GetBinContent(1), //
    ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900 //
  );
  disp_text->SetTitle(name.c_str());

  // Update warn text
  if (evt_hist->GetBinContent(1) < m_min_events) {
  	name = Form (
      "Not enough events (min %0.E) to be statistically significant yet",
	  m_min_events
	);
  } else {
    name = " ";
  }
  warn_text->SetTitle(name.c_str());

  return 0;
}


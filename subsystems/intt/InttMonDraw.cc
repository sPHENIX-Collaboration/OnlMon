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

  // if (what == "ALL" || what == "chip_hitmap")
  // {
  //   iret += DrawHitMap(icnvs);
  //   ++idraw;
  // }
  // ++icnvs;

  // if (what == "ALL" || what == "bco_diff") // Expert
  // {
  //   iret += DrawFelixBcoFphxBco(icnvs);
  //   ++idraw;
  // }
  // ++icnvs;

  // if (what == "ALL" || what == "peaks")
  // {
  //   iret += DrawPeaks(icnvs);
  //   ++idraw;
  // }
  // ++icnvs;

  if(what == "ALL" || what == "hitrates") // Expert
  {
	  iret += Draw_HitRates(icnvs);
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

int
InttMonDraw::MakeCanvas_Generic (
	int icnvs
) {
	std::string name;

    name = Form("%s", m_name.c_str());
    if(dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()))) return 0;

	TC[icnvs] = new TCanvas (
    	name.c_str(), name.c_str(), //
    	m_cnvs_width, m_cnvs_height //
    );
    gSystem->ProcessEvents();

	if(m_disp_frac) {
		name = Form("%s_disp_pad", m_name.c_str());
		TP[icnvs][k_disp_pad] = new TPad (
				name.c_str(), name.c_str(), //
				0.0, 1.0 - m_disp_frac,     // Southwest x, y
				1.0, 1.0                    // Northeast x, y
		);
	}

	if(m_lgnd_frac) {
		name = Form("%s_lgnd_pad", m_name.c_str());
		TP[icnvs][k_lgnd_pad] = new TPad (
				name.c_str(), name.c_str(), //
				1.0 - m_lgnd_frac, 0.0,     // Southwest x, y
				1.0, 1.0                    // Northeast x, y
		);
	}

	for(int i = 0; i < 8; ++i) {
		name = Form("%s_hist_pad_%01d", m_name.c_str(), i);
		TP[icnvs][i] = new TPad (
			name.c_str(), name.c_str(), //
			(i % 4 + 0.0) / 4.0 * (1.0 - m_lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac), // Southwest x, y
			(i % 4 + 1.0) / 4.0 * (1.0 - m_lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)  // Northeast x, y
		);
		DrawPad(TC[icnvs], TP[icnvs][i]);
	}

	return 0;
}

int InttMonDraw::DrawDispPad_Generic (
	int icnvs
) {
	std::string name;

 	CdPad(TP[icnvs][k_disp_pad]);
	TP[icnvs][k_disp_pad]->Clear();

 	OnlMonClient* cl = OnlMonClient::instance();

 	name = "InttEvtHist";
	TH1D* evt_hist = nullptr;
	for (int i = 0; i < 8; ++i)
	{
		if((evt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name))))break;
	}

 	if(!evt_hist)
	{
		name = "NO CONNECTION TO ANY INTT ONLMON SERVERS";
		TText title_text(0.5, 0.75, name.c_str());
		title_text.SetTextAlign(22);
		title_text.SetTextSize(m_disp_text_size);
		title_text.Draw();
	
	   	std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		<< "\tCould not get \"" << name << "\" from any server" << std::endl;
		return 1;
	}

 	// Title
	name = Form("%s", m_name.c_str());
	TText title_text(0.5, 0.75, name.c_str());
	title_text.SetTextAlign(22);
	title_text.SetTextSize(m_disp_text_size);
	title_text.Draw();

 	// Display text
	std::time_t t = cl->EventTime("CURRENT"); // BOR, CURRENT, or EOR
	struct tm* ts = std::localtime(&t);
	name = Form (
		"Run: %08d, Events: %d, Date: %02d/%02d/%4d",    //
		cl->RunNumber(),                                 //
		(int) evt_hist->GetBinContent(1),                //
		ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900  //
	);
	TText disp_text(0.5, 0.5, name.c_str());
	disp_text.SetTextAlign(22);
	disp_text.SetTextSize(m_disp_text_size);
	disp_text.Draw();

 	if (m_min_events < evt_hist->GetBinContent(1))return 0;

	// Disclaimer if not enough events
	name = Form("Not statistically significant (fewer than  %0.E events)", m_min_events);
	TText warn_text(0.5, 0.25, name.c_str());
	warn_text.SetTextAlign(22);
	warn_text.SetTextSize(m_warn_text_size);
	warn_text.SetTextColor(kRed);
	warn_text.Draw();

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

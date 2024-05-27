#include "InttMonDraw.h"

#include <TPolyLine.h>

InttMonDraw::InttMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
}

InttMonDraw::~InttMonDraw()
{
	delete m_style;

	for(int i = 0; i < 8; ++i)
	{
		for(int j = 0; j < 14; ++j)
		{
			delete m_hist_felixbcofphxbco[i][j];
		}
	}

	for(int i = 0; i < 8; ++i)
	{
		delete m_hist_hitmap[i];
	}

	for(int i = 0; i < 8; ++i)
	{
		delete m_hist_hitrates[i];
	}
}

int InttMonDraw::Init()
{
  m_style = new TStyle("INTT_Style", "INTT_Style");
  m_style->SetOptStat(0);

  Int_t palette[3] = {kBlue, kGreen, kRed};
  m_style->SetPalette(3, palette);
  // m_style->SetNumberContours(3);

  return 0;
}

int InttMonDraw::Draw(const std::string& what)
{
  int idraw = 0;
  int iret = 0;

  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    ++idraw;
  }

  if (what == "ALL" || what == "chip_hitmap")
  {
    iret += Draw_HitMap();
    ++idraw;
  }

  if (what == "ALL" || what == "bco_diff")
  {
    iret += Draw_FelixBcoFphxBco();
    ++idraw;
  }

  // if (what == "ALL" || what == "peaks")
  // {
  //   iret += Draw_Peaks(icnvs);
  //   ++idraw;
  // }

  if (what == "ALL" || what == "hitrates")
  {
    iret += Draw_HitRates();
    ++idraw;
  }

  if (!idraw)
  {
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

	// Caller sets m_name
	if(m_name.empty())return 1;

    name = Form("%s", m_name.c_str());
    if( gROOT->FindObject(name.c_str()) ) return 0;

	TC[icnvs] = new TCanvas (
    	name.c_str(), name.c_str(), //
    	m_cnvs_width, m_cnvs_height //
    );
    gSystem->ProcessEvents();
	TC[icnvs]->SetEditable(true);

	name = Form("%s_disp_pad", m_name.c_str());
	m_disp_pad[icnvs] = new TPad (
			name.c_str(), name.c_str(), //
			0.0, 1.0 - m_disp_frac,     // Southwest x, y
			1.0, 1.0                    // Northeast x, y
	);
	TC[icnvs]->cd();
	m_disp_pad[icnvs]->SetFillStyle(4000); // Transparent
	m_disp_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
	m_disp_pad[icnvs]->Draw();

	// Some methods do not need a legend, test this member variable
	if(m_lgnd_frac) {
		name = Form("%s_lgnd_pad", m_name.c_str());
		m_lgnd_pad[icnvs] = new TPad (
				name.c_str(), name.c_str(), //
				1.0 - m_lgnd_frac, 0.0,     // Southwest x, y
				1.0, 1.0 - m_disp_frac      // Northeast x, y
		);
		TC[icnvs]->cd();
		m_lgnd_pad[icnvs]->SetFillStyle(4000); // Transparent
		m_lgnd_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
		m_lgnd_pad[icnvs]->Draw();
	}

	for(int i = 0; i < 8; ++i) {
		name = Form("%s_hist_pad_%01d", m_name.c_str(), i);
		m_hist_pad[icnvs][i] = new TPad (
			name.c_str(), name.c_str(), //
			(i % 4 + 0.0) / 4.0 * (1.0 - m_lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac), // Southwest x, y
			(i % 4 + 1.0) / 4.0 * (1.0 - m_lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)  // Northeast x, y
		);
		TC[icnvs]->cd();
		m_hist_pad[icnvs][i]->SetFillStyle(4000); // Transparent
		m_hist_pad[icnvs][i]->Range(0.0, 0.0, 1.0, 1.0);
		m_hist_pad[icnvs][i]->Draw();
	}

	return 0;
}

int InttMonDraw::DrawDispPad_Generic (
	int icnvs
) {
	std::string name;

	m_disp_pad[icnvs]->Clear();
 	m_disp_pad[icnvs]->cd();;

 	OnlMonClient* cl = OnlMonClient::instance();

	// Get the "event" count from the first server we can
 	name = "InttEvtHist";
	TH1D* evt_hist = nullptr;
	for (int i = 0; i < 8; ++i)
	{
		if((evt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name))))break;
	}

	// If we can't find any, there is a problem
 	if(!evt_hist)
	{
		TText title_text(0.5, 0.75, "NO CONNECTION TO ANY INTT ONLMON SERVERS");
		title_text.SetTextAlign(22);
		title_text.SetTextSize(m_disp_text_size);
		title_text.SetTextColor(kRed);
		title_text.Draw();
	
	   	std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		<< "\tCould not get \"" << name << "\" from any server" << std::endl;
		return 1;
	}

 	// Title
	name = Form("%s", m_name.c_str());
	TText title_text;
	title_text.SetTextAlign(22);
	title_text.SetTextSize(m_disp_text_size);
	title_text.DrawText(0.5, 0.75, name.c_str());

 	// Display text
	std::time_t t = cl->EventTime("CURRENT"); // BOR, CURRENT, or EOR
	struct tm* ts = std::localtime(&t);
	name = Form (
		"Run: %08d, Events: %d, Date: %02d/%02d/%4d %02d:%02d", //
		cl->RunNumber(),                                        //
		(int) evt_hist->GetBinContent(1),                       //
		ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900,        //
		ts->tm_hour, ts->tm_min                                 //
	);
	TText disp_text;
	disp_text.SetTextAlign(22);
	disp_text.SetTextSize(m_disp_text_size);
	disp_text.DrawText(0.5, 0.5, name.c_str());

 	if (m_min_events < evt_hist->GetBinContent(1))return 0;

	// Disclaimer if not enough events
	name = Form("Not statistically significant (fewer than  %0.E events)", m_min_events);
	TText warn_text;
	warn_text.SetTextAlign(22);
	warn_text.SetTextSize(m_warn_text_size);
	warn_text.SetTextColor(kRed);
	warn_text.DrawText(0.5, 0.25, name.c_str());

 	return 0;
}

//====== FelixBcoFphxBco ======//

int
InttMonDraw::Draw_FelixBcoFphxBco(
) {
  // Set global values we use to what they should be at the beginning of each call
  m_name = "INTT_FelixBco_FphxBco_Diff";
  m_lgnd_frac = 0.15;
  m_style->cd();

  std::string name;

  MakeCanvas_Generic(k_felixbcofphxbco);
  TC[k_felixbcofphxbco]->SetEditable(true);

  DrawDispPad_Generic(k_felixbcofphxbco);
  DrawLgndPad_FelixBcoFphxBco();

  int iret = 1;
  for(int i = 0; i < 8; ++i) {
	  // If any subdraw succeeds, say the entire call succeeds
	  iret = DrawHistPad_FelixBcoFphxBco(i) && iret;
  }
  
  TC[k_felixbcofphxbco]->Update();
  TC[k_felixbcofphxbco]->Show();
  TC[k_felixbcofphxbco]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawLgndPad_FelixBcoFphxBco(
) {
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad[k_felixbcofphxbco]->Clear();
  m_lgnd_pad[k_felixbcofphxbco]->cd();

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, Form("FCh %2d", fee));

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  return 0;
}

int InttMonDraw::DrawHistPad_FelixBcoFphxBco(
    int i
) {
  std::string name;

  // Validate member histos
  struct InttMon::BcoData_s bco_data;
  for (bco_data.fee = 0; bco_data.fee < 14; ++bco_data.fee)
  {
    name = Form("%s_hist_%01d_%02d", m_name.c_str(), i, bco_data.fee);
    if (!m_hist_felixbcofphxbco[i][bco_data.fee])
    {
      m_hist_felixbcofphxbco[i][bco_data.fee] = new TH1D(
          name.c_str(), name.c_str(), //
          128, 0, 128                 //
      );
      m_hist_felixbcofphxbco[i][bco_data.fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
      m_hist_felixbcofphxbco[i][bco_data.fee]->GetXaxis()->SetNdivisions(16);  //, true);
      m_hist_felixbcofphxbco[i][bco_data.fee]->SetLineColor(GetFeeColor(bco_data.fee));
      m_hist_felixbcofphxbco[i][bco_data.fee]->SetFillStyle(4000); // Transparent
    }
	m_hist_pad[k_felixbcofphxbco][i]->SetLogy();
	m_hist_pad[k_felixbcofphxbco][i]->cd();

    m_hist_felixbcofphxbco[i][bco_data.fee]->Reset();
    m_hist_felixbcofphxbco[i][bco_data.fee]->Draw("same");
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttBcoHist";
  TH1D* bco_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  // Fill
  int bin, max = 0;
  for (bco_data.fee = 0; bco_data.fee < 14; ++bco_data.fee)
  {
    for (bco_data.bco = 0; bco_data.bco < 128; ++bco_data.bco)
    {
      bin = InttMon::BcoBin(bco_data);    // Which bin has the data we want (retrieved by helper method)
      bin = bco_hist->GetBinContent(bin); // Reuse the index as the value in that bin
      if (bin > max) max = bin;
      m_hist_felixbcofphxbco[i][bco_data.fee]->SetBinContent(bco_data.bco + 1, bin);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  // Noramlize ranges
  for (bco_data.fee = 0; bco_data.fee < 14; ++bco_data.fee)
  {
    m_hist_felixbcofphxbco[i][bco_data.fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
  }

  return 0;
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

//====== HitMap ======//

int InttMonDraw::Draw_HitMap(
) {
  // Set member variables we use to what they should be at beginning of each call
  m_name = "INTT_HitMap";
  m_lgnd_frac = 0.1;
  m_style->cd();

  std::string name;

  MakeCanvas_Generic(k_hitmap);
  TC[k_hitmap]->SetEditable(true);

  DrawDispPad_Generic(k_hitmap);
  DrawLgndPad_HitMap();

  int iret = 1;
  for(int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_HitMap(i) && iret;
  }

  TC[k_hitmap]->Update();
  TC[k_hitmap]->Show();
  TC[k_hitmap]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawLgndPad_HitMap()
{
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.03;
  double lgnd_text_size = 0.12;

  std::string name;

  m_lgnd_pad[k_hitmap]->Clear();
  m_lgnd_pad[k_hitmap]->cd();

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
  for (int c = 0; c < 3; ++c)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * c + 1.0) / (2.0 * 3);

    switch (c)
    {
    case 0:
      label = "Cold";
      color = kBlue;
      break;
    case 1:
      label = "Good";
      color = kGreen;
      break;
    case 2:
      label = "Hot";
      color = kRed;
      break;
    }

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(
        x0 + 1.5 * lgnd_box_width, y0, //
        label.c_str() //
	);

	x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
	y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(color);
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  return 0;
}

int InttMonDraw::DrawHistPad_HitMap(
    int i)
{
  double lower = 10e-4, upper = 10e-2;

  std::string name;

  name = Form("%s_hist_%01d", m_name.c_str(), i);
  if(!m_hist_hitmap[i])
  {
    m_hist_hitmap[i] = new TH2D(
        name.c_str(), name.c_str(),
        26, 0, 26,  // 26, -0.5, 25.5,
        14, 0, 14   // 14, -0.5, 13.5
    );
    m_hist_hitmap[i]->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));

    m_hist_hitmap[i]->GetXaxis()->SetNdivisions(13, true);
    m_hist_hitmap[i]->GetYaxis()->SetNdivisions(14, true);
    m_hist_hitmap[i]->GetZaxis()->SetRangeUser(0, 3);
    m_hist_hitmap[i]->SetFillStyle(4000); // Transparent

    Double_t levels[4] = {0, 1, 2, 3};
    m_hist_hitmap[i]->SetContour(4, levels);
  }
  m_hist_pad[k_hitmap][i]->cd();

  m_hist_hitmap[i]->Reset();
  m_hist_hitmap[i]->Draw("COL");  // "COLZ" for a legend, "COL" for no legend; no legend is preferrable here

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttEvtHist";
  TH1D* evt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  name = "InttHitHist";
  TH1D* bco_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  // Fill
  double bin;
  struct InttMon::HitData_s hit_data;
  for (hit_data.fee = 0; hit_data.fee < 14; ++hit_data.fee)
  {
    for (hit_data.chp = 0; hit_data.chp < 26; ++hit_data.chp)
    {
      bin = InttMon::HitBin(hit_data);           // Which bin has the data we want
      bin = bco_hist->GetBinContent((int) bin);  // Reuse the index as the value in that bin
      bin /= evt_hist->GetBinContent(1);         // Normalize by number of events

      // Assign a value to this bin
      // that will give it the appropriate color
      // based on how it compares to the hot/cold thresholds
      if (bin < lower)
      {
        bin = 0.4;  // 0.4 Cold/Dead
      }
      else if (upper < bin)
      {
        bin = 3.0;  // 3.0 Hot
      }
      else
      {
        bin = 1.7;  // 1.7 Good
      }

      m_hist_hitmap[i]->SetBinContent(
          hit_data.chp + 1,  // + 1 is b/c the 0th x bin is an underflow bin
          hit_data.fee + 1,  // + 1 is b/c the 0th y bin is an underflow bin
          bin // 
	  );
    }
  }

  return 0;
}

//====== HitRates ======//

int
InttMonDraw::Draw_HitRates (
) {
  // Set member variables we use to what they should be at beginning of each call
  m_name = "INTT_HitRates";
  m_lgnd_frac = 0.0;
  m_style->cd();

  std::string name;

  MakeCanvas_Generic(k_hitrates);
  TC[k_hitrates]->SetEditable(true);

  DrawDispPad_Generic(k_hitrates);

  int iret = 1;
  for(int i = 0; i < 8; ++i) {
	  // If any subdraw succeeds, say the entire call succeeds
	  iret = DrawHistPad_HitRates(i) && iret;
  }
  
  TC[k_hitrates]->Update();
  TC[k_hitrates]->Show();
  TC[k_hitrates]->SetEditable(false);
  
  return iret;
}

int
InttMonDraw::DrawHistPad_HitRates (
	int i 
) {
	double m_lower = 0.0, m_upper = 0.02;

	std::string name;

	// Validate member histos
	name = Form("%s_hist_%01d", m_name.c_str(), i);
	if(!m_hist_hitrates[i]) {
		m_hist_hitrates[i] = new TH1D (
			name.c_str(), name.c_str(), //
			112, m_lower, m_upper       //
		);
		m_hist_hitrates[i]->SetTitle(Form("intt%01d;Hits/Event (overflow is shown in last bin);Entries (One Hitrate per Chip)", i));
		m_hist_hitrates[i]->GetXaxis()->SetNdivisions(8, true);
		m_hist_hitrates[i]->SetFillStyle(4000); // Transparent
	}
	m_hist_pad[k_hitrates][i]->cd();

	m_hist_hitrates[i]->Reset();
	m_hist_hitrates[i]->Draw();

	// Access client
	OnlMonClient* cl = OnlMonClient::instance();

	name = "InttEvtHist";
	TH1D* evt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
	if (!evt_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
		return 1;
	}

	name = "InttHitHist";
	TH1D* bco_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
	if (!bco_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
		return 1;
	}

	// Fill
	double bin;
	struct InttMon::HitData_s hit_data;
	for(hit_data.fee = 0; hit_data.fee < 14; ++hit_data.fee) {
		for(hit_data.chp = 0; hit_data.chp < 26; ++hit_data.chp) {
			bin = InttMon::HitBin(hit_data);         // Which bin has the data we want (retrieved by helper method)
			bin = bco_hist->GetBinContent((int)bin); // Reuse the index as the value in that bin
			bin /= evt_hist->GetBinContent(1);       // Normalize by number of events

			// Manually catch overflows and put them in the last displayed bin
			if(m_upper <= bin) {
				bin = m_upper - m_hist_hitrates[i]->GetXaxis()->GetBinWidth(1);
			}

			m_hist_hitrates[i]->Fill(bin);
		}
	}

	return 0;
}


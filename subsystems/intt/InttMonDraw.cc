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
  for (int i = 0; i < 8; ++i)
  {
    std::string name;
    // Histogram creation and configuration
    name = Form("INTT_HitMap_hist_%01d", i);
    hist[i] = new TH2D(
        name.c_str(), name.c_str(),
        26, 0, 26,  // 26, -0.5, 25.5,
        14, 0, 14   // 14, -0.5, 13.5
    );
    hist[i]->GetXaxis()->SetNdivisions(13, true);
    hist[i]->GetYaxis()->SetNdivisions(14, true);
    hist[i]->GetZaxis()->SetRangeUser(0, 3);

    Double_t levels[4] = {0, 1, 2, 3};
    hist[i]->SetContour(4, levels);
    for (int fee = 0; fee < 14; ++fee)
    {
      name = Form("%s_hist_%01d_%02d", "INTT_FelixBco_FphxBco_Diff", i, fee);

      hist_FB[i][fee] = new TH1D(
          name.c_str(), name.c_str(),
          128,
          0, 127);
      hist_FB[i][fee]->GetXaxis()->SetNdivisions(16);  // , true);
    }
  }

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
    iret += DrawHitMap();
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "bco_diff")
  {
    iret += Draw_FelixBcoFphxBco(icnvs);
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "peaks")
  {
    iret += DrawPeaks(icnvs);
    ++idraw;
  }
  ++icnvs;

  if (what == "ALL" || what == "hitrates")
  {
    iret += Draw_HitRates(icnvs);
    ++idraw;
  }
  ++icnvs;

  if (!idraw)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tUnimplemented drawing option \"" << what << "\"" << std::endl;
  }

  return iret;
}
int InttMonDraw::DrawPeaks(int i){

  return i;
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
  if (name == "InttMonHitMap")
  {
    TC[1] = new TCanvas(name.c_str(), "InttMon Hit Map", xsize, 0, xsize, ysize);
    gSystem->ProcessEvents();
    disp_pad[0] = new TPad(
        "INTT_HitMap", "INTT_HitMap",
        0.0, 0.9,  // Southwest x, y
        0.9, 1.0   // Northeast x, y
    );
    disp_pad[0]->Draw();

    lgnd_pad[0] = new TPad(
        "lgnd_pad", "lgnd_pad",
        0.9, 0.9,  // Southwest x, y (1.0 - m_HitMap.lgnd_frac, 1.0 - m_HitMap.disp_frac)
        1.0, 1.0   // Northeast x, y
    );
    lgnd_pad[0]->Draw();
    const double x_min = 0.0;
    const double x_max = 0.9;
    const double y_min = 0.0;
    const double y_max = 0.9;

    for (int i = 0; i < 8; ++i)
    {
      std::string namepad = Form("INTT_HitMap_hist_pad_%d", i);

      Pad_hit_hist[i] = new TPad(
          namepad.c_str(), namepad.c_str(),
          x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
          x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Northeast x, y
      );
      Pad_hit_hist[i]->SetRightMargin(0.2);
      Pad_hit_hist[i]->Draw();
    }

    transparent[1] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillColor(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  if (name == "InttMonFelixBcoFphxBco")
  {
    TC[2] = new TCanvas(name.c_str(), "InttMon Felix BCO vs FPHX BCO", xsize, 0, xsize, ysize);
    gSystem->ProcessEvents();
    disp_pad[1] = new TPad(
        "INTT_FelixBcoFphxBco", "INTT_FelixBcoFphxBco",
        0.0, 0.9,  // Southwest x, y
        0.9, 1.0   // Northeast x, y
    );
    disp_pad[1]->Draw();

    lgnd_pad[1] = new TPad(
        "lgnd_pad", "lgnd_pad",
        0.9, 0.9,  // Southwest x, y (1.0 - m_FelixBcoFphxBco.lgnd_frac, 1.0 - m_FelixBcoFphxBco.disp_frac)
        1.0, 1.0   // Northeast x, y
    );
    lgnd_pad[1]->Draw();
    const double x_min = 0.0;
    const double x_max = 0.8;
    const double y_min = 0.0;
    const double y_max = 0.8;

    for (int i = 0; i < 8; ++i)
    {
      std::string namepad = Form("INTT_FelixBcoFphxBco_hist_pad_%d", i);

      Pad_felixbcofphxbco_hist[i] = new TPad(
          namepad.c_str(), namepad.c_str(),
          x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
          x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Northeast x, y
      );
      Pad_felixbcofphxbco_hist[i]->SetRightMargin(0.2);
      Pad_felixbcofphxbco_hist[i]->Draw();
    }

    transparent[2] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillColor(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
  }
  return 0;
}

int InttMonDraw::DrawFelixBcoFphxBco()
{
  OnlMonClient* cl = OnlMonClient::instance();

  TH1D* evt_hist = (TH1D*) cl->getHisto("INTTMON_0", "InttEvtHist");

  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"InttEvtHist\" from INTTMON_0" << std::endl;
    return 1;
  }

  if (!gROOT->FindObject("InttMonHitMap"))
  {
    MakeCanvas("InttMonHitMap");
  }
  TC[2]->Clear("D");
  TC[2]->SetEditable(true);
  disp_pad[1]->cd();

  // Display text
  TText disp_text(0.5, 0.5, "INTT_HitMap_disp_text");
  disp_text.SetTextAlign(22);
  disp_text.SetTextSize(0.25);
  std::ostringstream displaytext;
  time_t evttime = cl->EventTime("CURRENT");
  displaytext << "Run " << cl->RunNumber() << ", Event: " << evt_hist->GetBinContent(1) << ", Time: " << ctime(&evttime);
  disp_text.DrawText(0.5, 0.5, displaytext.str().c_str());

  // Title text
  TText title_text(0.5, 0.75, "INTT_HitMap_title_text");
  title_text.SetTextAlign(22);
  title_text.SetTextSize(0.25);  // m_FelixBcoFphxBco.disp_text_size
  title_text.DrawText(0.5, 0.75, "INTT_HitMap_title_text");
  title_text.SetTitle("INTT_HitMap");

  // legend
  lgnd_pad[1]->cd();
  std::string label;

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - m_FelixBcoFphxBco.lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text(x0 + 1.5 * m_FelixBcoFphxBco.lgnd_box_width, y0, Form("FCh %2d", fee));
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(m_FelixBcoFphxBco.lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * m_FelixBcoFphxBco.lgnd_box_width, y0, Form("FCh %2d", fee));

    x[0] = -1;
    x[1] = +1;
    x[2] = +1;
    x[3] = -1;
    y[0] = -1;
    y[1] = -1;
    y[2] = +1;
    y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box(4, x, y);
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.Draw("f");
  }

  for (int i = 0; i < 8; ++i)
  {
    TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), "InttEvtHist");
    if (!bco_hist)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
                << "\tCould not get \"InttEvtHist\" from INTTMON_" << i << std::endl;
      continue;
    }

    std::string name;

    Pad_felixbcofphxbco_hist[i]->cd();

    int max = 0;
    int bin;
    struct InttMon::BcoData_s bco_data;
    for (int fee = 0; fee < 14; ++fee)
    {
      hist_FB[i][fee]->Reset();

      // Fill
      bco_data.fee = fee;
      for (int bco = 0; bco < 128; ++bco)
      {
        bco_data.bco = bco;
        bin = InttMon::BcoBin(bco_data);
        bin = bco_hist->GetBinContent(bin);  // reuse the index as the value in that bin
        if (bin > max) max = bin;
        hist_FB[i][fee]->SetBinContent(bco + 1, bin);  // + 1 is b/c the 0th bin is an underflow bin
      }
    }

    Pad_felixbcofphxbco_hist[i]->SetLogy();
    for (int fee = 0; fee < 14; ++fee)
    {
      hist_FB[i][fee]->SetLineColor(GetFeeColor(fee));
      hist_FB[i][fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
      if (fee)
      {
        hist_FB[i][fee]->Draw("same");
      }
      else
      {
        hist_FB[i][fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
        hist_FB[i][fee]->Draw();
      }
    }
  }

  return 0;
}

int InttMonDraw::DrawHitMap()
{
  OnlMonClient* cl = OnlMonClient::instance();

  TH1D* evt_hist = (TH1D*) cl->getHisto("INTTMON_0", "InttEvtHist");

  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"InttEvtHist\" from INTTMON_0" << std::endl;
    return 1;
  }

  if (!gROOT->FindObject("InttMonHitMap"))
  {
    MakeCanvas("InttMonHitMap");
  }
  TC[1]->Clear("D");
  TC[1]->SetEditable(true);
  disp_pad[0]->cd();

  TText disp_text(0.5, 0.5, "INTT_HitMap_disp_text");
  disp_text.SetTextAlign(22);
  disp_text.SetTextSize(0.25);
  std::ostringstream displaytext;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  displaytext << "Run " << cl->RunNumber() << ", Event: " << evt_hist->GetBinContent(1) << ", Time: " << ctime(&evttime);

  disp_text.DrawText(0.5, 0.5, displaytext.str().c_str());

  TText title_text(0.5, 0.75, "INTT_HitMap_title_text");
  title_text.SetTextAlign(22);
  title_text.SetTextSize(0.25);  // m_HitMap.disp_text_size
  title_text.DrawText(0.5, 0.75, "INTT_HitMap_title_text");

  // Set the title for the TText object
  title_text.SetTitle("INTT_HitMap");

  lgnd_pad[0]->cd();

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
  for (int c = 0; c < 3; ++c)
  {
    x0 = 0.5 - 0.16;  // m_HitMap.lgnd_box_width
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

    // Create and configure TText for legend
    TText lgnd_text(x0 + 1.5 * 0.16, y0, label.c_str());
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(0.12);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * 0.16, y0, label.c_str());

    x[0] = -1;
    x[1] = +1;
    x[2] = +1;
    x[3] = -1;
    y[0] = -1;
    y[1] = -1;
    y[2] = +1;
    y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * 0.16;
      x[i] += x0;

      y[i] *= 0.5 * 0.03;
      y[i] += y0;
    }

    TPolyLine box(4, x, y);
    box.SetFillColor(color);
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y);

    for (int i = 0; i < 8; ++i)
    {
      std::string name;

      Pad_hit_hist[i]->cd();
      hist[i]->Reset();

      name = "InttEvtHist";
      evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
      if (!evt_hist)
      {
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
                  << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
        continue;
      }

      name = "InttHitHist";
      TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
      if (!bco_hist)
      {
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
                  << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
        continue;
      }

      // Fill histogram
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
          if (bin < 0.0001)
          {
            bin = 0.4;  // Cold/Dead
          }
          else if (0.01 < bin)
          {
            bin = 3.0;  // Hot
          }
          else
          {
            bin = 1.7;  // Good
          }

          hist[i]->SetBinContent(
              hit_data.chp + 1,  // + 1 is b/c the 0th x bin is an underflow bin
              hit_data.fee + 1,  // + 1 is b/c the 0th y bin is an underflow bin
              bin);
        }
        hist[i]->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", 1));
      }

      hist[i]->Draw("COL");  // "COLZ" for a legend; no legend is preferrable here
    }
  }

  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);

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

#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::HitRates_s const
InttMonDraw::m_HitRates {
	.cnvs_width = 1280, .cnvs_height = 720, //
	.disp_frac = 0.1, //
	.disp_text_size = 0.25, //
	.warn_text_size = 0.2, .min_events = 50000, //
	.lower = 0.0, .upper = 0.02, //
	.name = "INTT_HitRates"
};

int
InttMonDraw::DrawHitRates (
	int icnvs
) {
	std::string name;

	// use gROOT to find TStyle
	name = Form("%s_style", m_HitRates.name.c_str());
	TStyle* style = new TStyle(name.c_str(), name.c_str());
	style->SetOptStat(0);
	//...

	style->cd();
	// gROOT->SetStyle(name.c_str());
	// gROOT->ForceStyle();

	name = Form("%s", m_HitRates.name.c_str());
	TC[icnvs] = new TCanvas (
		name.c_str(), name.c_str(),
		0, 0,
		m_HitRates.cnvs_width, m_HitRates.cnvs_height
	);
	gSystem->ProcessEvents(); // ...ROOT garbage collection?

	int iret = 0;
	iret += DrawHitRates_DispPad(icnvs);
	iret += DrawHitRates_SubPads(icnvs);

	TC[icnvs]->Update();
	TC[icnvs]->Show();
	TC[icnvs]->SetEditable(false);

	return iret;
}

int
InttMonDraw::DrawHitRates_DispPad (
	int icnvs
) {
  std::string name;

  name = Form("%s_disp_pad", m_HitRates.name.c_str());
  TPad* disp_pad = new TPad (
    name.c_str(), name.c_str(),
    0.0, 1.0 - m_HitRates.disp_frac,  // Southwest x, y
    1.0, 1.0                                 // Northeast x, y
  );
  DrawPad(TC[icnvs], disp_pad); // Floor of division will be the right icnvs

  name = "InttEvtHist";
  OnlMonClient* cl = OnlMonClient::instance();
  TH1D* evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", 0), name);
  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
    return 1;
  }

  std::time_t t = cl->EventTime("CURRENT");  // BOR, CURRENT, or EOR
  struct tm* ts = std::localtime(&t);
  name = Form(
      "Run: %08d, Events: %d, Date: %02d/%02d/%4d",
      cl->RunNumber(),
      (int) evt_hist->GetBinContent(1),
      ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900);
  TText* disp_text = new TText(0.5, 0.5, name.c_str());
  disp_text->SetTextAlign(22);
  disp_text->SetTextSize(m_HitRates.disp_text_size);
  disp_text->Draw();

  name = Form("%s", m_HitRates.name.c_str());
  TText* title_text = new TText(0.5, 0.75, name.c_str());
  title_text->SetTextAlign(22);
  title_text->SetTextSize(m_HitRates.disp_text_size);
  title_text->Draw();

  name = "  "; // Nothing if we have enough events
  if (evt_hist->GetBinContent(1) < m_HitRates.min_events) {
  	name = Form("Not enough events (min %0.E) to be statistically significant yet", m_HitRates.min_events);
  }
  TText* warn_text = new TText(0.5, 0.25, name.c_str());
  warn_text->SetName(name.c_str());
  warn_text->SetTextAlign(22);
  warn_text->SetTextSize(m_HitRates.warn_text_size);
  warn_text->SetTextColor(kRed);
  warn_text->Draw();

  return 0;

}

int
InttMonDraw::DrawHitRates_SubPads (
	int icnvs
) {
  std::string name;

  int iret = 0;
  double x_min = 0.0, x_max = 1.0;
  double y_min = 0.0, y_max = 1.0 - m_HitRates.disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_HitRates.name.c_str(), i);
    TPad* hist_pad = new TPad(
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
    );
    hist_pad->SetBottomMargin(0.15);
    hist_pad->SetLeftMargin(0.15);
    DrawPad(TC[icnvs], hist_pad);

    iret += DrawHitRates_SubPad(hist_pad, i);
  }

  return iret;
}

int
InttMonDraw::DrawHitRates_SubPad (
	TPad* prnt_pad,
	int i 
) {
	std::string name;

	if(!prnt_pad) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tnull TPad*" << std::endl;
		return 1;
	}
	CdPad(prnt_pad);

	// For now, just the histogram
	// Other niceties (manual axis labels/ticks, maybe gridlines)
	//   in the future (broken up into other methods)

	OnlMonClient* cl = OnlMonClient::instance();

	name = "InttEvtHist";
	TH1D* evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", 0), name);
	if (!evt_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
		return 1;
	}

	name = "InttHitHist";
	TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
	if (!bco_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
		return 1;
	}

	name = Form("%s_hist_%01d", m_HitRates.name.c_str(), i);
	TH1D* hist = new TH1D (
		name.c_str(), name.c_str(),
		112, m_HitRates.lower, m_HitRates.upper
	);
	hist->GetXaxis()->SetNdivisions(8, true);

	// Fill
	double bin;
	struct InttMon::HitData_s hit_data;
	for(hit_data.fee = 0; hit_data.fee < 14; ++hit_data.fee) {
		for(hit_data.chp = 0; hit_data.chp < 26; ++hit_data.chp) {
			bin = InttMon::HitBin(hit_data);         // Which bin has the data we want
			bin = bco_hist->GetBinContent((int)bin); // Reuse the index as the value in that bin
			bin /= evt_hist->GetBinContent(1);       // Normalize by number of events

			// Manually catch overflows and put them in the last displayed bin
			if(m_HitRates.upper <= bin) {
				bin = m_HitRates.upper - hist->GetXaxis()->GetBinWidth(1);
			}

			hist->Fill(bin);
		}
		hist->SetTitle(Form("intt%01d;Hits/Event (overflow is shown in last bin);Entries (One Hitrate per Chip)", i));
	}
	hist->Draw();

	return 0;
}

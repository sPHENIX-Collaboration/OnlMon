#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::HitRates_s const
InttMonDraw::m_HitRates {
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
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if(!style) {
  	style = new TStyle(name.c_str(), name.c_str());
  	style->SetOptStat(0);
  }
  //...
  
  style->cd();
  
  name = Form("%s", m_HitRates.name.c_str());
  TC[icnvs] = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if(!TC[icnvs])
  {
    // I'm almost certain this line is safe (and preventing a leak)
    // But I leave it out--better to leak than crash
    // delete TC[icnvs];
    TC[icnvs] = new TCanvas (
    	name.c_str(), name.c_str(), //
    	0, 0, //
    	m_cnvs_width, m_cnvs_height
    );
  }
  TC[icnvs]->cd();
  gSystem->ProcessEvents(); // ...ROOT garbage collection?
  
  int iret = 0;
  iret += Draw_DispPad(icnvs, m_HitRates.name);
  iret += DrawHitRates_SubPads(icnvs);
  
  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);
  
  return iret;
}

int
InttMonDraw::DrawHitRates_SubPads (
	int icnvs
) {
  std::string name;

  int iret = 1;
  double x_min = 0.0, x_max = 1.0;
  double y_min = 0.0, y_max = 1.0 - m_disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_HitRates.name.c_str(), i);
    TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
    if(!hist_pad) {
      hist_pad = new TPad (
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
      );
      hist_pad->SetBottomMargin(0.15);
      hist_pad->SetLeftMargin(0.15);
      DrawPad(TC[icnvs], hist_pad);
    }
	hist_pad->cd();
    iret = DrawHitRates_SubPad(i) && iret;
  }

  return iret;

}

int
InttMonDraw::DrawHitRates_SubPad (
	int i 
) {
	// For now, just the histogram
	// Other niceties (manual axis labels/ticks, maybe gridlines)
	//   in the future (broken up into other methods)

	std::string name;

	name = Form("%s_hist_%01d", m_HitRates.name.c_str(), i);
	TH1D* hist = dynamic_cast<TH1D*>(gROOT->FindObject(name.c_str()));
	if(!hist) {
		hist = new TH1D (
			name.c_str(), name.c_str(),
			112, m_HitRates.lower, m_HitRates.upper
		);
		hist->GetXaxis()->SetNdivisions(8, true);
		hist->Draw();
	}
	hist->Reset();

	// Fill
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

	return 0;
}

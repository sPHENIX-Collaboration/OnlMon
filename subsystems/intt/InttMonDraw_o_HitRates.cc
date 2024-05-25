#include "InttMon.h"
#include "InttMonDraw.h"

int
InttMonDraw::Draw_HitRates (
	int icnvs
) {
  // Set global values we use to what they should be at beginning of each call
  m_name = "INTT_HitRates";
  m_disp_frac = 0.2;
  m_lgnd_frac = 0.2;

  std::string name;
 
  // gROOT can find Styles
  // ensures unique allocations
  name = Form("%s_style", m_name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if(!style) {
  	style = new TStyle(name.c_str(), name.c_str());
  	style->SetOptStat(0);
    //...
  }
  style->cd();

  MakeCanvas_Generic(icnvs); // Makes TC[icnvs] and all its subpads, or does nothing
  TC[icnvs]->SetEditable(true);

  DrawDispPad_Generic(icnvs);
  // DrawLgndPad_HitRates(icnvs);
  // This option doesn't need a legend but I leave it here to emphasize the idiom

  int iret = 1;
  for(int i = 0; i < 8; ++i) {
	  // If any subdraw succeeds, say the entire call succeeds
	  iret = DrawSubPad_HitRates(icnvs, i) && iret;
  }
  
  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);
  
  return iret;
}

int
InttMonDraw::DrawSubPad_HitRates (
	int icnvs,
	int i 
) {
	double m_lower = 0.0;
	double m_upper = 0.02;

	std::string name;

	// Validate member histos
	name = Form("%s_hist_%01d", m_name.c_str(), i);
	if(!m_hist_hitrates[i]) {
		m_hist_hitrates[i] = new TH1D (
			name.c_str(), name.c_str(), //
			112, m_lower, m_upper       //
		);
		m_hist_hitrates[i]->GetXaxis()->SetNdivisions(8, true);
		m_hist_hitrates[i]->SetTitle(Form("intt%01d;Hits/Event (overflow is shown in last bin);Entries (One Hitrate per Chip)", i));
	}
	m_hist_hitrates[i]->Reset();
	CdPad(TP[icnvs][i]);
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


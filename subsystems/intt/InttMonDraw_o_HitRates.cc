#include "InttMon.h"
#include "InttMonDraw.h"

int
InttMonDraw::Draw_HitRates (
) {
  // Set member variables we use to what they should be at beginning of each call
  m_name = "INTT_HitRates";
  m_lgnd_frac = 0.0;

  std::string name;

  // This uses m_name for name mangling and it is set to the right value
  MakeCanvas_Generic(k_hitrates);
  TC[k_hitrates]->SetEditable(true);

  DrawDispPad_Generic(k_hitrates);
  // DrawLgndPad_HitRates(k_hitrates);
  // This option doesn't need a legend but I leave it here to emphasize the idiom

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
		std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tNote: TH1D \"" << name << "\" allocated" << std::endl;
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


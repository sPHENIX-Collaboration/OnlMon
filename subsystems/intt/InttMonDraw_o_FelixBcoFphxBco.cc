#include "InttMon.h"
#include "InttMonDraw.h"

int
InttMonDraw::Draw_FelixBcoFphxBco(
    int icnvs)
{
  std::string name;

  // Set global values we use to what they should be at the beginning of each call
  m_name = "INTT_FelixBco_FphxBco_Diff";

  m_disp_frac = 0.1;
  m_disp_text_size = 0.25;

  m_lgnd_frac = 0.15;
  m_lgnd_text_size = 0.08;
  m_lgnd_box_width = 0.16;
  m_lgnd_box_height = 0.01;

  // use gROOT to find TStyle
  name = Form("%s_style", m_name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if (!style)
  {
    style = new TStyle(name.c_str(), name.c_str());
    style->SetOptStat(0);
    //...
  }
  style->cd();

  MakeCanvas_Generic(icnvs); // Makes TC[icnvs] and all its subpads, or does nothing
  TC[icnvs]->SetEditable(true);

  DrawDispPad_Generic(icnvs);
  DrawLgndPad_FelixBcoFphxBco(icnvs);

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

int InttMonDraw::DrawLgndPad_FelixBcoFphxBco(
	int icnvs
) {
  std::string name;

  TP[icnvs][k_disp_pad]->Clear();
  CdPad(TP[icnvs][k_disp_pad]);

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - m_lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text(
        x0 + 1.5 * m_lgnd_box_width,
        y0,
        Form("FCh %2d", fee));
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(m_lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.Draw();

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * m_lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * m_lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box(4, x, y);
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.Draw("f");
  }

  return 0;
}

int InttMonDraw::DrawSubPad_FelixBcoFphxBco(
	int icnvs,
    int i)
{
  std::string name;

  // Validate member histos
  struct InttMon::BcoData_s bco_data;
  for (bco_data.fee = 0; bco_data.fee < 14; ++bco_data.fee)
  {
    name = Form("%s_hist_%01d_%02d", m_name.c_str(), i, bco_data.fee);
    if (!m_hist_bco[i][bco_data.fee])
    {
      m_hist_bco[i][bco_data.fee] = new TH1D(
          name.c_str(), name.c_str(), //
          128, 0, 127                 //
      );
      m_hist_bco[i][bco_data.fee]->GetXaxis()->SetNdivisions(16);  //, true);
      m_hist_bco[i][bco_data.fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
      m_hist_bco[i][bco_data.fee]->SetLineColor(GetFeeColor(bco_data.fee));
    }
    m_hist_bco[i][bco_data.fee]->Reset();
	TP[icnvs][i]->SetLogy();
	CdPad(TP[icnvs][i]);
    m_hist_bco[i][bco_data.fee]->Draw();
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttBcoHist";
  TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
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
      m_hist_bco[i][bco_data.fee]->SetBinContent(bco_data.bco + 1, bin);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  // Noramlize ranges
  for (bco_data.fee = 0; bco_data.fee < 14; ++bco_data.fee)
  {
    m_hist_bco[i][bco_data.fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
  }

  return 0;
}

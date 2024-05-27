#include "InttMon.h"
#include "InttMonDraw.h"

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

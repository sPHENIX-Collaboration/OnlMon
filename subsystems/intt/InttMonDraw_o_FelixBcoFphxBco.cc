#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::FelixBcoFphxBco_s const
InttMonDraw::m_FelixBcoFphxBco{
		.lgnd_frac = 0.15, //
		.lgnd_box_width = 0.16, .lgnd_box_height = 0.01, .lgnd_text_size = 0.08, //
		.name = "INTT_FelixBco_FphxBco_Diff"
};

int InttMonDraw::DrawFelixBcoFphxBco(
    int icnvs)
{
  std::string name;

  name = Form("%s_style", m_FelixBcoFphxBco.name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if(!style)
  {
 	style = new TStyle(name.c_str(), name.c_str());
    style->SetOptStat(0);
    //...
  }
  style->cd();

  name = Form("%s", m_FelixBcoFphxBco.name.c_str());
  TC[icnvs] = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if(!TC[icnvs])
  {
	// I'm almost certain this line is safe (and preventing a leak)
	// But I leave it out--better to leak than crash
    delete TC[icnvs];
    TC[icnvs] = new TCanvas (
      name.c_str(), name.c_str(), //
	  m_cnvs_width, m_cnvs_height //
    );
  }
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += Draw_DispPad(icnvs, m_FelixBcoFphxBco.name.c_str());
  iret += DrawFelixBcoFphxBco_LgndPad(icnvs);
  iret += DrawFelixBcoFphxBco_SubPads(icnvs);

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawFelixBcoFphxBco_LgndPad(
	int icnvs
) {
  std::string name;

  name = Form("%s_lgnd_pad", m_FelixBcoFphxBco.name.c_str());
  TPad* lgnd_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if(lgnd_pad) return 0;

  lgnd_pad = new TPad (
    name.c_str(), name.c_str(),
    1.0 - m_FelixBcoFphxBco.lgnd_frac, 0.0, // Southwest x, y
    1.0, 1.0 - m_disp_frac                  // Northeast x, y
  );
  DrawPad(TC[icnvs], lgnd_pad);

  double x0, y0, x[4], y[4];
  double w[4] = {-1, +1, +1, -1};
  double h[4] = {-1, -1, +1, +1};
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - m_FelixBcoFphxBco.lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText* lgnd_text = new TText (
      x0 + 1.5 * m_FelixBcoFphxBco.lgnd_box_width,
      y0,
      Form("FCh %2d", fee)
	);
    lgnd_text->SetTextAlign(12);
    lgnd_text->SetTextSize(m_FelixBcoFphxBco.lgnd_text_size);
    lgnd_text->SetTextColor(kBlack);
    lgnd_text->Draw();

    for (int i = 0; i < 4; ++i)
    {
      x[i] = w[i];
      x[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_width;
      x[i] += x0;

      y[i] = h[i];
      y[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine* box = new TPolyLine(4, x, y);
    box->SetFillColor(GetFeeColor(fee));
    box->SetLineColor(kBlack);
    box->SetLineWidth(1);
    box->Draw("f");
  }

  return 0;
}

int InttMonDraw::DrawFelixBcoFphxBco_SubPads(
	int icnvs
) {
  std::string name;

  int iret = 1;
  double x_min = 0.0, x_max = 1.0 - m_FelixBcoFphxBco.lgnd_frac;
  double y_min = 0.0, y_max = 1.0 - m_disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_FelixBcoFphxBco.name.c_str(), i);
    TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
    if(!hist_pad) {
      hist_pad = new TPad (
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
      );
      hist_pad->SetBottomMargin(0.15);
      hist_pad->SetLeftMargin(0.15);
      hist_pad->SetLogy();
      DrawPad(TC[icnvs], hist_pad);
    }

	// If even one succeeds, return 0
	// iret must be second b/c of early return of &&
    CdPad(hist_pad);
    iret = DrawFelixBcoFphxBco_SubPad(i) && iret;
  }

  return iret;
}

int InttMonDraw::DrawFelixBcoFphxBco_SubPad(
    int i)
{
  // For now, just the histogram
  // Other niceties (manual axis labels/ticks, maybe gridlines)
  //   in the future (broken up into other methods)

  std::string name;

  int bin, max = 0;
  struct InttMon::BcoData_s bco_data;
  TH1D* hist[14];
  for (int fee = 0; fee < 14; ++fee)
  {
    name = Form("%s_hist_%01d_%02d", m_FelixBcoFphxBco.name.c_str(), i, fee);
    hist[fee] = dynamic_cast<TH1D*>(gROOT->FindObject(name.c_str()));
    if (hist[fee])
	{
      continue;
	}
    hist[fee] = new TH1D (
      name.c_str(), name.c_str(),
      128,
      0, 128);
    hist[fee]->GetXaxis()->SetNdivisions(16, true);
    hist[fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
    hist[fee]->SetLineColor(GetFeeColor(fee));

    // If using TH1::Draw (as opposed to TH1::DrawCopy)
    // This should be called ONCE per histogram, per pad
    // DrawCopy doesn't update until the whole canvas is redrawn
    if (fee)
    {
      hist[fee]->Draw("same");
    }
    else
    {
      hist[fee]->Draw();
    }
    hist[fee]->Reset();
  }

  // Fill
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttBcoHist";
  TH1D* bco_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    bco_data.fee = fee;
    for (int bco = 0; bco < 128; ++bco)
    {
      bco_data.bco = bco;
      bin = InttMon::BcoBin(bco_data);
      bin = bco_hist->GetBinContent(bin);  // reuse the index as the value in that bin
      if (bin > max) max = bin;
      hist[fee]->SetBinContent(bco + 1, bin);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    hist[fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
  }

  return 0;
}

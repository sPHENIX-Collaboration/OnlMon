#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::FelixBcoFphxBco_s const
InttMonDraw::m_FelixBcoFphxBco{
        .cnvs_width = 1280, .cnvs_height = 720, //
		.disp_frac = 0.1, .lgnd_frac = 0.15, //
		.disp_text_size = 0.25, //
		.warn_text_size = 0.2, .min_events = 50000, //
		.lgnd_box_width = 0.16, .lgnd_box_height = 0.01, .lgnd_text_size = 0.08, //
		.name = "INTT_FelixBco_FphxBco_Diff"
};

int InttMonDraw::DrawFelixBcoFphxBco(
    int icnvs)
{
  std::string name;

  TStyle* style = new TStyle(name.c_str(), name.c_str());
  style->SetOptStat(0);
  //...

  style->cd();
  // gROOT->SetStyle(name.c_str());
  // gROOT->ForceStyle();

  name = Form("%s", m_FelixBcoFphxBco.name.c_str());
  TC[icnvs] = new TCanvas(
    name.c_str(), name.c_str(),
    0, 0,
    m_FelixBcoFphxBco.cnvs_width, m_FelixBcoFphxBco.cnvs_height
  );
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += DrawFelixBcoFphxBco_DispPad(icnvs);
  iret += DrawFelixBcoFphxBco_LgndPad(icnvs);
  iret += DrawFelixBcoFphxBco_SubPads(icnvs);

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawFelixBcoFphxBco_DispPad (
	int icnvs
) {
  std::string name;

  name = Form("%s_disp_pad", m_FelixBcoFphxBco.name.c_str());
  TPad* disp_pad = new TPad (
    name.c_str(), name.c_str(),
    0.0, 1.0 - m_FelixBcoFphxBco.disp_frac,  // Southwest x, y
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
  disp_text->SetTextSize(m_FelixBcoFphxBco.disp_text_size);
  disp_text->Draw();

  name = Form("%s", m_FelixBcoFphxBco.name.c_str());
  TText* title_text = new TText(0.5, 0.75, name.c_str());
  title_text->SetTextAlign(22);
  title_text->SetTextSize(m_FelixBcoFphxBco.disp_text_size);
  title_text->Draw();

  name = "  "; // Nothing if we have enough events
  if (evt_hist->GetBinContent(1) < m_FelixBcoFphxBco.min_events) {
  	name = Form("Not enough events (min %0.E) to be statistically significant yet", m_FelixBcoFphxBco.min_events);
  }
  TText* warn_text = new TText(0.5, 0.25, name.c_str());
  warn_text->SetName(name.c_str());
  warn_text->SetTextAlign(22);
  warn_text->SetTextSize(m_FelixBcoFphxBco.warn_text_size);
  warn_text->SetTextColor(kRed);
  warn_text->Draw();

  return 0;
}

int InttMonDraw::DrawFelixBcoFphxBco_LgndPad(
	int icnvs
) {
  std::string name;

  TPad* lgnd_pad = new TPad (
      name.c_str(), name.c_str(),
      1.0 - m_FelixBcoFphxBco.lgnd_frac, 0.0,  // Southwest x, y
      1.0, 1.0 - m_FelixBcoFphxBco.disp_frac   // Northeast x, y
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
        Form("FCh %2d", fee));
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

  int iret = 0;
  double x_min = 0.0, x_max = 1.0 - m_FelixBcoFphxBco.lgnd_frac;
  double y_min = 0.0, y_max = 1.0 - m_FelixBcoFphxBco.disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_FelixBcoFphxBco.name.c_str(), i);
    TPad* hist_pad = new TPad(
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
    );
    hist_pad->SetBottomMargin(0.15);
    hist_pad->SetLeftMargin(0.15);
    DrawPad(TC[icnvs], hist_pad);

    iret += DrawFelixBcoFphxBco_SubPad(hist_pad, i);
  }

  return iret;
}

int InttMonDraw::DrawFelixBcoFphxBco_SubPad(
	TPad* prnt_pad,
    int i)
{
  std::string name;

  if (!prnt_pad)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tnull TPad*" << std::endl;
    return 1;
  }
  CdPad(prnt_pad);

  // For now, just the histogram
  // Other niceties (manual axis labels/ticks, maybe gridlines)
  //   in the future (broken up into other methods)

  name = "InttBcoHist";
  OnlMonClient* cl = OnlMonClient::instance();
  TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  int max = 0;
  int bin;
  struct InttMon::BcoData_s bco_data;
  TH1D* hist[14];
  for (int fee = 0; fee < 14; ++fee)
  {
    name = Form("%s_hist_%01d_%02d", m_FelixBcoFphxBco.name.c_str(), i, fee);
    hist[fee] = dynamic_cast<TH1D*>(gROOT->FindObject(name.c_str()));
    if (!hist[fee])
    {
      hist[fee] = new TH1D(
          name.c_str(), name.c_str(),
          128,
          0, 127);
      hist[fee]->GetXaxis()->SetNdivisions(16);  //, true);
    }
    hist[fee]->Reset();

    // Fill
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

  prnt_pad->SetLogy();
  for (int fee = 0; fee < 14; ++fee)
  {
    hist[fee]->SetLineColor(GetFeeColor(fee));
    hist[fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
    if (fee)
    {
      hist[fee]->Draw("same");
    }
    else
    {
      hist[fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
      hist[fee]->Draw();
    }
  }

  return 0;
}

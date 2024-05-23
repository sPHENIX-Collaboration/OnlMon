#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::HitMap_s const
InttMonDraw::m_HitMap {
	.lgnd_frac = 0.1, //
	.lgnd_box_width = 0.16, .lgnd_box_height = 0.01, .lgnd_text_size = 0.08, //
	.lower = 10e-4, .upper = 10e-2, //
	.name = "INTT_HitMap"
};

int InttMonDraw::DrawHitMap (
    int icnvs //
) {
  std::string name;

  name = Form("%s_style", m_HitMap.name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if(!style) {
    style= new TStyle(name.c_str(), name.c_str());
    style->SetOptStat(0);
    //...
  
    Int_t palette[3] = {kBlue, kGreen, kRed};
    style->SetPalette(3, palette);
  }
  style->cd();
  gROOT->SetStyle(name.c_str());
  gROOT->ForceStyle();

  // use member TCanvas instead of (purely) gROOT here
  name = Form("%s", m_HitMap.name.c_str());
  TC[icnvs] = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if(!TC[icnvs])
  {
	// I'm almost certain this line is safe (and preventing a leak)
	// But I leave it out--better to leak than crash
    // delete TC[icnvs];
    TC[icnvs] = new TCanvas(
	  name.c_str(), name.c_str(), //
	  m_cnvs_width, m_cnvs_height
	);
  }
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += Draw_DispPad(icnvs, m_HitMap.name);
  iret += DrawHitMap_LgndPad(icnvs);
  iret += DrawHitMap_SubPads(icnvs);

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHitMap_LgndPad(
	int icnvs
) {
  std::string name;

  name = Form("%s_lgnd_pad", m_HitMap.name.c_str());
  TPad* lgnd_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if(lgnd_pad) return 0;

  lgnd_pad = new TPad (
    name.c_str(), name.c_str(),
    1.0 - m_HitMap.lgnd_frac, 0.5 - m_disp_frac,  // Southwest x, y
    1.0, 0.5 + m_disp_frac                        // Northeast x, y
  );
  DrawPad(TC[icnvs], lgnd_pad);

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
  double w[4] = {-1, +1, +1, -1};
  double h[4] = {-1, -1, +1, +1};
  for (int c = 0; c < 3; ++c)
  {
    x0 = 0.5 - m_HitMap.lgnd_box_width;
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

    TText* lgnd_text = new TText (
      x0 + 1.5 * m_HitMap.lgnd_box_width,
      y0,
      label.c_str()
	);
    lgnd_text->SetTextAlign(12);
    lgnd_text->SetTextSize(m_HitMap.lgnd_text_size);
    lgnd_text->SetTextColor(kBlack);
    lgnd_text->Draw();

    for (int i = 0; i < 4; ++i)
    {
      x[i] = w[i];
      x[i] *= 0.5 * m_HitMap.lgnd_box_width;
      x[i] += x0;

      y[i] = h[i];
      y[i] *= 0.5 * m_HitMap.lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine* box = new TPolyLine(4, x, y);
    box->SetFillColor(color);
    box->SetLineColor(kBlack);
    box->SetLineWidth(1);
    box->Draw("f");
  }

  return 0;
}

int InttMonDraw::DrawHitMap_SubPads(
	int icnvs
) {
  std::string name;

  int iret = 0;
  double x_min = 0.0, x_max = 1.0 - m_HitMap.lgnd_frac;
  double y_min = 0.0, y_max = 1.0 - m_disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_HitMap.name.c_str(), i);
    TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(!hist_pad) {
      hist_pad = new TPad (
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
      );
      hist_pad->SetRightMargin(0.2);
      DrawPad(TC[icnvs], hist_pad);
    }

	// If even one succeeds, return 0
	// iret must be second b/c of early return of &&
	CdPad(hist_pad);
    iret = DrawHitMap_SubPad(i) && iret;
  }

  return iret;
}

int InttMonDraw::DrawHitMap_SubPad(
    int i)
{
  // For now, just the histogram
  // Other niceties (manual axis labels/ticks, maybe gridlines)
  //   in the future (broken up into other methods)

  std::string name;

  name = Form("%s_hist_%01d", m_HitMap.name.c_str(), i);
  TH2D* hist = dynamic_cast<TH2D*>(gROOT->FindObject(name.c_str()));
  if (!hist) {
    hist = new TH2D(
        name.c_str(), name.c_str(),
        26, 0, 26,  // 26, -0.5, 25.5,
        14, 0, 14   // 14, -0.5, 13.5
    );
    hist->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));
    hist->GetXaxis()->SetNdivisions(13, true);
    hist->GetYaxis()->SetNdivisions(14, true);
    hist->GetZaxis()->SetRangeUser(0, 4);

    Double_t levels[5] = {0, 1, 2, 3, 4};
    hist->SetContour(5, levels);
    hist->Draw("COLZ");  // "COLZ" for a legend; no legend is preferrable here
  }
  hist->Reset();

  // Fill
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
	  if (!bin)
	  {
        bin = 0.0; // Empty bins should be blank
	  }
	  else if (bin < m_HitMap.lower)
      {
        bin = 0.5; // Cold
      }
      else if (m_HitMap.upper < bin)
      {
        bin = 4.0; // Hot
      }
      else
      {
        bin = 2.2; // Good
      }

      hist->SetBinContent(
          hit_data.chp + 1,  // + 1 is b/c the 0th x bin is an underflow bin
          hit_data.fee + 1,  // + 1 is b/c the 0th y bin is an underflow bin
          bin);
    }
  }

  return 0;
}

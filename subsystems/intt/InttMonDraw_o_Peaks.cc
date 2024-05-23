#include "InttMon.h"
#include "InttMonDraw.h"

#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TMarker.h>

InttMonDraw::Peaks_s const
    InttMonDraw::m_Peaks{
        .peak_frac = 0.035,  // between 0.01 and 0.1 is reasonable
        .max_width = 3,      // The max "width" of a peak before we denote a problem
        .name = "INTT_Peaks" //
};

int InttMonDraw::DrawPeaks(
    int icnvs)
{
  std::string name;

  // use gROOT to find TStyle
  name = Form("%s_style", m_Peaks.name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if(!style) {
	  style = new TStyle(name.c_str(), name.c_str());
	  style->SetOptStat(0);
	  style->SetMarkerStyle(8); // Circle
	  style->SetMarkerSize(0.5);
	  style->SetMarkerColor(kBlack);
  }
  //...

  style->cd();

  name = Form("%s", m_Peaks.name.c_str());
  TC[icnvs] = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if(!TC[icnvs])
  {
	// I'm almost certain this line is safe (and preventing a leak)
	// But I leave it out--better to leak than crash
    // delete TC[icnvs];
    TC[icnvs] = new TCanvas(
      name.c_str(), name.c_str(), //
      m_cnvs_width, m_cnvs_height //
	);
  }
  TC[icnvs]->cd();
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += Draw_DispPad(icnvs, m_Peaks.name);
  iret += DrawPeaks_SubPads(icnvs);

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawPeaks_SubPads(
	int icnvs
) {
  std::string name;

  int iret = 1;
  double x_min = 0.0, x_max = 1.0;
  double y_min = 0.0, y_max = 1.0 - m_disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_Peaks.name.c_str(), i);
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

	// If even one succeeds, return 0
	// iret must be second b/c of early return of &&
	hist_pad->Clear("D");
	CdPad(hist_pad);
    iret = DrawPeaks_SubPad(i) && iret;
  }

  return iret;
}

int InttMonDraw::DrawPeaks_SubPad(
    int i)
{
  std::string name;
  double x[14] = {0.0}, x_err[14] = {0.0};
  double y[14] = {0.0}, y_err[14] = {0.0};

  name = Form("%s_black_graph_%01d", m_Peaks.name.c_str(), i);
  TGraphErrors* black_graph = dynamic_cast<TGraphErrors*>(gROOT->FindObject(name.c_str()));
  if(!black_graph)
  {
    black_graph = new TGraphErrors(14, x, y, x_err, y_err);
    black_graph->SetName(name.c_str());
	black_graph->SetMarkerColor(kBlack);
  }

  name = Form("%s_red_graph_%01d", m_Peaks.name.c_str(), i);
  TGraphErrors* red_graph = dynamic_cast<TGraphErrors*>(gROOT->FindObject(name.c_str()));
  if(!red_graph)
  {
    red_graph = new TGraphErrors(14, x, y, x_err, y_err);
    red_graph->SetName(name.c_str());
	red_graph->SetMarkerColor(kRed);
  }

  name = Form("%s_multi_graph_%01d", m_Peaks.name.c_str(), i);
  TMultiGraph* multi_graph = dynamic_cast<TMultiGraph*>(gROOT->FindObject(name.c_str()));
  if(!multi_graph)
  {
    multi_graph = new TMultiGraph();
	multi_graph->SetName(name.c_str());
    multi_graph->SetTitle(Form("BCO Peaks for intt%01d;Felix Channel;Felix BCO - FPHX BCO", i));
	multi_graph->Add(black_graph);
	multi_graph->Add(red_graph);
    multi_graph->Draw("AP");
  }

  if(DrawPeaks_GetFeePeakAndWidth(i, x, y, y_err))
  {
	  return 1;
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    if(y_err[fee] < m_Peaks.max_width)
	{
      black_graph->SetPoint(fee, x[fee], y[fee]);
      black_graph->SetPointError(fee, x_err[fee], y_err[fee]);

      red_graph->SetPoint(fee, -1.0, -1.0);
      red_graph->SetPointError(fee, 0.0, 0.0);
    }
	else
	{
      black_graph->SetPoint(fee, -1.0, -1.0);
      black_graph->SetPointError(fee, 0.0, 0.0);

      red_graph->SetPoint(fee, x[fee], y[fee]);
      red_graph->SetPointError(fee, x_err[fee], y_err[fee]);
	}
  }

  // Need to reset the axes after drawing points
  multi_graph->GetXaxis()->SetRangeUser(-0.5, 13.5);
  multi_graph->GetXaxis()->SetNdivisions(14, true);
  multi_graph->GetYaxis()->SetRangeUser(-0.5, 127.5);
  multi_graph->GetYaxis()->SetNdivisions(16, true);

  return 0;
}

int InttMonDraw::DrawPeaks_GetFeePeakAndWidth(
    int i,
    double* fees,  // double[14]
    double* peak,  // double[14]
    double* width  // double[14]
)
{
  std::string name;

  name = "InttBcoHist";
  OnlMonClient* cl = OnlMonClient::instance();
  TH1D* bco_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  struct InttMon::BcoData_s bco_data;
  for (int fee = 0; fee < 14; ++fee)
  {
    bco_data.fee = fee;

    fees[fee] = fee;
    peak[fee] = 0;
    width[fee] = -0.5;  // The peak itself is counted later on--this cancels it

    // Loop over bco values and identify the peak
    double bin, max = -1;
    for (int bco = 0; bco < 128; ++bco)
    {
      bco_data.bco = bco;
      bin = InttMon::BcoBin(bco_data);
      bin = bco_hist->GetBinContent(bin);  // reuse the index as the value in that bin

      if (bin < max) continue;
      peak[fee] = bco;
      max = bin;
    }
    // max is now the max number of entries in the bco peak
    // peak[fee] is now the location of the peak

    // Multiple simple ways to define a meaningful "width"
    // for now count how many bins have some fracion as many entries as the peak
    // doesn't matter where these bins are, can do something different later
    max *= m_Peaks.peak_frac;
    for (int bco = 0; bco < 128; ++bco)
    {
      bco_data.bco = bco;
      bin = InttMon::BcoBin(bco_data);
      bin = bco_hist->GetBinContent(bin);  // reuse the index as the value in that bin

      if (bin < max) continue;
      width[fee] += 0.5;
    }
  }

  return 0;
}

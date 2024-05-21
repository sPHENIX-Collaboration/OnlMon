#include "InttMon.h"
#include "InttMonDraw.h"

#include <TGraphErrors.h>
#include <TMarker.h>

InttMonDraw::Peaks_s const
    InttMonDraw::m_Peaks{
        .cnvs_width = 1280, .cnvs_height = 720, .disp_frac = 0.1, .disp_text_size = 0.25,
        .frac = 0.035,   // between 0.01 and 0.1 is reasonable
        .max_width = 3,  // The max "width" of a peak before we denote a problem
        .name = "INTT_Peaks"};

int InttMonDraw::DrawPeaks(
    int icnvs)
{
  std::string name;

  // use gROOT to find TStyle
  name = Form("%s_style", m_Peaks.name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if (!style)
  {
    style = new TStyle(name.c_str(), name.c_str());
    style->SetOptStat(0);
    style->SetMarkerStyle(8);  // Circle
    style->SetMarkerSize(0.5);
    style->SetMarkerColor(kBlack);
    //...
  }
  style->cd();
  gROOT->SetStyle(name.c_str());
  gROOT->ForceStyle();

  // Only allocate if gROOT doesn't find it
  name = Form("%s", m_Peaks.name.c_str());
  if (!dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str())))
  {
    delete TC[icnvs];
    TC[icnvs] = new TCanvas(
        name.c_str(), name.c_str(),
        0, 0,
        m_Peaks.cnvs_width, m_Peaks.cnvs_height);
  }
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += DrawPeaks_DispPad();
  iret += DrawPeaks_SubPads();

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawPeaks_DispPad()
{
  std::string name;

  // use gROOT to find parent TPad (TCanvas)
  name = Form("%s", m_Peaks.name.c_str());
  TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if (!cnvs)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }

  // find or make this this pad
  name = Form("%s_disp_pad", m_Peaks.name.c_str());
  TPad* disp_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if (!disp_pad)
  {
    disp_pad = new TPad(
        name.c_str(), name.c_str(),
        0.0, 1.0 - m_Peaks.disp_frac,  // Southwest x, y
        1.0, 1.0                       // Northeast x, y
    );
    DrawPad(cnvs, disp_pad);
  }
  CdPad(disp_pad);

  name = Form("%s_disp_text", m_Peaks.name.c_str());
  TText* disp_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if (!disp_text)
  {
    disp_text = new TText(0.5, 0.5, name.c_str());
    disp_text->SetName(name.c_str());
    disp_text->SetTextAlign(22);
    disp_text->SetTextSize(m_Peaks.disp_text_size);
    disp_text->Draw();
  }

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
  disp_text->SetTitle(name.c_str());

  name = Form("%s_title_text", m_Peaks.name.c_str());
  TText* title_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if (title_text) return 0;  // Early return since the title text is unchanging, and this means we've drawn it

  title_text = new TText(0.5, 0.75, name.c_str());
  title_text->SetName(name.c_str());
  title_text->SetTextAlign(22);
  title_text->SetTextSize(m_Peaks.disp_text_size);
  title_text->Draw();

  name = Form("%s", m_Peaks.name.c_str());
  title_text->SetTitle(name.c_str());

  return 0;
}

int InttMonDraw::DrawPeaks_SubPads()
{
  std::string name;

  // use gROOT to find parent TPad (TCanvas)
  name = Form("%s", m_Peaks.name.c_str());
  TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if (!cnvs)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }

  double x_min = 0.0, x_max = 1.0;
  double y_min = 0.0, y_max = 1.0 - m_Peaks.disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_Peaks.name.c_str(), i);
    TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
    if (hist_pad) continue;

    hist_pad = new TPad(
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
    );
    hist_pad->SetBottomMargin(0.15);
    hist_pad->SetLeftMargin(0.15);
    DrawPad(cnvs, hist_pad);
  }

  int iret = 0;
  for (int i = 0; i < 8; ++i)
  {
    iret += DrawPeaks_SubPad(i);
  }

  return iret;
}

int InttMonDraw::DrawPeaks_SubPad(
    int i)
{
  std::string name;

  name = Form("%s_hist_pad_%01d", m_Peaks.name.c_str(), i);
  TPad* prnt_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if (!prnt_pad)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }
  CdPad(prnt_pad);

  double x[14], y[14], x_err[14] = {0.0}, y_err[14];
  if (DrawPeaks_GetFeePeakAndWidth(i, x, y, y_err))
  {
    return 1;
  }

  name = Form("%s_graph_%01d", m_Peaks.name.c_str(), i);
  TGraphErrors* graph = dynamic_cast<TGraphErrors*>(gROOT->FindObject(name.c_str()));
  delete graph;
  graph = new TGraphErrors(14, x, y, x_err, y_err);
  graph->SetName(name.c_str());
  graph->SetTitle(Form("BCO Peaks for intt%01d;Felix Channel;Felix BCO - FPHX BCO", i));
  graph->GetXaxis()->SetRangeUser(-0.5, 13.5);
  graph->GetXaxis()->SetNdivisions(14, true);
  graph->GetYaxis()->SetRangeUser(0.0, 127.0);
  graph->GetYaxis()->SetNdivisions(14, true);
  graph->Draw("AP");

  // Go back over and add red markers
  // for FEEs with peaks that are too "wide"
  for (int fee = 0; fee < 14; ++fee)
  {
    if (y_err[fee] < m_Peaks.max_width) continue;

    TMarker* m = new TMarker();
    m->SetX(x[fee]);
    m->SetY(y[fee]);
    m->SetMarkerColor(kRed);
    m->Draw();
  }

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
    max *= m_Peaks.frac;
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

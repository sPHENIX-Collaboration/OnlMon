#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::HitMap_s const
    InttMonDraw::m_HitMap{
        .cnvs_width = 1280, .cnvs_height = 720, .disp_frac = 0.1, .lgnd_frac = 0.1, .disp_text_size = 0.25, .lgnd_box_width = 0.16, .lgnd_box_height = 0.03, .lgnd_text_size = 0.12, .lower = 10e-4, .upper = 10e-2, .name = "INTT_HitMap"};

int InttMonDraw::DrawHitMap(
    int icnvs)
{
  std::string name;

  // use gROOT to find TStyle
  name = Form("%s_style", m_HitMap.name.c_str());
  TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
  if (!style)
  {
    style = new TStyle(name.c_str(), name.c_str());
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
  if (!dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str())))
  {  // Only allocate if gROOT doesn't find it
    // If ROOT freed the memory when a user closed the TCanvas,
    // This next line would throw a double free() and cause it to crash
    // This next line does not cause it to crash
    delete TC[icnvs];
    TC[icnvs] = new TCanvas(
        name.c_str(), name.c_str(),
        0, 0,
        m_HitMap.cnvs_width, m_HitMap.cnvs_height);
  }
  gSystem->ProcessEvents();  // ...ROOT garbage collection?

  int iret = 0;
  iret += DrawHitMap_DispPad();
  iret += DrawHitMap_LgndPad();
  iret += DrawHitMap_SubPads();

  TC[icnvs]->Update();
  TC[icnvs]->Show();
  TC[icnvs]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHitMap_DispPad()
{
  std::string name;

  // use gROOT to find parent TPad (TCanvas)
  name = Form("%s", m_HitMap.name.c_str());
  TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if (!cnvs)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }

  // find or make this this pad
  name = Form("%s_disp_pad", m_HitMap.name.c_str());
  TPad* disp_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if (!disp_pad)
  {  // Make if it does not exist
    disp_pad = new TPad(
        name.c_str(), name.c_str(),
        0.0, 1.0 - m_HitMap.disp_frac,  // Southwest x, y
        1.0 - m_HitMap.lgnd_frac, 1.0   // Northeast x, y
    );
    DrawPad(cnvs, disp_pad);
  }
  CdPad(disp_pad);

  name = Form("%s_disp_text", m_HitMap.name.c_str());
  TText* disp_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if (!disp_text)
  {
    disp_text = new TText(0.5, 0.5, name.c_str());
    disp_text->SetName(name.c_str());
    disp_text->SetTextAlign(22);
    disp_text->SetTextSize(m_HitMap.disp_text_size);
    disp_text->Draw();
  }

  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttEvtHist";
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

  name = Form("%s_title_text", m_HitMap.name.c_str());
  TText* title_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
  if (title_text) return 0;
  // Everything that follows doesn't change,
  // early return if we've drawn it

  title_text = new TText(0.5, 0.75, name.c_str());
  title_text->SetName(name.c_str());
  title_text->SetTextAlign(22);
  title_text->SetTextSize(m_HitMap.disp_text_size);
  title_text->Draw();

  name = Form("%s", m_HitMap.name.c_str());
  title_text->SetTitle(name.c_str());

  return 0;
}

int InttMonDraw::DrawHitMap_LgndPad()
{
  std::string name;

  // use gROOT to find parent TPad (TCanvas)
  name = Form("%s", m_HitMap.name.c_str());
  TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if (!cnvs)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }

  // find or make this this pad
  name = Form("%s_lgnd_pad", m_HitMap.name.c_str());
  TPad* lgnd_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if (lgnd_pad) return 0;
  // Everything that follows doesn't change,
  // early return if we've drawn it

  lgnd_pad = new TPad(
      name.c_str(), name.c_str(),
      1.0 - m_HitMap.lgnd_frac, 0.5 - m_HitMap.disp_frac,  // Southwest x, y
      1.0, 0.5 + m_HitMap.disp_frac                        // Northeast x, y
  );
  DrawPad(cnvs, lgnd_pad);

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
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

    TText* lgnd_text = new TText(
        x0 + 1.5 * m_HitMap.lgnd_box_width,
        y0,
        label.c_str());
    lgnd_text->SetTextAlign(12);
    lgnd_text->SetTextSize(m_HitMap.lgnd_text_size);
    lgnd_text->SetTextColor(kBlack);
    lgnd_text->Draw();

    x[0] = -1;
    x[1] = +1;
    x[2] = +1;
    x[3] = -1;
    y[0] = -1;
    y[1] = -1;
    y[2] = +1;
    y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * m_HitMap.lgnd_box_width;
      x[i] += x0;

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

int InttMonDraw::DrawHitMap_SubPads()
{
  std::string name;

  // use gROOT to find parent TPad (TCanvas)
  name = Form("%s", m_HitMap.name.c_str());
  TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
  if (!cnvs)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }

  double x_min = 0.0;
  double x_max = 1.0;
  double y_min = 0.0;
  double y_max = 1.0 - m_HitMap.disp_frac;
  for (int i = 0; i < 8; ++i)
  {
    name = Form("%s_hist_pad_%d", m_HitMap.name.c_str(), i);
    TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
    if (hist_pad) continue;

    hist_pad = new TPad(
        name.c_str(), name.c_str(),
        x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0,  // Southwest x, y
        x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0   // Southwest x, y
    );
    hist_pad->SetRightMargin(0.2);
    DrawPad(cnvs, hist_pad);
  }

  int iret = 0;
  for (int i = 0; i < 8; ++i)
  {
    iret += DrawHitMap_SubPad(i);
  }

  return iret;
}

int InttMonDraw::DrawHitMap_SubPad(
    int i)
{
  std::string name;

  // use gROOT to find parent TPad
  name = Form("%s_hist_pad_%d", m_HitMap.name.c_str(), i);
  TPad* prnt_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
  if (!prnt_pad)
  {  // If we fail to find it, give up
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
    return 1;
  }
  CdPad(prnt_pad);

  // For now, just the histogram
  // Other niceties (manual axis labels/ticks, maybe gridlines)
  //   in the future (broken up into other methods)

  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttEvtHist";
  TH1D* evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", 0), name);
  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
    return 1;
  }

  name = "InttHitHist";
  TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
  if (!bco_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
    return 1;
  }

  name = Form("%s_hist_%01d", m_HitMap.name.c_str(), i);
  TH2D* hist = dynamic_cast<TH2D*>(gROOT->FindObject(name.c_str()));
  if (!hist)
  {
    hist = new TH2D(
        name.c_str(), name.c_str(),
        26, 0, 25,  // 26, -0.5, 25.5,
        14, 0, 13   // 14, -0.5, 13.5
    );
    hist->GetXaxis()->SetNdivisions(13, true);
    hist->GetYaxis()->SetNdivisions(14, true);
    hist->GetZaxis()->SetRangeUser(0, 3);

    Double_t levels[4] = {0, 1, 2, 3};
    hist->SetContour(4, levels);
  }
  hist->Reset();

  // Fill
  double bin;
  struct InttMon::HitData_s hit_data;
  for (hit_data.fee = 0; hit_data.fee < 14; ++hit_data.fee)
  {
    for (hit_data.chp = 0; hit_data.chp < 26; ++hit_data.chp)
    {
      bin = InttMon::HitBin(hit_data);           // Which bin has the data we want
      bin = bco_hist->GetBinContent((int) bin);  // Reuse the index as the value in that bin
      bin /= evt_hist->GetBinContent(1);         // Normalize by number of events

      // // normalize by strip length--different values for different sensors
      // double norm = (hit_data.chp % 13 < 5) ? 2.0 : 1.6;
      // bin /= norm;

      // Assign a value to this bin
      // that will give it the appropriate color
      // based on how it compares to the hot/cold thresholds
      if (bin < m_HitMap.lower)
      {
        bin = 0.4;  // Cold/Dead
      }
      else if (m_HitMap.upper < bin)
      {
        bin = 3.0;  // Hot
      }
      else
      {
        bin = 1.7;  // Good
      }

      hist->SetBinContent(
          hit_data.chp + 1,  // + 1 is b/c the 0th x bin is an underflow bin
          hit_data.fee + 1,  // + 1 is b/c the 0th y bin is an underflow bin
          bin);
    }
    hist->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));
  }

  hist->DrawCopy("COL");  // "COLZ" for a legend; no legend is preferrable here

  return 0;
}

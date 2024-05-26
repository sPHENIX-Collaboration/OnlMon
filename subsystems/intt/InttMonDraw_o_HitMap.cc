#include "InttMon.h"
#include "InttMonDraw.h"

int InttMonDraw::Draw_HitMap(
) {
  // Set member variables we use to what they should be at beginning of each call
  m_name = "INTT_HitMap";
  m_lgnd_frac = 0.1;

  std::string name;

  MakeCanvas_Generic(k_hitmap);
  TC[k_hitmap]->SetEditable(true);

  DrawDispPad_Generic(k_hitmap);
  DrawLgndPad_HitMap();

  int iret = 1;
  for(int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_HitMap(i) && iret;
  }

  TC[k_hitmap]->Update();
  TC[k_hitmap]->Show();
  TC[k_hitmap]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawLgndPad_HitMap()
{
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.03;
  double lgnd_text_size = 0.12;

  std::string name;

  m_lgnd_pad[k_hitmap]->Clear();
  m_lgnd_pad[k_hitmap]->cd();

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
  for (int c = 0; c < 3; ++c)
  {
    x0 = 0.5 - lgnd_box_width;
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

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(
        x0 + 1.5 * lgnd_box_width, y0, //
        label.c_str() //
	);

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
    box.SetFillColor(color);
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  return 0;
}

int InttMonDraw::DrawHistPad_HitMap(
    int i)
{
  double lower = 10e-4, upper = 10e-2;

  std::string name;

  name = Form("%s_hist_%01d", m_name.c_str(), i);
  if(!m_hist_hitmap[i])
  {
    std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tNote: TH2D \"" << name << "\" allocated" << std::endl;
    m_hist_hitmap[i] = new TH2D(
        name.c_str(), name.c_str(),
        26, 0, 25,  // 26, -0.5, 25.5,
        14, 0, 13   // 14, -0.5, 13.5
    );
    m_hist_hitmap[i]->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));

    m_hist_hitmap[i]->GetXaxis()->SetNdivisions(13, true);
    m_hist_hitmap[i]->GetYaxis()->SetNdivisions(14, true);
    m_hist_hitmap[i]->GetZaxis()->SetRangeUser(0, 3);
    m_hist_hitmap[i]->SetFillStyle(4000); // Transparent

    Double_t levels[4] = {0, 1, 2, 3};
    m_hist_hitmap[i]->SetContour(4, levels);
  }
  m_hist_pad[k_hitmap][i]->cd();

  m_hist_hitmap[i]->Reset();
  m_hist_hitmap[i]->Draw("COL");  // "COLZ" for a legend; no legend is preferrable here

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  name = "InttEvtHist";
  TH1D* evt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_%d", i), name));
  if (!evt_hist)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
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

      // Assign a value to this bin
      // that will give it the appropriate color
      // based on how it compares to the hot/cold thresholds
      if (bin < lower)
      {
        bin = 0.4;  // Cold/Dead
      }
      else if (upper < bin)
      {
        bin = 3.0;  // Hot
      }
      else
      {
        bin = 1.7;  // Good
      }

      m_hist_hitmap[i]->SetBinContent(
          hit_data.chp + 1,  // + 1 is b/c the 0th x bin is an underflow bin
          hit_data.fee + 1,  // + 1 is b/c the 0th y bin is an underflow bin
          bin // 
	  );
    }
  }

  return 0;
}

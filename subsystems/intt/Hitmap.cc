#include "Hitmap.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TH2D.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <TText.h>

#include <boost/format.hpp>

Hitmap::Hitmap(std::string const& name) : SingleInttDrawer(name)
{
}

Hitmap::~Hitmap()
{
  for(auto& hist_ptr : m_hist)
  {
    delete hist_ptr;
  }
}

int Hitmap::DrawCanvas()
{
  MakeCanvas();
  m_canvas->SetTitle("Intt Hit Map");
  return SingleInttDrawer::DrawCanvas();
}

int Hitmap::DrawLgnd()
{
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.03;
  double lgnd_text_size = 0.12;

  m_lgnd_pad->Clear();
  m_lgnd_pad->cd();

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
    default:
      label = "Unknown";
      color = kOrange;
      break;
    }

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(
        x0 + 1.5 * lgnd_box_width, y0,  //
        label.c_str()                   //
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

int Hitmap::DrawHist(int fsv)
{
  if (!m_hist[fsv])
  {
    std::string name = (boost::format("%s_hist_%01d") % m_name.c_str() % fsv).str();
    m_hist[fsv] = new TH2D(
        name.c_str(), name.c_str(),
        26, -0.5, 25.5, //
        14, -0.5, 13.5  //
    );
    m_hist[fsv]->SetTitle((boost::format("intt%01d;Chip ID (0-base);Felix Channel") % fsv).str().c_str());

    m_hist[fsv]->GetXaxis()->SetNdivisions(13, true);
    m_hist[fsv]->GetYaxis()->SetNdivisions(14, true);
    m_hist[fsv]->GetZaxis()->SetRangeUser(0, 3);
    m_hist[fsv]->SetFillStyle(4000);  // Transparent

    Double_t levels[4] = {0, 1, 2, 3};
    m_hist[fsv]->SetContour(4, levels);
  }
  m_hist_pad[fsv]->SetGrid(1);
  m_hist_pad[fsv]->cd();

  m_hist[fsv]->Reset();
  m_hist[fsv]->Draw("COL");  // "COLZ" for a legend, "COL" for no legend; no legend is preferrable here

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();
  TH1* evt_hist = cl->getHisto((boost::format("INTTMON_%d") % fsv).str(), "InttEvtHist");
  TH1* hit_hist = cl->getHisto((boost::format("INTTMON_%d") % fsv).str(), "InttHitHist");
  if (SingleInttDrawer::DrawHist(fsv) || !evt_hist || !hit_hist)
  {
    return 1;
  }

  // Fill
  double lower = 0.015;
  double upper = 0.650;
  for (int fch = 0; fch < 14; ++fch)
  {
    for (int chp = 0; chp < 128; ++chp)
    {
      double bincont = hit_hist->GetBinContent(fch * 128 + chp + 1);
      if(!bincont)
      {
        continue;
      }
      bincont /= evt_hist->GetBinContent(2); // Normalize by number of unique BCOs

      // Assign a value to this bin
      // that will give it the appropriate color
      // based on how it compares to the hot/cold thresholds
      if (bincont < lower)
      {
        bincont = 0.4;  // 0.4 Cold/Dead
      }
      else if (upper < bincont)
      {
        bincont = 3.0;  // 3.0 Hot
      }
      else
      {
        bincont = 1.7;  // 1.7 Good
      }

      m_hist[fsv]->SetBinContent(chp + 1, fch + 1, bincont);  // +1 to start at first bin
    }
  }

  return 0;
}


#include "SingleInttDrawer.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>
#include <phool/phool.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <boost/format.hpp>

SingleInttDrawer::SingleInttDrawer(std::string const& name)
  : SingleCanvasDrawer(name)
{
}

int SingleInttDrawer::DrawCanvas()
{
  gStyle->SetOptStat(0);
  MakeCanvas();

  int iret = 0;
  iret += DrawDisp();
  iret += DrawLgnd();
  for(int fsv = 0; fsv < 8; ++fsv)
  {
    iret += DrawHist(fsv);
  }
  m_canvas->Update();
  m_canvas->Show();

  return iret;
}

int SingleInttDrawer::MakeCanvas()
{
  if(SingleCanvasDrawer::MakeCanvas())
  {
    return 1;
  }

  std::string name;

  name = (boost::format("%s_disp_pad") % m_name.c_str()).str();
  m_disp_pad = new TPad(
      name.c_str(), name.c_str(),  //
      0.0, 1.0 - m_disp_frac,      // Southwest x, y
      1.0, 1.0                     // Northeast x, y
  );
  m_canvas->cd();
  m_disp_pad->SetFillStyle(4000);  // Transparent
  m_disp_pad->Range(0.0, 0.0, 1.0, 1.0);
  m_disp_pad->Draw();

  if(m_lgnd_frac)
  {
    name = (boost::format("%s_lgnd_pad") % m_name.c_str()).str();
    m_lgnd_pad = new TPad(
        name.c_str(), name.c_str(),  //
        1.0 - m_lgnd_frac, 0.0,      // Southwest x, y
        1.0, 1.0 - m_disp_frac       // Northeast x, y
    );
    m_canvas->cd();
    m_lgnd_pad->SetFillStyle(4000);  // Transparent
    m_lgnd_pad->Range(0.0, 0.0, 1.0, 1.0);
    m_lgnd_pad->Draw();
  }

  for (int fsv = 0; fsv < 8; ++fsv)
  {
    name = (boost::format("%s_hist_pad_%01d") % m_name.c_str() % fsv).str();
    m_hist_pad[fsv] = new TPad(
        name.c_str(), name.c_str(),                                                                //
        (fsv % 4 + 0.0) / 4.0 * (1.0 - m_lgnd_frac), (fsv / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (fsv % 4 + 1.0) / 4.0 * (1.0 - m_lgnd_frac), (fsv / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    m_canvas->cd();
    m_hist_pad[fsv]->SetFillStyle(4000);  // Transparent
    m_hist_pad[fsv]->Range(0.0, 0.0, 1.0, 1.0);
    m_hist_pad[fsv]->Draw();

    name = (boost::format("%s_dead_pad_%01d") % m_name.c_str() % fsv).str();
    m_dead_pad[fsv] = new TPad(
        name.c_str(), name.c_str(),                                                                //
        (fsv % 4 + 0.0) / 4.0 * (1.0 - m_lgnd_frac), (fsv / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (fsv % 4 + 1.0) / 4.0 * (1.0 - m_lgnd_frac), (fsv / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    m_canvas->cd();
    m_dead_pad[fsv]->SetFillStyle(4000);  // Transparent
    m_dead_pad[fsv]->Range(0.0, 0.0, 1.0, 1.0);
    m_dead_pad[fsv]->Draw();
  }

  name = (boost::format("%s_transparent") % m_name.c_str()).str();
  m_transparent = new TPad(
      name.c_str(), name.c_str(),  //
      0.0, 0.0,                    // Southwest x, y
      1.0, 1.0                     // Northeast x, y
  );
  m_canvas->cd();
  m_transparent->SetFillStyle(4000);  // Transparent
  m_transparent->Range(0.0, 0.0, 1.0, 1.0);
  m_transparent->Draw();

  return 0;
}

int SingleInttDrawer::DrawDisp()
{
  m_transparent->Clear();
  m_disp_pad->Clear();
  m_disp_pad->cd();

  OnlMonClient* cl = OnlMonClient::instance();

  // Title
  TText title_text;
  title_text.SetTextAlign(22);
  title_text.SetTextSize(m_disp_text_size);
  title_text.DrawText(0.5, 0.75, m_canvas->GetTitle());

  // Get the "event" count from the first server we can
  TH1* evt_hist{nullptr};
  for (int fsv = 0; fsv < 8; ++fsv)
  {
    if ((evt_hist = cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttEvtHist")))
    {
      break;
    }
  }

  // If we can't find any, there is a problem
  if (!evt_hist)
  {
    m_transparent->cd();

    TText dead_text;
    dead_text.SetTextColor(kRed);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(m_dead_text_size);
    dead_text.DrawText(0.5, 0.6, "All");
    dead_text.DrawText(0.5, 0.5, "intt OnlMon");
    dead_text.DrawText(0.5, 0.4, "Servers Dead");

    return 1;
  }

  // Display text
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");  // BOR, CURRENT, or EOR
  std::string text = "Run " + std::to_string(cl->RunNumber()) + ", Events: " + std::to_string((int) evt_hist->GetBinContent(1)) + ", " + ctime(&evttime.first);
  TText disp_text;
  disp_text.SetTextColor(evttime.second);
  disp_text.SetTextAlign(22);
  disp_text.SetTextSize(m_disp_text_size);
  disp_text.DrawText(0.5, 0.5, text.c_str());

  // Disclaimer if not enough events
  if (evt_hist->GetBinContent(1) < m_min_events)
  {
    TText warn_text;
    warn_text.SetTextAlign(22);
    warn_text.SetTextSize(m_warn_text_size);
    warn_text.SetTextColor(kRed);
    warn_text.DrawText(0.5, 0.25, (boost::format("Not statistically significant (fewer than  %0.E events)") % m_min_events).str().c_str());
  }

  return 0;
}

int SingleInttDrawer::DrawLgnd()
{
  if(!m_lgnd_frac)
  {
    return 0;
  }

  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad->Clear();
  m_lgnd_pad->cd();

  double x0, y0, x[4], y[4];
  for (int fch = 0; fch < 14; ++fch)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fch + 1.0) / (2.0 * 14);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, (boost::format("FChn %2d") % fch).str().c_str());

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
    box.SetFillColor(GetFchColor(fch));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  return 0;
}

int SingleInttDrawer::DrawHist(int fsv)
{
  m_dead_pad[fsv]->Clear();
  m_dead_pad[fsv]->cd();

  OnlMonClient* cl = OnlMonClient::instance();
  if(cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttEvtHist"))
  {
    return 0;
  }

  TText dead_text;
  dead_text.SetTextColor(kBlue);
  dead_text.SetTextAlign(22);
  dead_text.SetTextSize(m_dead_text_size);
  dead_text.SetTextAngle(45);
  dead_text.DrawText(0.45, 0.55, (boost::format("intt%01d OnlMon") % fsv).str().c_str());
  dead_text.DrawText(0.55, 0.45, "Server Dead");

  return 1;
}

int SingleInttDrawer::GetFchColor(int fch)
{
  switch (fch % 7)
  {
  case 0:
    return (fch / 7) ? kOrange : kBlack;
  case 1:
    return kRed + 3 * (fch / 7);
  case 2:
    return kViolet + 3 + 7 * (fch / 7);
  case 3:
    return kGreen + 3 * (fch / 7);
  case 4:
    return kCyan + 3 * (fch / 7);
  case 5:
    return kBlue + 3 * (fch / 7);
  case 6:
    return kMagenta + 3 * (fch / 7);
  }
  return kBlack;
}


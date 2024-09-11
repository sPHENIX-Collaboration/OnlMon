#include "History.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TH1D.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <TText.h>

#include <boost/format.hpp>

History::History(std::string const& name) : SingleInttDrawer(name)
{
}

History::~History()
{
  for(auto& hist_ptr : m_hist)
  {
    delete hist_ptr;
  }

}

int History::MakeCanvas()
{
  if(SingleInttDrawer::MakeCanvas())
  {
    return 1;
  }

  std::string name;

  name = (boost::format("%s_single") % m_name.c_str()).str();
  m_single = new TPad(
      name.c_str(), name.c_str(),          //
      0.0, 0.0,                            // Southwest x, y
      1.0 - m_lgnd_frac, 1.0 - m_disp_frac // Northeast x, y
  );
  m_canvas->cd();
  m_single->SetFillStyle(4000);  // Transparent
  m_single->Range(0.0, 0.0, 1.0, 1.0);
  m_single->Draw();

  name = (boost::format("%s_dead") % m_name.c_str()).str();
  m_dead = new TPad(
      name.c_str(), name.c_str(),          //
      0.0, 0.0,                            // Southwest x, y
      1.0 - m_lgnd_frac, 1.0 - m_disp_frac // Northeast x, y
  );
  m_canvas->cd();
  m_dead->SetFillStyle(4000);  // Transparent
  m_dead->Range(0.0, 0.0, 1.0, 1.0);
  m_dead->Draw();

  return 0;
}

int History::DrawCanvas()
{
  MakeCanvas();
  m_canvas->SetTitle("Intt History");

  int iret = 0;
  iret += DrawDisp();
  iret += DrawLgnd();

  // hist
  int num_dead = 0;

  int oldest = std::numeric_limits<int>::max();
  int newest = std::numeric_limits<int>::min();

  for(int fsv = 0; fsv < 8; ++fsv)
  {
    // Access client
    OnlMonClient* cl = OnlMonClient::instance();

    TH1* evt_hist = cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttEvtHist");
    TH1* log_hist = cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttLogHist");
    if(!evt_hist || !log_hist)
    {
      ++num_dead;
      continue;
    }

    int N = log_hist->GetNbinsX();
    double w = log_hist->GetXaxis()->GetBinWidth(0);

	// Step through most recent 90 seconds of the histogram
	// if the rate is identically 0, say it is dead
	bool is_dead = true;
    int buff_index = log_hist->GetBinContent(N);
    for(double duration = 0; duration < 90; duration += w)
    {
      double rate = log_hist->GetBinContent(buff_index);
	  if(0 < rate)
	  {
        is_dead = false;
        break;
	  }

	  // N + 1 bin stores how many times the data has been wrapped
	  // if it's not wrapped and we're at bin 0, break b/c we don't have the duration of data yet
	  if(buff_index == 0 && (log_hist->GetBinContent(N + 1) == 0))
	  {
        is_dead = false;
        break;
	  }

	  buff_index = (buff_index + N - 1) % N;
    }

	if(is_dead)
	{
      ++num_dead;
	}

    // Get the time frame of server relative to Unix Epoch
    // stored in bins 4 and 5 of EvtHist
    if(evt_hist->GetBinContent(4) < oldest)
    {
      oldest = evt_hist->GetBinContent(4); // SOR time relative to epoch
    }
    if(newest < evt_hist->GetBinContent(5))
    {
      newest = evt_hist->GetBinContent(5); // present or EOR time relative to epoch
    }
  }

  m_dead->Clear();
  if(0 < num_dead)
  {
    m_dead->cd();
    TText dead_text;
    dead_text.SetTextColor(kRed);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.06);
    // dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.65, "Dead Felix Servers");
    dead_text.DrawText(0.5, 0.50, "Check server stats");
    dead_text.DrawText(0.5, 0.35, "If no dead OnlMon servers, restart run");
  }

  double max = 1.0;
  for(int fsv = 0; fsv < 8; ++fsv)
  {
    // Access client
    OnlMonClient* cl = OnlMonClient::instance();

    TH1* evt_hist = cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttEvtHist");
    TH1* log_hist = cl->getHisto((boost::format("INTTMON_%01d") % fsv).str(), "InttLogHist");
    if(!evt_hist || !log_hist)
    {
      continue;
    }

    int N = log_hist->GetNbinsX();
    double w = log_hist->GetXaxis()->GetBinWidth(0);
    // Validate member histos
    if (!m_hist[fsv])
    {
      std::string name = (boost::format("%s_hist_%01d") % m_name.c_str() % fsv).str();
      m_hist[fsv] = new TH1D(
          name.c_str(), name.c_str(), //
          N, 0.0, w * N //
      );
      m_hist[fsv]->SetTitle((boost::format("Rate of BCO decoding;Most recent %.0lf seconds;Decoded BCOs / s") % (double)(w * N)).str().c_str());
      m_hist[fsv]->SetFillStyle(4000); // Transparent
      m_hist[fsv]->SetLineColor(GetFchColor(fsv)); // Transparent
    }
    m_single->cd();

    m_hist[fsv]->Reset();
    m_hist[fsv]->Draw("Same");

    // Fill
	if(w * N < newest - oldest)
	{
      // Draw it conventionally
	  // Present/EOR is aligned on right edge
      int buff_index = log_hist->GetBinContent(N);
      for(int n = 0; n < N; ++n)
      {
        double rate = log_hist->GetBinContent(buff_index) / w;
	    double time = (evt_hist->GetBinContent(5) - newest) + w * (N - n);
	    int bin = m_hist[fsv]->FindBin(time);

	    m_hist[fsv]->SetBinContent(bin, rate);
        if(max < rate)
        {
          max = rate;
        }

	    // N + 1 bin stores how many times the data has been wrapped
	    // if it's not wrapped and we're at bin 0, break b/c we don't have the duration of data yet
	    if(buff_index == 0 && (log_hist->GetBinContent(N + 1) == 0))
	    {
          break;
	    }
	    buff_index = (buff_index + N - 1) % N;
      }
	}
	else
	{
      // I don't know why people want this but here it is
	  // SOR is aligned with left edge
	  int buff_index = log_hist->GetBinContent(N);
      for(int n = 0; n < N; ++n)
      {
        double rate = log_hist->GetBinContent(buff_index % N) / w;
	    double time = evt_hist->GetBinContent(5) - oldest - w * n;
	    int bin = m_hist[fsv]->FindBin(time);

	    m_hist[fsv]->SetBinContent(bin, rate);
        if(max < rate)
        {
          max = rate;
        }

	    // N + 1 bin stores how many times the data has been wrapped
	    // if it's not wrapped and we're at bin 0, break b/c we don't have the duration of data yet
	    if(buff_index == 0 && (log_hist->GetBinContent(N + 1) == 0))
	    {
          break;
	    }
	    buff_index = (buff_index + N - 1) % N;
      }
	}
  }


  for(int fsv = 0; fsv < 8; ++fsv)
  {
    if(!m_hist[fsv])
    {
      continue;
    }
    m_hist[fsv]->GetYaxis()->SetRangeUser(-0.2 * max,  1.2 * max);
  }

  m_canvas->Update();
  m_canvas->Show();

  return iret;
}

int History::DrawLgnd()
{
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad->Clear();
  m_lgnd_pad->cd();

  double x0, y0, x[4], y[4];
  for (int fsv = 0; fsv < 8; ++fsv)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fsv + 1.0) / (2.0 * 8);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, (boost::format("intt%01d") % fsv).str().c_str());

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
    box.SetFillColor(GetFchColor(fsv));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  return 0;
}

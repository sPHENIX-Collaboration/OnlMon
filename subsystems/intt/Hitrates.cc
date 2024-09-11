#include "Hitrates.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TH1D.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <TText.h>

#include <boost/format.hpp>

Hitrates::Hitrates(std::string const& name) : SingleInttDrawer(name)
{
}

Hitrates::~Hitrates()
{
  for(auto& hist_ptr : m_hist)
  {
    delete hist_ptr;
  }

}

int Hitrates::DrawCanvas()
{
  m_lgnd_frac = 0.0;
  MakeCanvas();
  m_canvas->SetTitle("Intt Hitrates");
  return SingleInttDrawer::DrawCanvas();
}

int Hitrates::DrawHist(int fsv)
{
  double lower = 0.00;
  double upper = 0.65;

  // Validate member histos
  if (!m_hist[fsv])
  {
    std::string name = (boost::format("%s_hist_%01d") % m_name.c_str() % fsv).str();
    m_hist[fsv] = new TH1D(
        name.c_str(), name.c_str(), //
        112, lower, upper           //
    );
    m_hist[fsv]->SetTitle((boost::format("intt%01d;Hits/Event (overflow is shown in last bin);Entries (One Hitrate per Chip)") % fsv).str().c_str());
    m_hist[fsv]->GetXaxis()->SetNdivisions(8, true);
    m_hist[fsv]->SetFillStyle(4000); // Transparent
  }
  m_hist_pad[fsv]->cd();

  m_hist[fsv]->Reset();
  m_hist[fsv]->Draw();

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();
  TH1* evt_hist = cl->getHisto((boost::format("INTTMON_%d") % fsv).str(), "InttEvtHist");
  TH1* hit_hist = cl->getHisto((boost::format("INTTMON_%d") % fsv).str(), "InttHitHist");
  if (SingleInttDrawer::DrawHist(fsv) || !evt_hist || !hit_hist)
  {
    return 1;
  }

  // Fill
  for (int fee = 0; fee < 14; ++fee)
  {
    for (int chp = 0; chp < 128; ++chp)
    {
      double bincont = hit_hist->GetBinContent(fee * 128 + chp + 1);
      bincont /= evt_hist->GetBinContent(2); // Normalize by number of unique BCOs

      // Manually catch overflows and put them in the last displayed bin
      if (upper <= bincont)
      {
        bincont = upper - m_hist[fsv]->GetXaxis()->GetBinWidth(1);
      }

      m_hist[fsv]->Fill(bincont);
    }
  }

  return 0;
}

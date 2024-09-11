#include "FphxBco.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TPad.h>

#include <boost/format.hpp>

FphxBco::FphxBco(std::string const& name) : SingleInttDrawer(name)
{
}

FphxBco::~FphxBco()
{
  for(auto& hist_arr : m_hist)
  {
    for(auto& hist_ptr : hist_arr)
    {
      delete hist_ptr;
    }
  }
}

int FphxBco::DrawCanvas()
{
  MakeCanvas();
  m_canvas->SetTitle("Just Fphx Bco (Streaming)");
  return SingleInttDrawer::DrawCanvas();
}

int FphxBco::DrawHist(int fsv)
{
  for (int fch = 0; fch < 14; ++fch)
  {
    if (!m_hist[fsv][fch])
    {
      std::string name = (boost::format("%s_hist_%01d_%02d") % m_name.c_str() % fsv % fch).str();
      m_hist[fsv][fch] = new TH1D(
          name.c_str(), name.c_str(),  //
          128, 0, 128                  //
      );
      m_hist[fsv][fch]->SetTitle((boost::format("intt%01d;FPHX BCO;Counts (Hits)") % fsv).str().c_str());
      m_hist[fsv][fch]->GetXaxis()->SetNdivisions(16);  //, true);
      m_hist[fsv][fch]->SetLineColor(GetFchColor(fch));
      m_hist[fsv][fch]->SetFillStyle(4000);  // Transparent
    }
    m_hist_pad[fsv]->cd();
    m_hist_pad[fsv]->SetLogy();

    m_hist[fsv][fch]->Reset();
    m_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, 10);
    m_hist[fsv][fch]->Draw("same");
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();
  TH1* bco_hist = cl->getHisto((boost::format("INTTMON_%d") % fsv).str(), "InttBcoHist");
  if (SingleInttDrawer::DrawHist(fsv) || !bco_hist)
  {
    return 1;
  }

  // Fill
  double max = 1;
  for (int fch = 0; fch < 14; ++fch)
  {
    for (int bco = 0; bco < 128; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fch * 128  + bco + 1));
      m_hist[fsv][fch]->SetBinContent(bco + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin

      if (max < bincont)
      {
        max = bincont;
      }
    }
  }
  max *= 10;

  // Noramlize ranges
  for (int fch = 0; fch < 14; ++fch)
  {
    m_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, max);
  }

  return 0;
}


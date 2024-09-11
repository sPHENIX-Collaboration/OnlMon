#include "ZoomedFphxBco.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TPad.h>

#include <boost/format.hpp>

ZoomedFphxBco::ZoomedFphxBco(std::string const& name) : SingleInttDrawer(name)
{
}

ZoomedFphxBco::~ZoomedFphxBco()
{
  for(auto& hist_arr : m_l_hist)
  {
    for(auto& hist_ptr : hist_arr)
    {
      delete hist_ptr;
    }
  }

  for(auto& hist_arr : m_r_hist)
  {
    for(auto& hist_ptr : hist_arr)
    {
      delete hist_ptr;
    }
  }
}

int ZoomedFphxBco::MakeCanvas()
{
  if(SingleInttDrawer::MakeCanvas())
  {
    return 1;
  }

  for(int fsv = 0; fsv < 8; ++fsv)
  {
    std::string name;

    name = (boost::format("%s_l_hist_pad_%01d") % m_name.c_str() % fsv).str();
    m_l_hist_pad[fsv] = new TPad (
      name.c_str(), name.c_str(),
	  0.0, 0.0, 0.5, 1.0
	);
    m_hist_pad[fsv]->cd();
    m_l_hist_pad[fsv]->SetFillStyle(4000);  // Transparent
    m_l_hist_pad[fsv]->SetLeftMargin(0.15);
    m_l_hist_pad[fsv]->SetRightMargin(0.01);
    m_l_hist_pad[fsv]->Range(0.0, 0.0, 1.0, 1.0);
    m_l_hist_pad[fsv]->Draw();

    name = (boost::format("%s_r_hist_pad_%01d") % m_name.c_str() % fsv).str();
    m_r_hist_pad[fsv] = new TPad (
      name.c_str(), name.c_str(),
	  0.5, 0.0, 1.0, 1.0
	);
    m_hist_pad[fsv]->cd();
    m_r_hist_pad[fsv]->SetFillStyle(4000);  // Transparent
    m_r_hist_pad[fsv]->SetLeftMargin(0.01);
    m_r_hist_pad[fsv]->Range(0.0, 0.0, 1.0, 1.0);
    m_r_hist_pad[fsv]->Draw();
  }

  return 0;
}

int ZoomedFphxBco::DrawCanvas()
{
  MakeCanvas();
  m_canvas->SetTitle("Zoomed Fphx Bco (Streaming)");
  return SingleInttDrawer::DrawCanvas();
}

int ZoomedFphxBco::DrawHist(int fsv)
{
  int num_fphx_bins = 20;
  for (int fch = 0; fch < 14; ++fch)
  {
    if (!m_l_hist[fsv][fch])
    {
      std::string name = (boost::format("%s_l_hist_%01d_%02d") % m_name.c_str() % fsv % fch).str();
      m_l_hist[fsv][fch] = new TH1D(
          name.c_str(), name.c_str(),     //
          num_fphx_bins, 0, num_fphx_bins //
      );
      m_l_hist[fsv][fch]->SetTitle((boost::format("intt%01d;FPHX BCO;Counts (Hits)") % fsv).str().c_str());
      m_l_hist[fsv][fch]->GetYaxis()->SetTitleSize(0.0);
      m_l_hist[fsv][fch]->GetYaxis()->SetTitleOffset(999);
      m_l_hist[fsv][fch]->GetXaxis()->SetNdivisions(16);  //, true);
      // m_l_hist[fsv][fch]->GetYaxis()->SetLabelSize(0.0);
      // m_l_hist[fsv][fch]->GetYaxis()->SetLabelOffset(999);
      m_l_hist[fsv][fch]->SetLineColor(GetFchColor(fch));
      m_l_hist[fsv][fch]->SetFillStyle(4000);  // Transparent
    }
    m_l_hist_pad[fsv]->cd();
    m_l_hist_pad[fsv]->SetLogy();

    m_l_hist[fsv][fch]->Reset();
    m_l_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, 10);
    m_l_hist[fsv][fch]->Draw("same");

    if (!m_r_hist[fsv][fch])
    {
      std::string name = (boost::format("%s_r_hist_%01d_%02d") % m_name.c_str() % fsv % fch).str();
      m_r_hist[fsv][fch] = new TH1D(
          name.c_str(), name.c_str(),             //
          num_fphx_bins, 128 - num_fphx_bins, 128 //
      );
      m_r_hist[fsv][fch]->SetTitle((boost::format("intt%01d;FPHX BCO;Counts (Hits)") % fsv).str().c_str());
      m_r_hist[fsv][fch]->SetTitleSize(0.0);
      m_r_hist[fsv][fch]->SetTitleOffset(999);
      m_r_hist[fsv][fch]->GetXaxis()->SetNdivisions(16);  //, true);
      m_r_hist[fsv][fch]->GetYaxis()->SetLabelSize(0.0);
      m_r_hist[fsv][fch]->GetYaxis()->SetLabelOffset(999);
      m_r_hist[fsv][fch]->SetLineColor(GetFchColor(fch));
      m_r_hist[fsv][fch]->SetFillStyle(4000);  // Transparent
    }
    m_r_hist_pad[fsv]->cd();
    m_r_hist_pad[fsv]->SetLogy();

    m_r_hist[fsv][fch]->Reset();
    m_r_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, 10);
    m_r_hist[fsv][fch]->Draw("same");
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
    for (int bco = 0; bco < num_fphx_bins; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fch * 128  + bco + 1));
      m_l_hist[fsv][fch]->SetBinContent(bco + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin

      if (max < bincont)
      {
        max = bincont;
      }
    }

    for (int bco = 128 - num_fphx_bins; bco < 128; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fch * 128  + bco + 1));
      m_r_hist[fsv][fch]->SetBinContent(bco - (128 - num_fphx_bins) + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin

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
    m_l_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, max);
    m_r_hist[fsv][fch]->GetYaxis()->SetRangeUser(1, max);

    // TLines
    TLine line;
    line.SetLineWidth(6);
    line.SetLineColor(kMagenta);
    line.SetLineStyle(2);
  
    m_l_hist_pad[fsv]->cd();
    line.DrawLine(5, 0, 5, max);
  
    m_r_hist_pad[fsv]->cd();
    line.DrawLine(116, 0, 116, max);
    line.DrawLine(120, 0, 120, max);
  }

  return 0;
}


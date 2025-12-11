#include "ZdcMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TText.h>
#include <TLatex.h>
#include <TLine.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

ZdcMonDraw::ZdcMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int ZdcMonDraw::Init()
{
  return 0;
}

int ZdcMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "ZdcMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "ZDC-SMD Monitor", -1, ysize, xsize * 0.9, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("zdcpad1", "who needs this?", 0.05, 0.5, 0.5, 0.9, 0);
    Pad[1] = new TPad("zdcpad2", "who needs this?", 0.5, 0.5, 0.95, 0.9, 0);
    Pad[2] = new TPad("zdcpad3", "who needs this?", 0.05, 0.05, 0.5, 0.45, 0);
    Pad[3] = new TPad("zdcpad4", "who needs this?", 0.5, 0.05, 0.95, 0.45, 0);
    Pad[0]->Draw();
    Pad[1]->Draw();
    Pad[2]->Draw();
    Pad[3]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "ZdcMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "ZDC North and South Channels", -1, -ysize, xsize * 0.9, ysize);
    gSystem->ProcessEvents();

    Pad[4] = new TPad("zdcpad5", "who needs this?", 0.05, 0.65, 0.35, 0.95, 0);
    Pad[5] = new TPad("zdcpad6", "who needs this?", 0.35, 0.65, 0.65, 0.95, 0);
    Pad[6] = new TPad("zdcpad7", "who needs this?", 0.65, 0.65, 0.95, 0.95, 0);
    Pad[7] = new TPad("zdcpad8", "who needs this?", 0.05, 0.35, 0.35, 0.65, 0);
    Pad[8] = new TPad("zdcpad9", "who needs this?", 0.35, 0.35, 0.65, 0.65, 0);
    Pad[9] = new TPad("zdcpad10", "who needs this?", 0.65, 0.35, 0.95, 0.65, 0);

    Pad[4]->Draw();
    Pad[5]->Draw();
    Pad[6]->Draw();
    Pad[7]->Draw();
    Pad[8]->Draw();
    Pad[9]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  // DRAW SMD GOOD AND SMALL VALUES
  else if (name == "SmdValues")
  {
    // xpos negative: do not draw menu bar
    TC[2] = new TCanvas(name.c_str(), " Smd Values", -1, ysize, xsize*0.9, ysize / 2);

    gSystem->ProcessEvents();
    Pad[10] = new TPad("Smd Value", "Smd Value", 0.05, 0.05, 0.35, 0.9, 0);
    Pad[11] = new TPad("Smd Value (good)", "Smd Value (good)", 0.35, 0.05, 0.65, 0.9, 0);
    Pad[12] = new TPad("Smd Value (small)", "Smd Value (small)", 0.65, 0.05, 0.95, 0.9, 0);

    Pad[10]->Draw();
    Pad[11]->Draw();
    Pad[12]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillStyle(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
  }

  // DRAW SMD NORTH AND SOUTH
  else if (name == "SmdNorthandSouth")
  {
    // xpos negative: do not draw menu bar
    TC[3] = new TCanvas(name.c_str(), "EXPERT - Smd North and South", -1, ysize, xsize * 0.9, ysize * 0.9);
    gSystem->ProcessEvents();

    Pad[13] = new TPad("Smd Ver North (good)", "Smd Ver North (good)", 0.02, 0.05, 0.26, 0.35, 0);
    Pad[14] = new TPad("Smd Hor North (good)", "Smd Hor North (good)", 0.26, 0.05, 0.5, 0.35, 0);
    Pad[15] = new TPad("Smd Ver North (small)", "Smd Ver North (small)", 0.5, 0.05, 0.74, 0.35, 0);
    Pad[16] = new TPad("Smd Hor North (small)", "Smd Hor North (small)", 0.74, 0.05, 0.98, 0.35, 0);

    Pad[17] = new TPad("Smd Ver North", "Smd Ver North", 0.02, 0.35, 0.26, 0.65, 0);
    Pad[18] = new TPad("Smd Hor North", "Smd Hor North", 0.26, 0.35, 0.5, 0.65, 0);
    Pad[19] = new TPad("Smd Ver South", "Smd Ver South", 0.5, 0.35, 0.74, 0.65, 0);
    Pad[20] = new TPad("Smd Hor South", "Smd Hor South", 0.74, 0.35, 0.98, 0.65, 0);

    Pad[21] = new TPad("Smd sum Ver North", "Smd sum Ver North", 0.02, 0.65, 0.26, 0.95, 0);
    Pad[22] = new TPad("Smd sum Hor North", "Smd sum Hor North", 0.26, 0.65, 0.5, 0.95, 0);
    Pad[23] = new TPad("Smd sum Ver South", "Smd sum Ver South", 0.5, 0.65, 0.74, 0.95, 0);
    Pad[24] = new TPad("Smd sum Hor South", "Smd sum Hor South", 0.74, 0.65, 0.98, 0.95, 0);

    Pad[13]->Draw();
    Pad[14]->Draw();
    Pad[15]->Draw();
    Pad[16]->Draw();
    Pad[17]->Draw();
    Pad[18]->Draw();
    Pad[19]->Draw();
    Pad[20]->Draw();
    Pad[21]->Draw();
    Pad[22]->Draw();
    Pad[23]->Draw();
    Pad[24]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(false);
  }

  else if (name == "SmdAdcNorthIndividual")
  {
    // xpos negative: do not draw menu bar
    TC[4] = new TCanvas(name.c_str(), "SMD ADC North Individual values", -1, -ysize, xsize * 0.9, ysize * 0.9);
    gSystem->ProcessEvents();
    //  North Horizontal

    // xmin, ymin, xmax, ymax
    // First Line
    Pad[21] = new TPad("smd_adc_n_hor0", "smd_adc_n_hor0", 0.02, 0.74, 0.26, 0.98, 0);
    Pad[22] = new TPad("smd_adc_n_hor1", "smd_adc_n_hor1", 0.26, 0.74, 0.50, 0.98, 0);
    Pad[23] = new TPad("smd_adc_n_hor2", "smd_adc_n_hor2", 0.50, 0.74, 0.74, 0.98, 0);
    Pad[24] = new TPad("smd_adc_n_hor3", "smd_adc_n_hor3", 0.74, 0.74, 0.98, 0.98, 0);
    // Second Line
    Pad[25] = new TPad("smd_adc_n_hor4", "smd_adc_n_hor4", 0.02, 0.50, 0.26, 0.74, 0);
    Pad[26] = new TPad("smd_adc_n_hor5", "smd_adc_n_hor5", 0.26, 0.50, 0.50, 0.74, 0);
    Pad[27] = new TPad("smd_adc_n_hor6", "smd_adc_n_hor6", 0.50, 0.50, 0.74, 0.74, 0);
    Pad[28] = new TPad("smd_adc_n_hor7", "smd_adc_n_hor7", 0.74, 0.50, 0.98, 0.74, 0);
    // Third Line
    Pad[29] = new TPad("smd_adc_n_ver8", "smd_adc_n_ver8", 0.02, 0.26, 0.26, 0.50, 0);
    Pad[30] = new TPad("smd_adc_n_ver9", "smd_adc_n_ver9", 0.26, 0.26, 0.50, 0.50, 0);
    Pad[31] = new TPad("smd_adc_n_ver10", "smd_adc_n_ver10", 0.50, 0.26, 0.74, 0.50, 0);
    Pad[32] = new TPad("smd_adc_n_ver11", "smd_adc_n_ver11", 0.74, 0.26, 0.98, 0.50, 0);
    // Fourth Line
    Pad[33] = new TPad("smd_adc_n_ver12", "smd_adc_n_ver12", 0.02, 0.02, 0.26, 0.26, 0);
    Pad[34] = new TPad("smd_adc_n_ver13", "smd_adc_n_ver13", 0.26, 0.02, 0.50, 0.26, 0);
    Pad[35] = new TPad("smd_adc_n_ver14", "smd_adc_n_ver14", 0.50, 0.02, 0.74, 0.26, 0);

    for (int i = 21; i < 36; i++)
    {
      Pad[i]->Draw();
    }

    // this one is used to plot the run number on the canvas
    transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(false);
  }

  else if (name == "SmdAdcSouthIndividual")
  {
    // xpos negative: do not draw menu bar
    TC[5] = new TCanvas(name.c_str(), "SMD ADC South Individual values", -1, ysize, xsize * 0.9, ysize * 0.9);
    gSystem->ProcessEvents();
    //  North Horizontal

    // xmin, ymin, xmax, ymax
    // First Line
    Pad[36] = new TPad("smd_adc_s_hor16", "smd_adc_s_hor16", 0.02, 0.74, 0.26, 0.98, 0);
    Pad[37] = new TPad("smd_adc_s_hor17", "smd_adc_s_hor17", 0.26, 0.74, 0.50, 0.98, 0);
    Pad[38] = new TPad("smd_adc_s_hor18", "smd_adc_s_hor18", 0.50, 0.74, 0.74, 0.98, 0);
    Pad[39] = new TPad("smd_adc_s_hor19", "smd_adc_s_hor19", 0.74, 0.74, 0.98, 0.98, 0);
    // Second Line
    Pad[40] = new TPad("smd_adc_s_hor20", "smd_adc_s_hor20", 0.02, 0.50, 0.26, 0.74, 0);
    Pad[41] = new TPad("smd_adc_s_hor21", "smd_adc_s_hor21", 0.26, 0.50, 0.50, 0.74, 0);
    Pad[42] = new TPad("smd_adc_s_hor22", "smd_adc_s_hor22", 0.50, 0.50, 0.74, 0.74, 0);
    Pad[43] = new TPad("smd_adc_s_hor23", "smd_adc_s_hor23", 0.74, 0.50, 0.98, 0.74, 0);
    // Third Line
    Pad[44] = new TPad("smd_adc_s_ver24", "smd_adc_s_ver24", 0.02, 0.26, 0.26, 0.50, 0);
    Pad[45] = new TPad("smd_adc_s_ver25", "smd_adc_s_ver25", 0.26, 0.26, 0.50, 0.50, 0);
    Pad[46] = new TPad("smd_adc_s_ver26", "smd_adc_s_ver26", 0.50, 0.26, 0.74, 0.50, 0);
    Pad[47] = new TPad("smd_adc_s_ver27", "smd_adc_s_ver27", 0.74, 0.26, 0.98, 0.50, 0);
    // Fourth Line
    Pad[48] = new TPad("smd_adc_s_ver28", "smd_adc_s_ver28", 0.02, 0.02, 0.26, 0.26, 0);
    Pad[49] = new TPad("smd_adc_s_ver29", "smd_adc_s_ver29", 0.26, 0.02, 0.50, 0.26, 0);
    Pad[50] = new TPad("smd_adc_s_ver30", "smd_adc_s_ver30", 0.50, 0.02, 0.74, 0.26, 0);

    for (int i = 36; i < 51; i++)
    {
      Pad[i]->Draw();
    }

    // this one is used to plot the run number on the canvas
    transparent[5] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[5]->SetFillStyle(4000);
    transparent[5]->Draw();
    TC[5]->SetEditable(false);
  }

  else if (name == "veto")
  {
    // xpos negative: do not draw menu bar
    TC[6] = new TCanvas(name.c_str(), "EXPERT - Veto counter", -1, ysize, xsize / 2, ysize / 2);
    gSystem->ProcessEvents();

    Pad[51] = new TPad("Veto_NF", "Veto_NF", 0.05, 0.5, 0.5, 0.98, 0);
    Pad[52] = new TPad("Veto_NB", "Veto_NB", 0.5, 0.5, 0.98, 0.98, 0);
    Pad[53] = new TPad("Veto_SF", "Veto_SF", 0.05, 0.05, 0.5, 0.5, 0);
    Pad[54] = new TPad("Veto_SB", "Veto_SB", 0.5, 0.05, 0.95, 0.5, 0);


    Pad[51]->Draw();
    Pad[52]->Draw();
    Pad[53]->Draw();
    Pad[54]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
    transparent[6]->SetFillStyle(4000);
    transparent[6]->Draw();
    TC[6]->SetEditable(false);
  }

  else if (name == "SmdMultiplicities")
  {
    // xpos negative: do not draw menu bar
    TC[7] = new TCanvas(name.c_str(), "EXPERT - Smd Multiplicities", -1, ysize, xsize / 2, ysize / 2);
    gSystem->ProcessEvents();
    Pad[55] = new TPad("smd_north_hor_hits", "smd_north_hor_hits", 0.05, 0.5, 0.5, 0.98, 0);
    Pad[56] = new TPad("smd_north_ver_hits", "smd_north_ver_hits", 0.5, 0.5, 0.98, 0.98, 0);
    Pad[57] = new TPad("smd_south_hor_hits", "smd_south_hor_hits", 0.05, 0.05, 0.5, 0.5, 0);
    Pad[58] = new TPad("smd_south_ver_hits", "smd_south_ver_hits", 0.5, 0.05, 0.95, 0.5, 0);

    Pad[55]->Draw();
    Pad[56]->Draw();
    Pad[57]->Draw();
    Pad[58]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
    transparent[7]->SetFillStyle(4000);
    transparent[7]->Draw();
    TC[7]->SetEditable(false);
  }

  else if (name == "waveform")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[8] = new TCanvas(name.c_str(), "ZDC Wave form", -1, -ysize, xsize * 0.9, ysize * 0.9);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();

    // float xshift = 0.04;

    Pad[59] = new TPad("zdcpad59", "who needs this?", 0.05, 0.5, 0.2, 0.9, 0);
    Pad[60] = new TPad("zdcpad60", "who needs this?", 0.25, 0.5, 0.25+0.15, 0.9, 0);
    Pad[61] = new TPad("zdcpad61", "who needs this?", 0.25+0.15+0.05, 0.5, 0.25+0.3+0.05, 0.9, 0);
    Pad[62] = new TPad("zdcpad62", "who needs this?", 0.25+0.3+0.05+0.05, 0.5, 0.25+0.45+0.05+0.05, 0.9, 0);
    Pad[63] = new TPad("zdcpad63", "who needs this?", 0.25+0.45+0.05+0.05+0.05, 0.5, 1, 0.9, 0);


    Pad[64] = new TPad("zdcpad64", "who needs this?", 0.05, 0.05, 0.2, 0.4, 0);
    Pad[65] = new TPad("zdcpad65", "who needs this?", 0.25, 0.05, 0.25+0.15, 0.4, 0);
    Pad[66] = new TPad("zdcpad66", "who needs this?", 0.25+0.15+0.05, 0.05, 0.25+0.3+0.05, 0.4, 0);
    Pad[67] = new TPad("zdcpad67", "who needs this?", 0.25+0.3+0.05+0.05, 0.05, 0.25+0.45+0.05+0.05, 0.4, 0);
    Pad[68] = new TPad("zdcpad68", "who needs this?", 0.25+0.45+0.05+0.05+0.05, 0.05, 1, 0.4, 0);


    Pad[59]->Draw();
    Pad[60]->Draw();
    Pad[61]->Draw();
    Pad[62]->Draw();
    Pad[63]->Draw();
    Pad[64]->Draw();
    Pad[65]->Draw();
    Pad[66]->Draw();
    Pad[67]->Draw();
    Pad[68]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[8] = new TPad("transparent8", "this does not show", 0, 0, 1, 1);
    transparent[8]->SetFillStyle(4000);
    transparent[8]->Draw();
    TC[8]->SetEditable(false);
  }



  return 0;
}

int ZdcMonDraw::Draw(const std::string &what)
{
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "FIRST")
  {
    iret += DrawFirst(what);
    idraw++;
  }
  if (what == "ALL" || what == "SECOND")
  {
    iret += DrawSecond(what);
    idraw++;
  }

  if (what == "ALL" || what == "SMDVALUES")
  {
    iret += DrawSmdValues(what);
    idraw++;
  }

  if (what == "ALL" || what == "SMDN&S")
  {
    iret += DrawSmdNorthandSouth(what);
    idraw++;
  }

  if (what == "ALL" || what == "SMD_N_IND")
  {
    iret += DrawSmdAdcNorthIndividual(what);
    idraw++;
  }

  if (what == "ALL" || what == "SMD_S_IND")
  {
    iret += DrawSmdAdcSouthIndividual(what);
    idraw++;
  }

  if (what == "ALL" || what == "VETO_COUNTER")
  {
    iret += Drawveto(what);
    idraw++;
  }

  if (what == "ALL" || what == "SMD_MULTIPLICITIES")
  {
     iret += DrawSmdMultiplicities(what);
     idraw++;
  }

  if (what == "ALL" || what == "ZDC_WAVEFORM")
  {
    iret += DrawWaveForm(what);
    idraw++;
  }



  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret--;
    idraw++;
  }
  if (std::fabs(iret) != idraw) // at least one succeeded
  {
    return 0;
  }
  return iret;
}

int ZdcMonDraw::DrawFirst(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *zdc_adc_south = cl->getHisto("ZDCMON_0", "zdc_adc_south");
  TH1 *zdc_adc_north = cl->getHisto("ZDCMON_0", "zdc_adc_north");
  TH2 *smd_xy_north = (TH2 *) cl->getHisto("ZDCMON_0", "smd_xy_north");
  TH2 *smd_xy_south = (TH2 *) cl->getHisto("ZDCMON_0", "smd_xy_south");

  if (!gROOT->FindObject("ZdcMon1"))
  {
    MakeCanvas("ZdcMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  Pad[0]->cd();
  gPad->SetLogy();
  //  gPad->SetLogx();
  if (zdc_adc_south)
  {
    //zdc_adc_south->Scale(1 / zdc_adc_south->Integral(), "width");
    zdc_adc_south->SetXTitle("ZDC South ADC Distribution");
    zdc_adc_south->SetYTitle("Counts");
    zdc_adc_south->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    if (isHtml())
    {
      delete TC[0];
      TC[0] = nullptr;
    }
    return -1;
  }
  Pad[1]->cd();
  gPad->SetLogy();
  //  gPad->SetLogx();
  if (zdc_adc_north)
  {
    //zdc_adc_north->Scale(1 / zdc_adc_north->Integral(), "width");
    zdc_adc_north->SetXTitle("ZDC North ADC Distribution");
    zdc_adc_north->SetYTitle("Counts");
    zdc_adc_north->DrawCopy();
  }

  Pad[2]->cd();
  if (smd_xy_south)
  {
    smd_xy_south->SetXTitle("SMD south hit position in x [cm]");
    smd_xy_south->SetYTitle("SMD south hit position in y [cm]");
    smd_xy_south->DrawCopy("colz");

  }
  Pad[3]->cd();
  if (smd_xy_north)
  {
    smd_xy_north->SetXTitle("SMD north hit position in x [cm]");
    smd_xy_north->SetYTitle("SMD north hit position in y [cm]");
    smd_xy_north->DrawCopy("colz");
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);

  return 0;
}

int ZdcMonDraw::DrawSecond(const std::string & /* what */)
{

  int textsize = 22;
  TString zdclabels[6] = {"ZDCS1 ADC","ZDCS2 ADC","ZDCS3 ADC", "ZDCN1 ADC", "ZDCN2 ADC", "ZDCN3 ADC"};
  TLatex l[6];

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *zdc_S1 = cl->getHisto("ZDCMON_0", "zdc_S1");
  TH1 *zdc_S2 = cl->getHisto("ZDCMON_0", "zdc_S2");
  TH1 *zdc_S3 = cl->getHisto("ZDCMON_0", "zdc_S3");
  TH1 *zdc_N1 = cl->getHisto("ZDCMON_0", "zdc_N1");
  TH1 *zdc_N2 = cl->getHisto("ZDCMON_0", "zdc_N2");
  TH1 *zdc_N3 = cl->getHisto("ZDCMON_0", "zdc_N3");

  if (!gROOT->FindObject("ZdcMon2"))
  {
    MakeCanvas("ZdcMon2");
  }


  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[4]->cd();
  gPad->SetLogy();
  if (zdc_S1)
  {
      zdc_S1->SetStats(0);
      l[0].SetNDC();
      l[0].SetTextFont(43);
      l[0].SetTextSize(textsize);
      zdc_S1->SetXTitle("ZDC South First Module ADC Distribution");
      zdc_S1->SetYTitle("Counts");
      zdc_S1->DrawCopy();
      l[0].DrawLatex(0.15, 0.75, zdclabels[0]);
  }
  else
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    if (isHtml())
    {
      delete TC[1];
      TC[1] = nullptr;
    }
    return -1;
  }

  Pad[5]->cd();
  gPad->SetLogy();
  if (zdc_S2)
  {
      zdc_S2->SetStats(0);
      l[1].SetNDC();
      l[1].SetTextFont(43);
      l[1].SetTextSize(textsize);
      zdc_S2->SetXTitle("ZDC South Second Module ADC Distribution");
      zdc_S2->SetYTitle("Counts");
      zdc_S2->DrawCopy();
      l[1].DrawLatex(0.15, 0.75, zdclabels[1]);
  }
  Pad[6]->cd();
  gPad->SetLogy();
  if (zdc_S3)
  {

      zdc_S3->SetStats(0);
      l[2].SetNDC();
      l[2].SetTextFont(43);
      l[2].SetTextSize(textsize);
      zdc_S3->SetXTitle("ZDC South Third Module ADC Distribution");
      zdc_S3->SetYTitle("Counts");
      zdc_S3->DrawCopy();
      l[2].DrawLatex(0.15, 0.75, zdclabels[2]);

  }

  Pad[7]->cd();
  gPad->SetLogy();
  if (zdc_N1)
  {
     zdc_N1->SetStats(0);
     l[3].SetNDC();
     l[3].SetTextFont(43);
     l[3].SetTextSize(textsize);
     zdc_N1->SetXTitle("ZDC North First Module ADC Distribution");
     zdc_N1->SetYTitle("Counts");
     zdc_N1->DrawCopy();
     l[3].DrawLatex(0.15, 0.75, zdclabels[3]);
  }

  Pad[8]->cd();
  gPad->SetLogy();
  if (zdc_N2)
  {
     zdc_N2->SetStats(0);
     l[4].SetNDC();
     l[4].SetTextFont(43);
     l[4].SetTextSize(textsize);
     zdc_N2->SetXTitle("ZDC North Second Module ADC Distribution");
     zdc_N2->SetYTitle("Counts");
     zdc_N2->DrawCopy();
     l[4].DrawLatex(0.15, 0.75, zdclabels[4]);
  }

  Pad[9]->cd();
  gPad->SetLogy();
  if (zdc_N3)
  {
     zdc_N3->SetStats(0);
     l[5].SetNDC();
     l[5].SetTextFont(43);
     l[5].SetTextSize(textsize);
     zdc_N3->SetXTitle("ZDC North Third Module ADC Distribution");
     zdc_N3->SetYTitle("Counts");
     zdc_N3->DrawCopy();
     l[5].DrawLatex(0.15, 0.75, zdclabels[5]);
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);

  return 0;
}

int ZdcMonDraw::DrawSmdValues(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *smd_value = (TH2 *) cl->getHisto("ZDCMON_0", "smd_value");
  TH2 *smd_value_good = (TH2 *) cl->getHisto("ZDCMON_0", "smd_value_good");
  TH2 *smd_value_small = (TH2 *) cl->getHisto("ZDCMON_0", "smd_value_small");
  if (!gROOT->FindObject("SmdValues"))
  {
    MakeCanvas("SmdValues");
  }

  TC[2]->SetEditable(true);
  TC[2]->Clear("D");
  Pad[10]->cd();
  if (smd_value)
  {
    smd_value->SetXTitle("ADC");
    smd_value->SetYTitle("Channels");
    smd_value->SetStats(0);
    smd_value->DrawCopy("col");

  }
  else
  {
    DrawDeadServer(transparent[2]);
    TC[2]->SetEditable(false);
    if (isHtml())
    {
      delete TC[2];
      TC[2] = nullptr;
    }
    return -1;
  }

  Pad[11]->cd();
  if (smd_value_good)
  {
    smd_value_good->SetXTitle("ADC");
    smd_value_good->SetYTitle("Channels");
    smd_value_good->SetStats(0);
    smd_value_good->DrawCopy("col");
  }
  Pad[12]->cd();
  if (smd_value_small)
  {
    smd_value_small->SetXTitle("ADC");
    smd_value_small->SetYTitle("Channels");
    smd_value_small->SetStats(0);
    smd_value_small->DrawCopy("col");
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[2]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(false);

  return 0;
}

int ZdcMonDraw::DrawSmdNorthandSouth(const std::string & /* what */)
{
  int textsize = 12;
  TLatex l[12];
  float _low = 0.68;

  OnlMonClient *cl = OnlMonClient::instance();

  // get pointer for each histogram
  TH1 *smd_hor_south = cl->getHisto("ZDCMON_0", "smd_hor_south");
  TH1 *smd_ver_south = cl->getHisto("ZDCMON_0", "smd_ver_south");
  TH1 *smd_hor_north = cl->getHisto("ZDCMON_0", "smd_hor_north");
  TH1 *smd_ver_north = cl->getHisto("ZDCMON_0", "smd_ver_north");

  TH1 *smd_hor_north_good = cl->getHisto("ZDCMON_0", "smd_hor_north_good");
  TH1 *smd_ver_north_good = cl->getHisto("ZDCMON_0", "smd_ver_north_good");
  TH1 *smd_hor_south_good = cl->getHisto("ZDCMON_0", "smd_hor_south_good");
  TH1 *smd_ver_south_good = cl->getHisto("ZDCMON_0", "smd_ver_south_good");


  TH1 *smd_sum_hor_south = cl->getHisto("ZDCMON_0", "smd_sum_hor_south");
  TH1 *smd_sum_ver_south = cl->getHisto("ZDCMON_0", "smd_sum_ver_south");
  TH1 *smd_sum_hor_north = cl->getHisto("ZDCMON_0", "smd_sum_hor_north");
  TH1 *smd_sum_ver_north = cl->getHisto("ZDCMON_0", "smd_sum_ver_north");

  if (!gROOT->FindObject("SmdNorthandSouth"))
  {
    MakeCanvas("SmdNorthandSouth");
  }
  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  Pad[13]->cd();

  if (smd_ver_north_good)
  {
    l[8].SetNDC();
    l[8].SetTextFont(43);
    l[8].SetTextSize(textsize);
    smd_ver_north_good->SetXTitle("SMD North ADC weighted hit position x [cm]");
    smd_ver_north_good->SetYTitle("Counts");
    smd_ver_north_good->DrawCopy();
    l[8].DrawLatex(0.15, 0.75, "SMDN position in x[cm]");
    l[8].DrawLatex(0.15, _low, "(ZDC1, ZDC2, Veto ADC cut)");
  }
  else
  {
    DrawDeadServer(transparent[3]);
    TC[3]->SetEditable(false);
    if (isHtml())
    {
      delete TC[3];
      TC[3] = nullptr;
    }
    return -1;
  }
  Pad[14]->cd();
  if (smd_hor_north_good)
  {
    l[9].SetNDC();
    l[9].SetTextFont(43);
    l[9].SetTextSize(textsize);
    smd_hor_north_good->SetXTitle("SMD North ADC weighted hit position y [cm]");
    smd_hor_north_good->SetYTitle("Counts");
    smd_hor_north_good->DrawCopy();
    l[9].DrawLatex(0.15, 0.75, "SMDN position in y[cm]");
    l[9].DrawLatex(0.15, _low, "(ZDC1, ZDC2, Veto ADC cut)");

  }

    Pad[15]->cd();
    if (smd_ver_south_good)
    {
      l[10].SetNDC();
      l[10].SetTextFont(43);
      l[10].SetTextSize(textsize);
      smd_ver_south_good->SetXTitle("SMD South ADC weighted hit position x [cm]");
      smd_ver_south_good->SetYTitle("Counts");
      smd_ver_south_good->DrawCopy();
      l[10].DrawLatex(0.15, 0.75, "SMDS position in x[cm]");
      l[10].DrawLatex(0.15, _low, "(ZDC1, ZDC2, Veto ADC cut)");

    }
    Pad[16]->cd();
    if (smd_hor_south_good)
    {
      l[11].SetNDC();
      l[11].SetTextFont(43);
      l[11].SetTextSize(textsize);
      smd_hor_south_good->SetXTitle("SMD South ADC weighted hit position y [cm]");
      smd_hor_south_good->SetYTitle("Counts");
      smd_hor_south_good->DrawCopy();
      l[11].DrawLatex(0.15, 0.75, "SMDS position in y[cm] ");
      l[11].DrawLatex(0.15, _low, "(ZDC1, ZDC2, Veto ADC cut)");
    }

  // VERTICAL AND HORIZONTAL NORTH
  Pad[17]->cd();
  if (smd_ver_north)
  {
     l[4].SetNDC();
     l[4].SetTextFont(43);
     l[4].SetTextSize(textsize);
     smd_ver_north->SetXTitle("SMD North ADC weighted hit position x [cm]");
     smd_ver_north->SetYTitle("Counts");
     smd_ver_north->DrawCopy();
     l[4].DrawLatex(0.15, 0.75, "SMDN position in x[cm]");
     l[4].DrawLatex(0.15, _low, "(no ZDC ADC selection)");

  }
  Pad[18]->cd();
  if (smd_hor_north)
  {
      l[5].SetNDC();
      l[5].SetTextFont(43);
      l[5].SetTextSize(textsize);
      smd_hor_north->SetXTitle("SMD North ADC weighted hit position y [cm]");
      smd_hor_north->SetYTitle("Counts");
      smd_hor_north->DrawCopy();
      l[5].DrawLatex(0.15, 0.75, "SMDN position in y[cm]");
      l[5].DrawLatex(0.15, _low, "(no ZDC ADC selection)");

  }

  // VERTICAL AND HORIZONTAL SOUTH (good and small do not exist for south)
  Pad[19]->cd();
  if (smd_ver_south)
  {
     l[6].SetNDC();
     l[6].SetTextFont(43);
     l[6].SetTextSize(textsize);
     smd_ver_south->SetXTitle("SMD South ADC weighted hit position x [cm]");
     smd_ver_south->SetYTitle("Counts");
     smd_ver_south->DrawCopy();
     l[6].DrawLatex(0.15, 0.75, "SMDS position in x[cm]");
     l[6].DrawLatex(0.15, _low, "(no ZDC ADC selection)");
  }
  Pad[20]->cd();
  if (smd_hor_south)
  {
      l[7].SetNDC();
      l[7].SetTextFont(43);
      l[7].SetTextSize(textsize);
      smd_hor_south->SetXTitle("SMD South ADC weighted hit position y [cm]");
      smd_hor_south->SetYTitle("Counts");
      smd_hor_south->DrawCopy();
      l[7].DrawLatex(0.15, 0.75, "SMDS position in y[cm]");
      l[7].DrawLatex(0.15, _low, "(no ZDC ADC selection)");


  }

  // SUMS
  Pad[21]->cd();
  if (smd_sum_ver_north)
  {
     l[0].SetNDC();
     l[0].SetTextFont(43);
     l[0].SetTextSize(textsize);
     smd_sum_ver_north->GetXaxis()->SetRangeUser(0,800);
     smd_sum_ver_north->SetXTitle("SMD x-sum North ADC distribution");
     smd_sum_ver_north->SetYTitle("Counts");
     smd_sum_ver_north->DrawCopy();
     l[0].DrawLatex(0.15, 0.75, "SMD North ADC x-sum");
  }
  Pad[22]->cd();
  if (smd_sum_hor_north)
  {
     l[1].SetNDC();
     l[1].SetTextFont(43);
     l[1].SetTextSize(textsize);
     smd_sum_hor_north->GetXaxis()->SetRangeUser(0,800);
     smd_sum_hor_north->SetXTitle("SMD y-sum North ADC distribution");
     smd_sum_hor_north->SetYTitle("Counts");
     smd_sum_hor_north->DrawCopy();
     l[1].DrawLatex(0.15, 0.75, "SMD North ADC y-sum");
  }
  Pad[23]->cd();
  if (smd_sum_ver_south)
  {
     l[2].SetNDC();
     l[2].SetTextFont(43);
     l[2].SetTextSize(textsize);
     smd_sum_ver_south->GetXaxis()->SetRangeUser(0,800);
     smd_sum_ver_south->SetXTitle("SMD x-sum South ADC distribution");
     smd_sum_ver_south->SetYTitle("Counts");
     smd_sum_ver_south->DrawCopy();
     l[2].DrawLatex(0.15, 0.75, "SMD South ADC x-sum");
 }
  Pad[24]->cd();
  if (smd_sum_hor_south)
  {
      l[3].SetNDC();
      l[3].SetTextFont(43);
      l[3].SetTextSize(textsize);
      smd_sum_hor_south->GetXaxis()->SetRangeUser(0,800);
      smd_sum_hor_south->SetXTitle("SMD y-sum South ADC distribution");
      smd_sum_hor_south->SetYTitle("Counts");
      smd_sum_hor_south->DrawCopy();
      l[3].DrawLatex(0.15, 0.75, "SMD South ADC y-sum");
 }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_4 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[3]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);
  return 0;
}

int ZdcMonDraw::DrawSmdAdcNorthIndividual(const std::string & /* what */)
{

    TString smdnorthlabel[15] = {"SMDNH1 ADC","SMDNH2 ADC","SMDNH3 ADC","SMDNH4 ADC","SMDNH5 ADC","SMDNH6 ADC","SMDNH7 ADC", "SMDNH8 ADC","SMDNV1 ADC","SMDNV2 ADC","SMDNV3 ADC","SMDNV4 ADC","SMDNV5 ADC","SMDNV6 ADC", "SMDNV7 ADC"};
    TLatex l[15];

    int textsize = 18;
    TString meantrunc[15];


  OnlMonClient *cl = OnlMonClient::instance();

  // Array that holds pointer to the histogram of each channel
  TH1 *smd_adc_n_hor_ind[8];
  TH1 *smd_adc_n_ver_ind[7];
  // Horizontal
  for (int i = 0; i < 8; ++i)
  {
    smd_adc_n_hor_ind[i] = (TH1 *) cl->getHisto("ZDCMON_0", Form("smd_adc_n_hor_ind%d", i));  // Retrieve histogram pointer using 'histName'
  }
  // Vertical
  for (int i = 0; i < 7; ++i)
  {
    smd_adc_n_ver_ind[i] = (TH1 *) cl->getHisto("ZDCMON_0", Form("smd_adc_n_ver_ind%d", i));  // Retrieve histogram pointer using 'histName'
  }


  if (!gROOT->FindObject("SmdAdcNorthIndividual"))
  {
    MakeCanvas("SmdAdcNorthIndividual");
  }

  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  Pad[21]->cd();
  gPad->SetLogy(1);

  if (smd_adc_n_hor_ind[0])
  {
      float m = smd_adc_n_hor_ind[0]->GetMean();
      meantrunc[0].Append(Form("%.2f", m));
      smd_adc_n_hor_ind[0]->SetStats(0);
      l[0].SetNDC();
      l[0].SetTextFont(43);
      l[0].SetTextSize(textsize);
      smd_adc_n_hor_ind[0]->SetYTitle("Counts");
      smd_adc_n_hor_ind[0]->SetXTitle("ADC");
      smd_adc_n_hor_ind[0]->DrawCopy();
      l[0].DrawLatex(0.15, 0.75, smdnorthlabel[0]);
      l[0].DrawLatex(0.15, 0.65, "mean = " + meantrunc[0]);
  }
  else
  {
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    if (isHtml())
    {
      delete TC[4];
      TC[4] = nullptr;
    }
    return -1;
  }


  for (int i = 1; i < 8; ++i)
  {
    Pad[21 + i]->cd();
    gPad->SetLogy(1);
    if (smd_adc_n_hor_ind[i])
    {
        float m = smd_adc_n_hor_ind[i]->GetMean();
        meantrunc[i].Append(Form("%.2f", m));
        smd_adc_n_hor_ind[i]->SetStats(0);
        l[i].SetNDC();
        l[i].SetTextFont(43);
        l[i].SetTextSize(textsize);
        smd_adc_n_hor_ind[i]->SetYTitle("Counts");
        smd_adc_n_hor_ind[i]->SetXTitle("ADC");
        smd_adc_n_hor_ind[i]->DrawCopy();
        l[i].DrawLatex(0.15, 0.75, smdnorthlabel[i]);
        l[i].DrawLatex(0.15, 0.65, "mean = " + meantrunc[i]);

    }
  }


  for (int i = 0; i < 7; ++i)
  {
    Pad[29 + i]->cd();
    gPad->SetLogy(1);
    if (smd_adc_n_ver_ind[i])
    {
        float m = smd_adc_n_ver_ind[i]->GetMean();
        meantrunc[i+8].Append(Form("%.2f", m));
        smd_adc_n_ver_ind[i]->SetStats(0);
        l[i+8].SetNDC();
        l[i+8].SetTextFont(43);
        l[i+8].SetTextSize(textsize);
        smd_adc_n_ver_ind[i]->SetYTitle("Counts");
        smd_adc_n_ver_ind[i]->SetXTitle("ADC");
        smd_adc_n_ver_ind[i]->DrawCopy();
        l[i+8].DrawLatex(0.15, 0.75, smdnorthlabel[i+8]);
        l[i].DrawLatex(0.15, 0.65, "mean = " + meantrunc[i+8]);
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_5 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[4]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);
  return 0;
}

int ZdcMonDraw::DrawSmdAdcSouthIndividual(const std::string & /* what */)
{

  TString smdsouthlabel[15] = {"SMDSH1 ADC","SMDSH2 ADC","SMDSH3 ADC","SMDSH4 ADC","SMDSH5 ADC","SMDSH6 ADC","SMDSH7 ADC", "SMDSH8 ADC","SMDSV1 ADC","SMDSV2 ADC","SMDSV3 ADC","SMDSV4 ADC","SMDSV5 ADC","SMDSV6 ADC", "SMDSV7 ADC"};
  TLatex l[15];

  int textsize = 18;
  TString meantrunc[15];

  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *smd_adc_s_hor_ind[8];
  TH1 *smd_adc_s_ver_ind[7];
  // Horizontal
  for (int i = 0; i < 8; ++i)
  {
    smd_adc_s_hor_ind[i] = (TH1 *) cl->getHisto("ZDCMON_0", Form("smd_adc_s_hor_ind%d", i));
  }
  // Vertical
  for (int i = 0; i < 7; ++i)
  {
    smd_adc_s_ver_ind[i] = (TH1 *) cl->getHisto("ZDCMON_0", Form("smd_adc_s_ver_ind%d", i));
  }

  if (!gROOT->FindObject("SmdAdcSouthIndividual"))
  {
    MakeCanvas("SmdAdcSouthIndividual");
  }

  TC[5]->SetEditable(true);
  TC[5]->Clear("D");
  Pad[36]->cd();
  gPad->SetLogy(1);
  if (smd_adc_s_hor_ind[0])
  {
      float m = smd_adc_s_hor_ind[0]->GetMean();
      meantrunc[0].Append(Form("%.2f", m));
      smd_adc_s_hor_ind[0]->SetStats(0);
      l[0].SetNDC();
      l[0].SetTextFont(43);
      l[0].SetTextSize(textsize);
      smd_adc_s_hor_ind[0]->SetYTitle("Counts");
      smd_adc_s_hor_ind[0]->SetXTitle("ADC");
      smd_adc_s_hor_ind[0]->DrawCopy();
      l[0].DrawLatex(0.15, 0.75, smdsouthlabel[0]);
      l[0].DrawLatex(0.15, 0.65, "mean = " + meantrunc[0]);
  }
  else
  {
    DrawDeadServer(transparent[5]);
    TC[5]->SetEditable(false);
    if (isHtml())
    {
      delete TC[5];
      TC[5] = nullptr;
    }
    return -1;
  }

  for (int i = 1; i < 8; ++i)
  {
    Pad[36 + i]->cd();
    gPad->SetLogy(1);
    if (smd_adc_s_hor_ind[i])
    {
        float m = smd_adc_s_hor_ind[i]->GetMean();
        meantrunc[i].Append(Form("%.2f", m));
        smd_adc_s_hor_ind[i]->SetStats(0);
        l[i].SetNDC();
        l[i].SetTextFont(43);
        l[i].SetTextSize(textsize);
        smd_adc_s_hor_ind[i]->SetYTitle("Counts");
        smd_adc_s_hor_ind[i]->SetXTitle("ADC");
        smd_adc_s_hor_ind[i]->DrawCopy();
        l[i].DrawLatex(0.15, 0.75, smdsouthlabel[i]);
        l[i].DrawLatex(0.15, 0.65, "mean = " + meantrunc[i]);
    }
  }

  for (int i = 0; i < 7; ++i)
  {
    Pad[44 + i]->cd();
    gPad->SetLogy(1);
    if (smd_adc_s_ver_ind[i])
    {
        float m = smd_adc_s_ver_ind[i]->GetMean();
        meantrunc[i+8].Append(Form("%.2f", m));
        smd_adc_s_ver_ind[i]->SetStats(0);
        l[i+8].SetNDC();
        l[i+8].SetTextFont(43);
        l[i+8].SetTextSize(textsize);
        smd_adc_s_ver_ind[i]->SetYTitle("Counts");
        smd_adc_s_ver_ind[i]->SetXTitle("ADC");
        smd_adc_s_ver_ind[i]->DrawCopy();
        l[i+8].DrawLatex(0.15, 0.75, smdsouthlabel[i+8]);
        l[i].DrawLatex(0.15, 0.65, "mean = " + meantrunc[i+8]);

    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_6 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[5]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[5]->Update();
  TC[5]->Show();
  TC[5]->SetEditable(false);
  return 0;
}

int ZdcMonDraw::Drawveto(const std::string & /* what */)
{

  TString vnmame[4] = {"Veto North Front", "Veto North Back","Veto South Front","Veto South Back"};
  TLatex l[4];
  int textsize = 15;

  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *veto_NF = (TH1 *) cl->getHisto("ZDCMON_0", "veto_NF");
  TH1 *veto_NB = (TH1 *) cl->getHisto("ZDCMON_0", "veto_NB");
  TH1 *veto_SF = (TH1 *) cl->getHisto("ZDCMON_0", "veto_SF");
  TH1 *veto_SB = (TH1 *) cl->getHisto("ZDCMON_0", "veto_SB");


  if (!gROOT->FindObject("veto"))
  {
    MakeCanvas("veto");
  }


  TC[6]->SetEditable(true);
  TC[6]->Clear("D");
  Pad[51]->cd();
  gPad->SetLogy();
  if (veto_NF)
  {
      l[0].SetNDC();
      l[0].SetTextFont(43);
      l[0].SetTextSize(textsize);
       veto_NF->DrawCopy();
       l[0].DrawLatex(0.15, 0.75, vnmame[0]);

  }
  else
  {
    DrawDeadServer(transparent[6]);
    TC[6]->SetEditable(false);
    if (isHtml())
    {
      delete TC[6];
      TC[6] = nullptr;
    }
    return -1;
  }


  Pad[52]->cd();
  gPad->SetLogy();
  if (veto_NB)
  {
    l[1].SetNDC();
    l[1].SetTextFont(43);
    l[1].SetTextSize(textsize);
     veto_NB->DrawCopy();
   l[1].DrawLatex(0.15, 0.75, vnmame[1]);

  }
  Pad[53]->cd();
  gPad->SetLogy();
  if (veto_SF)
  {
    l[2].SetNDC();
    l[2].SetTextFont(43);
    l[2].SetTextSize(textsize);
    veto_SF->DrawCopy();
    l[2].DrawLatex(0.15, 0.75, vnmame[2]);

  }
  Pad[54]->cd();
  gPad->SetLogy();
  if (veto_SB)
  {
    l[3].SetNDC();
    l[3].SetTextFont(43);
    l[3].SetTextSize(textsize);
    veto_SB->DrawCopy();
    l[3].DrawLatex(0.15, 0.75, vnmame[3]);

  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_7 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[6]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[6]->Update();
  TC[6]->Show();
  TC[6]->SetEditable(false);

  return 0;
}

int ZdcMonDraw::DrawSmdMultiplicities(const std::string & /* what */)
{

 TString Mult[4] = {"SMD North y mult", "SMD North x mult","SMD South y mult","SMD South x mult"};
 TLatex l[4];
  int textsize = 15;

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *smd_north_hor_hits = (TH1 *) cl->getHisto("ZDCMON_0", "smd_north_hor_hits");
  TH1 *smd_north_ver_hits = (TH1 *) cl->getHisto("ZDCMON_0", "smd_north_ver_hits");
  TH1 *smd_south_hor_hits = (TH1 *) cl->getHisto("ZDCMON_0", "smd_south_hor_hits");
  TH1 *smd_south_ver_hits = (TH1 *) cl->getHisto("ZDCMON_0", "smd_south_ver_hits");

  if (!gROOT->FindObject("SmdMultiplicities"))
  {
    MakeCanvas("SmdMultiplicities");
  }

    TC[7]->SetEditable(true);
    TC[7]->Clear("D");
    Pad[55]->cd();
    if (smd_north_hor_hits)
    {
        l[0].SetNDC();
        l[0].SetTextFont(43);
        l[0].SetTextSize(textsize);
        smd_north_hor_hits->SetStats(0);
        smd_north_hor_hits->SetXTitle("SMD North Horizontal Multiplicities");
        smd_north_hor_hits->SetYTitle("Counts");
        smd_north_hor_hits->DrawCopy();
        l[0].DrawLatex(0.15, 0.75, Mult[0]);

    }
    else
    {
      DrawDeadServer(transparent[7]);
      TC[7]->SetEditable(false);
      if (isHtml())
      {
        delete TC[8];
        TC[8] = nullptr;
      }
      return -1;
    }

    Pad[56]->cd();
    gPad->SetLogy();
    if (smd_north_ver_hits)
    {
      l[1].SetNDC();
      l[1].SetTextFont(43);
      l[1].SetTextSize(textsize);
      smd_north_ver_hits->SetStats(0);
      smd_north_ver_hits->SetXTitle("SMD North Vertical Multiplicities");
      smd_north_ver_hits->SetYTitle("Counts");
      smd_north_ver_hits->DrawCopy();
      l[1].DrawLatex(0.15, 0.75, Mult[1]);
    }

    Pad[57]->cd();
    gPad->SetLogy();
    if (smd_south_hor_hits)
    {
      l[2].SetNDC();
      l[2].SetTextFont(43);
      l[2].SetTextSize(textsize);
      smd_south_hor_hits->SetStats(0);
      smd_south_hor_hits->SetXTitle("SMD South Horizontal Multiplicities");
      smd_south_hor_hits->SetYTitle("Counts");
      smd_south_hor_hits->DrawCopy();
      l[2].DrawLatex(0.15, 0.75, Mult[2]);
    }

    Pad[58]->cd();
    gPad->SetLogy();
    if (smd_south_ver_hits)
    {
      l[3].SetNDC();
      l[3].SetTextFont(43);
      l[3].SetTextSize(textsize);
      smd_south_ver_hits->SetStats(0);
      smd_south_ver_hits->SetXTitle("SMD South Vertical Multiplicities");
      smd_south_ver_hits->SetYTitle("Counts");
      smd_south_ver_hits->DrawCopy();
      l[3].DrawLatex(0.15, 0.75, Mult[3]);

    }


  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_7 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[7]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[7]->Update();
  TC[7]->Show();
  TC[7]->SetEditable(false);

  return 0;
}


int ZdcMonDraw::DrawWaveForm(const std::string & /* what */)
{

  float offset = 1.7;
  float lf = 0.16;
  float  maxwf = 20000.;

 OnlMonClient *cl = OnlMonClient::instance();

 TH2 *h_waveformZDC = (TH2F *) cl->getHisto("ZDCMON_0", "h_waveformZDC");
 TH1F* h_waveform_timez = (TH1F*) cl->getHisto("ZDCMON_0", "h_waveform_timez");
 TH2 *h_waveformSMD_North = (TH2F *) cl->getHisto("ZDCMON_0", "h_waveformSMD_North");
 TH1F* h_waveform_timesn = (TH1F*) cl->getHisto("ZDCMON_0", "h_waveform_timesn");
 TH2 *h_waveformSMD_South = (TH2F *) cl->getHisto("ZDCMON_0", "h_waveformSMD_South");
 TH2 *h_waveformVeto_North = (TH2F *) cl->getHisto("ZDCMON_0", "h_waveformVeto_North");
 TH2 *h_waveformVeto_South = (TH2F *) cl->getHisto("ZDCMON_0", "h_waveformVeto_South");
 TH1F* h_waveform_timess = (TH1F*) cl->getHisto("ZDCMON_0", "h_waveform_timess");
 TH1F* h_waveform_timevs = (TH1F*) cl->getHisto("ZDCMON_0", "h_waveform_timevs");
 TH1F* h_waveform_timevn = (TH1F*) cl->getHisto("ZDCMON_0", "h_waveform_timevn");

  if (!gROOT->FindObject("waveform"))
  {
    MakeCanvas("waveform");
  }
  TC[8]->SetEditable(true);
  TC[8]->Clear("D");
  if (!h_waveformZDC)
  {
    DrawDeadServer(transparent[8]);
    TC[8]->SetEditable(false);
    if (isHtml())
    {
      delete TC[8];
      TC[8] = nullptr;
    }
    return -1;
  }


  // --- widening window for ZDC from 5-9 to 4-10
  TLine *t1 = new TLine(4,0.0,4,maxwf);
  t1->SetLineWidth(3);
  t1->SetLineStyle(1);
  t1->SetLineColor(kRed);

  TLine *t2 = new TLine(10,0.0,10,maxwf);
  t2->SetLineWidth(3);
  t2->SetLineStyle(1);
  t2->SetLineColor(kRed);

  Pad[59]->cd();
  gPad->SetLogz();
  gPad->SetRightMargin(0.15);
  gPad->SetLeftMargin(lf);

  gStyle->SetTitleFontSize(0.08);
  h_waveformZDC->GetYaxis()->SetRangeUser(0, h_waveformZDC->GetMaximum() * 1.05);
  h_waveformZDC->Draw("colz");

  float tsize = 0.05;
  h_waveformZDC->SetStats(0);
  h_waveformZDC->GetXaxis()->SetNdivisions(510, kTRUE);
  h_waveformZDC->GetXaxis()->SetRangeUser(0, 16);
  h_waveformZDC->SetTitle("ZDC");
  h_waveformZDC->SetYTitle("Waveform [ADC]");
  h_waveformZDC->SetXTitle("Sample Number");
  h_waveformZDC->GetXaxis()->SetLabelSize(tsize);
  h_waveformZDC->GetYaxis()->SetLabelSize(tsize);
  h_waveformZDC->GetXaxis()->SetTitleSize(tsize);
  h_waveformZDC->GetYaxis()->SetTitleSize(tsize);
  h_waveformZDC->GetYaxis()->SetTitleOffset(offset);
  t1->Draw("same");
  t2->Draw("same");

  Pad[64]->cd();
  gPad->SetLeftMargin(lf);

  gStyle->SetTitleFontSize(0.06);
  h_waveform_timez->Draw("hist");
  h_waveform_timez->SetTitle("");
  h_waveform_timez->SetStats(0);
  h_waveform_timez->GetXaxis()->SetNdivisions(510, kTRUE);
  h_waveform_timez->GetXaxis()->SetRangeUser(0, 16);
  h_waveform_timez->SetTitle("ZDC");
  h_waveform_timez->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_timez->GetXaxis()->SetTitle("Sample Number");
  h_waveform_timez->GetXaxis()->SetLabelSize(tsize);
  h_waveform_timez->GetYaxis()->SetLabelSize(tsize);
  h_waveform_timez->GetXaxis()->SetTitleSize(tsize);
  h_waveform_timez->GetYaxis()->SetTitleSize(tsize);
  h_waveform_timez->GetYaxis()->SetTitleOffset(offset);

  h_waveform_timez->SetFillColorAlpha(kBlue, 0.1);
  if(h_waveform_timez->GetEntries()){
     h_waveform_timez->Scale(1. / h_waveform_timez->GetEntries());
   }

  // --- widening window for ZDC from 5-9 to 4-10
  TLine *t11 = new TLine(4,0.0,4,h_waveform_timez->GetMaximum() * 1.05);
  t11->SetLineWidth(3);
  t11->SetLineStyle(1);
  t11->SetLineColor(kRed);

  TLine *t22 = new TLine(10,0.0,10,h_waveform_timez->GetMaximum() * 1.05);
  t22->SetLineWidth(3);
  t22->SetLineStyle(1);
  t22->SetLineColor(kRed);
  t11->Draw("same");
  t22->Draw("same");


  // --- widening window for SMD north from 9-14 to 7-14
  // --- changing 7-14 to 8-14 for run25pp
    TLine *t3 = new TLine(8,0.0,8,  maxwf);
    t3->SetLineWidth(3);
    t3->SetLineStyle(1);
    t3->SetLineColor(kRed);

    TLine *t4 = new TLine(14,0.0,14, maxwf);
    t4->SetLineWidth(3);
    t4->SetLineStyle(1);
    t4->SetLineColor(kRed);

    Pad[60]->cd();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.08);
    h_waveformSMD_North->GetYaxis()->SetRangeUser(0, h_waveformSMD_North->GetMaximum() * 1.05);
    h_waveformSMD_North->Draw("colz");

    h_waveformSMD_North->SetStats(0);
    h_waveformSMD_North->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveformSMD_North->GetXaxis()->SetRangeUser(0, 16);
    h_waveformSMD_North->SetTitle("SMD NORTH");
    h_waveformSMD_North->SetXTitle("Sample Number");
    h_waveformSMD_North->SetYTitle("Waveform [ADC]");
    h_waveformSMD_North->GetXaxis()->SetLabelSize(tsize);
    h_waveformSMD_North->GetYaxis()->SetLabelSize(tsize);
    h_waveformSMD_North->GetXaxis()->SetTitleSize(tsize);
    h_waveformSMD_North->GetYaxis()->SetTitleSize(tsize);
    h_waveformSMD_North->GetYaxis()->SetTitleOffset(offset);

    t3->Draw("same");
    t4->Draw("same");

    Pad[65]->cd();
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.06);
    h_waveform_timesn->Draw("hist");
    h_waveform_timesn->SetTitle("");
    // h_waveform_timesn->GetXaxis()->CenterTitle();
    h_waveform_timesn->SetStats(0);
    h_waveform_timesn->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveform_timesn->GetXaxis()->SetRangeUser(0, 16);
    h_waveform_timesn->SetTitle("SMD NORTH");
    h_waveform_timesn->GetXaxis()->SetTitle("Sample Number");
    h_waveform_timesn->GetYaxis()->SetTitle("Fraction of towers");
    h_waveform_timesn->GetXaxis()->SetLabelSize(tsize);
    h_waveform_timesn->GetYaxis()->SetLabelSize(tsize);
    h_waveform_timesn->GetXaxis()->SetTitleSize(tsize);
    h_waveform_timesn->GetYaxis()->SetTitleSize(tsize);
    h_waveform_timesn->GetYaxis()->SetTitleOffset(offset);


    h_waveform_timesn->SetFillColorAlpha(kBlue, 0.1);
    if(h_waveform_timesn->GetEntries()){
        h_waveform_timesn->Scale(1. / h_waveform_timesn->GetEntries());
     }

    // --- widening window for SMD north from 9-14 to 7-14
    // --- changing to 8-14 for run25pp
    TLine *t33 = new TLine(8,0.0,8, h_waveform_timesn->GetMaximum() * 1.05);
    t33->SetLineWidth(3);
    t33->SetLineStyle(1);
    t33->SetLineColor(kRed);

    TLine *t44 = new TLine(14,0.0,14,h_waveform_timesn->GetMaximum() * 1.05);
    t44->SetLineWidth(3);
    t44->SetLineStyle(1);
    t44->SetLineColor(kRed);

    t33->Draw("same");
    t44->Draw("same");


    // --- leaving window for SMD south as 6-12
    // --- changing to 7-13 for run25pp
  TLine *t5 = new TLine(7,0.0,7, maxwf);
  t5->SetLineWidth(3);
  t5->SetLineStyle(1);
  t5->SetLineColor(kRed);

  TLine *t6 = new TLine(13,0.0,13, maxwf);
  t6->SetLineWidth(3);
  t6->SetLineStyle(1);
  t6->SetLineColor(kRed);

    Pad[61]->cd();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.08);
    h_waveformSMD_South->GetYaxis()->SetRangeUser(0, h_waveformSMD_South->GetMaximum() * 1.05);
    h_waveformSMD_South->Draw("colz");

    h_waveformSMD_South->SetStats(0);
    h_waveformSMD_South->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveformSMD_South->GetXaxis()->SetRangeUser(0, 16);
    h_waveformSMD_South->SetTitle("SMD SOUTH");
    h_waveformSMD_South->SetXTitle("Sample Number");
    h_waveformSMD_South->SetYTitle("Waveform [ADC]");
    h_waveformSMD_South->GetXaxis()->SetLabelSize(tsize);
    h_waveformSMD_South->GetYaxis()->SetLabelSize(tsize);
    h_waveformSMD_South->GetXaxis()->SetTitleSize(tsize);
    h_waveformSMD_South->GetYaxis()->SetTitleSize(tsize);
    h_waveformSMD_South->GetYaxis()->SetTitleOffset(offset);

     t5->Draw("same");
    t6->Draw("same");

    Pad[66]->cd();
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.06);
    h_waveform_timess->Draw("hist");
    h_waveform_timess->SetTitle("");
    // h_waveform_timess->GetXaxis()->CenterTitle();
    h_waveform_timess->SetStats(0);
    h_waveform_timess->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveform_timess->GetXaxis()->SetRangeUser(0, 16);
    h_waveform_timess->SetTitle("SMD SOUTH");
    h_waveform_timess->GetXaxis()->SetTitle("Sample Number");
    h_waveform_timess->GetYaxis()->SetTitle("Fraction of Towers");
    h_waveform_timess->GetXaxis()->SetLabelSize(tsize);
    h_waveform_timess->GetYaxis()->SetLabelSize(tsize);
    h_waveform_timess->GetXaxis()->SetTitleSize(tsize);
    h_waveform_timess->GetYaxis()->SetTitleSize(tsize);
    h_waveform_timess->GetYaxis()->SetTitleOffset(offset);


    h_waveform_timess->SetFillColorAlpha(kBlue, 0.1);
    if(h_waveform_timess->GetEntries()){
        h_waveform_timess->Scale(1. / h_waveform_timess->GetEntries());
     }

    // --- leaving window for SMD south as 6-12
    // --- changing to 7-13 for run25pp
  TLine *t55 = new TLine(7,0.0,7,h_waveform_timess->GetMaximum() * 1.05);
  t55->SetLineWidth(3);
  t55->SetLineStyle(1);
  t55->SetLineColor(kRed);

  TLine *t66 = new TLine(13,0.0,13,h_waveform_timess->GetMaximum() * 1.05);
  t66->SetLineWidth(3);
  t66->SetLineStyle(1);
  t66->SetLineColor(kRed);
  t55->Draw("same");
  t66->Draw("same");


  // --- widening window for VETO north from 5-9 to 4-10
  // --- staying the same for run25pp
  TLine *t7 = new TLine(4,0.0,4, maxwf);
  t7->SetLineWidth(3);
  t7->SetLineStyle(1);
  t7->SetLineColor(kRed);

  TLine *t8 = new TLine(10,0.0,10, maxwf);
  t8->SetLineWidth(3);
  t8->SetLineStyle(1);
  t8->SetLineColor(kRed);

    Pad[62]->cd();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.08);
    h_waveformVeto_North->GetYaxis()->SetRangeUser(0, h_waveformVeto_North->GetMaximum() * 1.05);
    h_waveformVeto_North->Draw("colz");

    h_waveformVeto_North->SetStats(0);
    h_waveformVeto_North->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveformVeto_North->GetXaxis()->SetRangeUser(0, 16);
    h_waveformVeto_North->SetTitle("VETO NORTH");
    h_waveformVeto_North->SetXTitle("Sample Number");
    h_waveformVeto_North->SetYTitle("Waveform [ADC]");
    h_waveformVeto_North->GetXaxis()->SetLabelSize(tsize);
    h_waveformVeto_North->GetYaxis()->SetLabelSize(tsize);
    h_waveformVeto_North->GetXaxis()->SetTitleSize(tsize);
    h_waveformVeto_North->GetYaxis()->SetTitleSize(tsize);
    h_waveformVeto_North->GetYaxis()->SetTitleOffset(offset);

    t7->Draw("same");
    t8->Draw("same");

    Pad[67]->cd();
    gPad->SetLeftMargin(lf);
    gStyle->SetTitleFontSize(0.06);
    h_waveform_timevn->Draw("hist");
    h_waveform_timevn->SetTitle("");
    // h_waveform_timevn->GetXaxis()->CenterTitle();
    h_waveform_timevn->SetStats(0);
    h_waveform_timevn->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveform_timevn->GetXaxis()->SetRangeUser(0, 16);
    h_waveform_timevn->SetTitle("VETO NORTH");
    h_waveform_timevn->GetXaxis()->SetTitle("Sample Number");
    h_waveform_timevn->GetYaxis()->SetTitle("Fraction of Towers");
    h_waveform_timevn->GetXaxis()->SetLabelSize(tsize);
    h_waveform_timevn->GetYaxis()->SetLabelSize(tsize);
    h_waveform_timevn->GetXaxis()->SetTitleSize(tsize);
    h_waveform_timevn->GetYaxis()->SetTitleSize(tsize);
    h_waveform_timevn->GetYaxis()->SetTitleOffset(offset);


    h_waveform_timevn->SetFillColorAlpha(kBlue, 0.1);
    if(h_waveform_timevn->GetEntries()){
        h_waveform_timevn->Scale(1. / h_waveform_timevn->GetEntries());
     }



  // --- widening window for VETO north from 5-9 to 4-10
  // --- staying the same for run25pp
  TLine *t77 = new TLine(4,0.0,4,h_waveform_timevn->GetMaximum() * 1.05);
  t77->SetLineWidth(3);
  t77->SetLineStyle(1);
  t77->SetLineColor(kRed);

  TLine *t88 = new TLine(10,0.0,10,h_waveform_timevn->GetMaximum() * 1.05);
  t88->SetLineWidth(3);
  t88->SetLineStyle(1);
  t88->SetLineColor(kRed);

  t77->Draw("same");
  t88->Draw("same");

  // --- leaving VETO south as 6-12
  // --- changing to 7-13 for run25pp
  TLine *t9 = new TLine(7,0.0,7,maxwf);
  t9->SetLineWidth(3);
  t9->SetLineStyle(1);
  t9->SetLineColor(kRed);

  TLine *t10 = new TLine(13,0.0,13,maxwf);
  t10->SetLineWidth(3);
  t10->SetLineStyle(1);
  t10->SetLineColor(kRed);


    Pad[63]->cd();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.08);
    h_waveformVeto_South->GetYaxis()->SetRangeUser(0, h_waveformVeto_South->GetMaximum() * 1.05);
    h_waveformVeto_South->Draw("colz");

    h_waveformVeto_South->SetStats(0);
    h_waveformVeto_South->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveformVeto_South->GetXaxis()->SetRangeUser(0, 16);
    h_waveformVeto_South->SetTitle("VETO SOUTH");
    h_waveformVeto_South->SetXTitle("Sample Number");
    h_waveformVeto_South->SetYTitle("Waveform [ADC]");
    h_waveformVeto_South->GetXaxis()->SetLabelSize(tsize);
    h_waveformVeto_South->GetYaxis()->SetLabelSize(tsize);
    h_waveformVeto_South->GetXaxis()->SetTitleSize(tsize);
    h_waveformVeto_South->GetYaxis()->SetTitleSize(tsize);
    h_waveformVeto_South->GetYaxis()->SetTitleOffset(offset);
    t9->Draw("same");
    t10->Draw("same");

    Pad[68]->cd();
    gPad->SetLeftMargin(lf);

    gStyle->SetTitleFontSize(0.06);
    h_waveform_timevs->Draw("hist");
    h_waveform_timevs->SetTitle("");
    // h_waveform_timevs->GetXaxis()->CenterTitle();
    h_waveform_timevs->SetStats(0);
    h_waveform_timevs->GetXaxis()->SetNdivisions(510, kTRUE);
    h_waveform_timevs->GetXaxis()->SetRangeUser(0, 16);
    h_waveform_timevs->SetTitle("VETO SOUTH");
    h_waveform_timevs->GetXaxis()->SetTitle("Sample Number");
    h_waveform_timevs->GetYaxis()->SetTitle("Fraction of Towers");
    h_waveform_timevs->GetXaxis()->SetLabelSize(tsize);
    h_waveform_timevs->GetYaxis()->SetLabelSize(tsize);
    h_waveform_timevs->GetXaxis()->SetTitleSize(tsize);
    h_waveform_timevs->GetYaxis()->SetTitleSize(tsize);
    h_waveform_timevs->GetYaxis()->SetTitleOffset(offset);
    h_waveform_timevs->SetFillColorAlpha(kBlue, 0.1);
    if(h_waveform_timevs->GetEntries()){
        h_waveform_timevs->Scale(1. / h_waveform_timevs->GetEntries());
     }


  // --- leaving VETO south as 6-12
  // --- changing to 7-13 for run25pp
  TLine *t99 = new TLine(7,0.0,7,h_waveform_timevs->GetMaximum() * 1.05);
  t99->SetLineWidth(3);
  t99->SetLineStyle(1);
  t99->SetLineColor(kRed);

  TLine *t100 = new TLine(13,0.0,13,h_waveform_timevs->GetMaximum() * 1.05);
  t100->SetLineWidth(3);
  t100->SetLineStyle(1);
  t100->SetLineColor(kRed);
  t99->Draw("same");
  t100->Draw("same");


  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[8]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[8]->Update();
  TC[8]->Show();
  TC[8]->SetEditable(false);

  return 0;
}



int ZdcMonDraw::SavePlot(const std::string &what, const std::string &type)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
    return iret;
  }
  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" +
                           std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}

int ZdcMonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient *cl = OnlMonClient::instance();

  int icnt = 0;
  std::set <int> expert_indices; // these canvases end up under expert
  expert_indices.insert(2);
  // expert_indices.insert(4);
  // expert_indices.insert(5);

  for (TCanvas* canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    std::string title = canvas->GetTitle();
    if (expert_indices.find(icnt) != expert_indices.end())
    {
      title = "EXPERT -/" + title;
    }
    std::string pngfile = cl->htmlRegisterPage(*this, title, std::to_string(icnt), "png");
    cl->CanvasToPng(canvas, pngfile);
    icnt++;
  }
  return 0;
}

#include "ServerStats.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <phool/phool.h> // PHWHERE

#include <TCanvas.h>
#include <TPad.h>
#include <TText.h>

#include <boost/format.hpp>
#include <sstream>

ServerStats::ServerStats(std::string const& name, OnlMonDraw* onlmondraw)
  : SingleInttDrawer(name)
  , m_onlmondraw(onlmondraw)
{
}

ServerStats::~ServerStats()
{
}

int ServerStats::DrawCanvas()
{
  MakeCanvas();
  m_canvas->SetTitle("Intt Server Stats");

  m_canvas->Clear("D");
  m_canvas->SetEditable(true);
  m_transparent->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  PrintRun.SetTextSize(0.04);
  PrintRun.SetTextColor(1);
  PrintRun.DrawText(0.5, 0.99, "Server Statistics");

  PrintRun.SetTextSize(0.02);
  double vdist = 0.05;
  double vpos = 0.9;

  if(!m_onlmondraw)
  {
    std::cerr << PHWHERE << "\n"
              << "\tMember 'm_onlmondraw' is NULL at call\n"
              << "\tConstructor must be passed a valid pointer\n"
              << std::endl;
    return 1;
  }

  OnlMonClient* cl = OnlMonClient::instance();
  for (auto server_itr = m_onlmondraw->ServerBegin(); server_itr != m_onlmondraw->ServerEnd(); ++server_itr)
  {
    std::ostringstream txt;
    auto servermapiter = cl->GetServerMap(*server_itr);
    if (servermapiter == cl->GetServerMapEnd())
    {
      txt << "Server " << *server_itr
          << " is dead ";
      PrintRun.SetTextColor(kRed);
    }
    else
    {
      txt << "Server " << *server_itr
          << ", run number " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second)
          << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
      if (std::get<0>(servermapiter->second))
      {
        PrintRun.SetTextColor(kGray + 2);
      }
      else
      {
        PrintRun.SetTextColor(kRed);
      }
    }
    PrintRun.DrawText(0.5, vpos, txt.str().c_str());
    vpos -= vdist;
  }
  m_canvas->Update();
  m_canvas->Show();
  m_canvas->SetEditable(false);


  return 0;
}

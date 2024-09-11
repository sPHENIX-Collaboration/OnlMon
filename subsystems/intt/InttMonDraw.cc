#include "InttMonDraw.h"

#include "ServerStats.h"
#include "BcoDiff.h"
#include "FphxBco.h"
#include "History.h"
#include "Hitmap.h"
#include "Hitrates.h"
#include "ZoomedFphxBco.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <iostream>

InttMonDraw::InttMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  m_options["SERVERSTATS"]     = new ServerStats("intt_server_stats", this);
  m_options["bco_diff"]        = new BcoDiff("intt_bco_diff");
  m_options["fphx_bco"]        = new FphxBco("intt_fphx_bco");
  m_options["history"]         = new History("intt_history");
  m_options["chip_hitmap"]     = new Hitmap("intt_hitmap");
  m_options["hitrates"]        = new Hitrates("intt_hitrates");
  m_options["zoomed_fphx_bco"] = new ZoomedFphxBco("intt_zoomed_fphx_bco");
  //...

  return;
}

InttMonDraw::~InttMonDraw()
{
  for(auto& [name, option] : m_options)
  {
    delete option;
  }

  return;
}

int InttMonDraw::Draw(const std::string &what)
{
  int iret = 0;
  bool found = false;
  for(auto const& [name, option] : m_options)
  {
    if(what != "ALL" && what != name)continue;
	found = true;

	// I've seen people returning -1 on error instead of 1
	// Increment if the return value is nonzero
    iret += (option->DrawCanvas() != 0);
  }

  if(!found)
  {
    std::cerr << "Unimplemented drawing option:\n"
	          << "\t" << what << "\n"
	          << "Implemented options:\n"
	          << "\tALL" << std::endl;
	for(auto const& [name, option] : m_options)
	{
		std::cerr << "\t" << name << std::endl;
	}
	++iret;
  }

  return iret;
}

int InttMonDraw::MakeHtml(const std::string &what)
{
  OnlMonClient *cl = OnlMonClient::instance();

  int iret = 0;
  int idraw = 0;
  bool found = false;
  for(auto const& [name, option] : m_options)
  {
	++idraw;
    if(what != "ALL" && what != name)continue;
	found = true;

	// I've seen people returning -1 on error instead of 1
	// Increment if the return value is nonzero
	int rv = (option->DrawCanvas() != 0);
    iret += rv;

	// on error no html output please
	if(rv || !option->GetCanvas())continue;

    // Registers the canvas png file to the menu and produces the png file
	std::string pngfile = cl->htmlRegisterPage(*this, name, std::to_string(idraw), "png");
    cl->CanvasToPng(option->GetCanvas(), pngfile);
  }

  if(!found)
  {
    std::cerr << "Unimplemented drawing option:\n"
	          << "\t" << what << "\n"
	          << "Implemented options:\n"
	          << "\tALL" << std::endl;
	for(auto const& [name, option] : m_options)
	{
		std::cerr << "\t" << name << std::endl;
	}
	++iret;
  }

  return iret;
}

int InttMonDraw::SavePlot(std::string const& what, std::string const& type)
{
  OnlMonClient *cl = OnlMonClient::instance();

  int iret = 0;
  int idraw = 0;
  bool found = false;
  for(auto const& [name, option] : m_options)
  {
	++idraw;
    if(what != "ALL" && what != name)continue;
	found = true;

	// I've seen people returning -1 on error instead of 1
	// Increment if the return value is nonzero
	int rv = (option->DrawCanvas() != 0);
    iret += rv;

	// on error no png files please
	if(rv || !option->GetCanvas())continue;

    std::string pngfile = ThisName + "_" + std::to_string(idraw) + "_" + std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(option->GetCanvas(), pngfile);
  }

  if(!found)
  {
    std::cerr << "Unimplemented drawing option:\n"
	          << "\t" << what << "\n"
	          << "Implemented options:\n"
	          << "\tALL" << std::endl;
	for(auto const& [name, option] : m_options)
	{
		std::cerr << "\t" << name << std::endl;
	}
	++iret;
  }

  return iret;
}


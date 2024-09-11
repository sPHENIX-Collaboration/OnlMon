#include "SingleCanvasDrawer.h"
#include "OnlMonClient.h"

#include <phool/phool.h> // PHWHERE

#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>

#include <iostream>

SingleCanvasDrawer::SingleCanvasDrawer(std::string const& name)
  : m_name(name)
{
}

SingleCanvasDrawer::~SingleCanvasDrawer()
{
  delete gROOT->FindObject(m_name.c_str());
}

int SingleCanvasDrawer::DrawCanvas()
{
  MakeCanvas();
  return 0;
}

int SingleCanvasDrawer::MakeCanvas()
{
  TObject* found_object = gROOT->FindObject(m_name.c_str());
  if(found_object && found_object == m_canvas)
  {
    /// No (re)draw is necessary
    return 1;
  }

  if(found_object)
  {
    /// There is a TObject called m_name somewhere, but it's not our m_canvas
    std::cerr << PHWHERE << "\n"
              << "\tMultiple instances of classes trying to manage '" << m_name << "' are running together\n"
              << "\tFound existing TObject at: " << Form("0x%p", (void*)found_object) << "\n"
              << "\tNot the member TCanvas at: " << Form("0x%p",     (void*)m_canvas) << "\n"
              << "\tDeleting found TObject at: " << Form("0x%p", (void*)found_object) << std::endl;
    delete found_object;
  }

  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  m_canvas = new TCanvas(m_name.c_str(), m_name.c_str(), -1, 0, xsize, ysize);
  gSystem->ProcessEvents();

  return 0;
}


#ifndef SINGLE_CANVAS_DRAWER_H
#define SINGLE_CANVAS_DRAWER_H

#include <string>

class TCanvas;
class TPad;

class SingleCanvasDrawer
{
public:
  SingleCanvasDrawer(std::string const&);
  virtual ~SingleCanvasDrawer();

  std::string GetName() {return m_name;}
  TCanvas* GetCanvas() {return m_canvas;}

  virtual int DrawCanvas();

protected:
  virtual int MakeCanvas();

  std::string m_name;
  TCanvas* m_canvas{nullptr};
};

#endif//SINGLE_CANVAS_DRAWER_H

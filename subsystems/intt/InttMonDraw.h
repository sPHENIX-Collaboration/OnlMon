#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>
#include <onlmon/SingleCanvasDrawer.h>

#include <map>
#include <string>

class InttMonDraw : public OnlMonDraw
{
 public: 
  InttMonDraw(const std::string &name = "INTTMONDRAW");
  ~InttMonDraw() override;

  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(std::string const& = "ALL", std::string const& = "png") override;

 private:
  std::map<std::string, SingleCanvasDrawer*> m_options;

};

#endif//INTT_MON_DRAW_H

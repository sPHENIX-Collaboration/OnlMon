#ifndef TPOT_TPOTMONDRAW_H
#define TPOT_TPOTMONDRAW_H

#include "MicromegasDefs.h"
#include "MicromegasGeometry.h"
#include "MicromegasMapping.h"

#include <onlmon/OnlMonDraw.h>

#include <array>
#include <memory>
#include <string>  

class OnlMonDB;
class TCanvas;
class TH1;
class TPad;

class TpotMonDraw : public OnlMonDraw
{
  public:
  
  /// constructor
  TpotMonDraw(const std::string &name);
  
  /// destructor
  ~TpotMonDraw() override = default;

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakePS(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;

  private:

  TCanvas* get_canvas(const std::string& name, bool clear = true );
  TCanvas* create_canvas(const std::string &name);
  
  int draw_detector_occupancy();
  
  using histogram_array_t = std::array<TH1*, MicromegasDefs::m_nfee>;
  
  /// get detector dependent histogram array from base name
  histogram_array_t get_histograms( const std::string& name );  

  /// draw histogram array
  int draw_array( const std::string& name, const histogram_array_t&, const std::string& /*option*/ = "" );
  
  /// draw detector names in current canvas
  /** only works if canvas contains one of the properly formated TH2Poly histograms */
  void draw_detnames_sphenix();
    
  // mapping
  MicromegasMapping m_mapping;
  
  // geometry
  MicromegasGeometry m_geometry;
  
  /// needed to get time axis right
  int TimeOffsetTicks = -1;
  
  // draw run and time in a given pad
  void draw_time( TPad*);
  
  // canvases
  std::array<TCanvas*, 8> m_canvas = {{nullptr}};
  std::unique_ptr<OnlMonDB> dbvars;
};

#endif /* TPOT_TPOTMONDRAW_H */

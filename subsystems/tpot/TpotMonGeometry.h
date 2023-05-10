#ifndef TPOT_TPOTMONGEOMETRY_H
#define TPOT_TPOTMONGEOMETRY_H

#include "MicromegasDefs.h"

#include <vector>
#include <map>

/// handle simplified TPOT 2D geometry, for histogram representation
class TpotMonGeometry
{
  public:
  
  /// constructor
  TpotMonGeometry();
  
  //! tile definitions
  static constexpr double m_tile_length = 54.2; // cm
  static constexpr double m_tile_width = 31.6;  // cm
  
  //! tile centers
  using point_t = std::pair<double, double>;
  using point_list_t = std::vector<point_t>;
  point_list_t m_tile_centers;
  
};

#endif

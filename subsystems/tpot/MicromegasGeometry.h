#ifndef TPOT_TPOTMONGEOMETRY_H
#define TPOT_TPOTMONGEOMETRY_H

#include "MicromegasDefs.h"

#include <cstddef>
#include <vector>
#include <map>

/// handle simplified TPOT 2D geometry, for histogram representation
class MicromegasGeometry
{
  public:

  using point_t = std::pair<double, double>;
  using point_list_t = std::vector<point_t>;

  /// constructor
  MicromegasGeometry();
  
  //! get number of defined tiles
  size_t get_ntiles() const 
  { return m_tile_centers.size(); }
  
  //! get tile center for given index
  point_t get_tile_center( size_t index )  const 
  { return m_tile_centers[index]; }

  //! get tile boundaries for a given index
  point_list_t get_tile_boundaries( size_t /* index */ ) const;
      
  private:

  //! tile definitions
  static constexpr double m_tile_length = 54.2; // cm
  static constexpr double m_tile_width = 31.6;  // cm

  //! tile centers
  point_list_t m_tile_centers;

};

#endif

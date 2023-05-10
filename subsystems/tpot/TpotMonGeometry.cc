#include "TpotMonGeometry.h"

//__________________________________________________
TpotMonGeometry::TpotMonGeometry()
{
  /*
   * to convert sphenix coordinates into a x,y 2D histogram, 
   * we transform z(3D) = x(2D)
   * and x (3D) = y (2D)
   */
  {
    const double tile_x = 0;
    for( const double& tile_z:{ -84.6, -28.2, 28.2, 84.6 } )
    { m_tile_centers.push_back( {tile_z, tile_x} ); }
  }
    
  {
    // neighbor sectors have two modules, separated by 10cm
    for( const double& tile_x: { -m_tile_width - 2, m_tile_width+2 } )
      for( const double& tile_z:{ -37.1, 37.1 } )
    { m_tile_centers.push_back( {tile_z, tile_x} ); }
  }  
}

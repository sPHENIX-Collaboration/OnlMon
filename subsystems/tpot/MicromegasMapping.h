#ifndef MICROMEGAS_MICROMEGASMAPPING_H
#define MICROMEGAS_MICROMEGASMAPPING_H

/*!
 * \file MicromegasMapping.h
 * \author Hugo Pereira Da Costa <hugo.pereira-da-costa@cea.fr>
 */

#include "MicromegasDefs.h"

#include <trackbase/TrkrDefs.h>

#include <array>
#include <map>
#include <string>
#include <vector>

/// micromegas mapping
/** this is a trimmed down version of coresoftware/offline/packages/micromegas/MicromegasMapping.h */
class MicromegasMapping
{
  public:

  /// constructor
  MicromegasMapping();

  /// get list of fee ids
  std::vector<int> get_fee_id_list() const;

  /// detector names (sphenix)
  std::vector<std::string> get_detnames_sphenix() const;
  
  /// get hitsetkey from fiber_id (fee_id)
  MicromegasDefs::SegmentationType get_segmentation( int /*fee_id*/ ) const;

  /// tile id
  uint8_t get_tile( int /* fee_id */ ) const;  
  
  /// get detector name (saclay) from fiber_id (fee_id)
  /** saclay detector name are of type MxxP and MxxZ, with xx the module number */
  std::string get_detname_saclay( int /*fee_id*/) const;

  /// get detector name (sphenix) from fiber_id (fee_id)
  /** sphenix detector name are of type SWP, SWZ, etc. */
  std::string get_detname_sphenix( int /*fee_id*/) const;

  /// get physical strip number from channel_id
  /**
   * physical strip number correspond to a position in the
   * detector's geant implementation, with increasing number corresponding to strips further away from the detector's edge,
   * as defined in CylinderGeomMicromegas
   */
  int get_physical_strip( int /*fee_id*/, int /*channel_id*/) const;

  /// fee ids
  std::vector<int> get_fee_ids() const;
  
  private:

  /// construct fee channel id to physical strip mapping
  void construct_channel_mapping();
  
  class TileId
  {
    public:

    //! constructor
    TileId( uint8_t layer, MicromegasDefs::SegmentationType segmentation, uint8_t tile ):
      m_layer( layer ),
      m_segmentation( segmentation ),
      m_tile( tile )
    {}
    
    uint8_t m_layer = 0;
    MicromegasDefs::SegmentationType m_segmentation = MicromegasDefs::SegmentationType::SEGMENTATION_Z;
    uint8_t m_tile = 0;
  };
  
  /// contains all relevant detector information
  /** this effectively implements mapping between fee_id as defined in EDBC,    * detector names (in both Saclay and sPHENIX convention),
   * and hitsetkey which is the detector unique identifier
   */
  class DetectorId
  {
    public:

    /// constructor
    DetectorId(
      int fee_id, const TileId& tile_id,
      const std::string& fibername, const std::string& breakoutname, 
      const std::string& detname_saclay, const std::string& detname_sphenix ):
      m_fee_id( fee_id ),
      m_tile_id( tile_id ),
      m_fibername( fibername ),
      m_breakoutname( breakoutname ),
      m_detname_saclay( detname_saclay ),
      m_detname_sphenix( detname_sphenix )
    {}

    /// fee_id
    int m_fee_id = 0;

    /// tile id 
    /** 
     * contains layer, segmentation, tile number
     * in offline, this is the same information as the HitSetKey
     **/
    TileId m_tile_id;

    /// fiber name
    std::string m_fibername;
    
    /// breakout cable name
    std::string m_breakoutname;
    
    /// detector name
    std::string m_detname_saclay;

    /// detector name
    std::string m_detname_sphenix;
  };

  /// list of defined detectors
  std::vector<DetectorId> m_detectors;

  /// map detector_id to fee_id
  std::map<int, DetectorId> m_detector_map;
  
  /// map FEE channel id to physical strip id (z view)
  std::array<int, MicromegasDefs::m_nchannels_fee> m_fee_to_strip_mapping_z = {{0}};
  
  /// map FEE channel id to physical strip id (phi view)
  std::array<int, MicromegasDefs::m_nchannels_fee> m_fee_to_strip_mapping_phi = {{0}};

};

#endif

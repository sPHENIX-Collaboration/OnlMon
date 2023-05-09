// Tell emacs that this is a C++ source
// This file is really -*- C++ -*-.

#ifndef MICROMEGAS_MICROMEGASDEFS_H
#define MICROMEGAS_MICROMEGASDEFS_H

/*!
 * \file MicromegasDefs.h
 * \author Hugo Pereira Da Costa <hugo.pereira-da-costa@cea.fr>
 */

#include <cstdint>

namespace MicromegasDefs
{

  //! tells the direction along which a given cylinder is segmented
  enum class SegmentationType: uint8_t
  {
    SEGMENTATION_Z,
    SEGMENTATION_PHI
  };

  //! tells the drift direction for a given micromegas layer
  /*! this is needed for properly implementing transverse diffusion in the layer */
  enum class DriftDirection: uint8_t
  {
    INWARD,
    OUTWARD
  };

  //! TPOT packet id
  static constexpr unsigned int m_packet_id = 5001;
  
  //! number of channels per fee board
  static constexpr int m_nchannels_fee = 256;

  //! number of fee boards
  static constexpr int m_nfee = 16;

  //! total number of channels
  static constexpr int m_nchannels_total = m_nfee*m_nchannels_fee;

  //! max adc value per readout sample
  static constexpr int m_max_adc = 1024;
  
}

#endif

#ifndef TPOT_TPOTMON_H
#define TPOT_TPOTMON_H

#include "MicromegasDefs.h"
#include "MicromegasMapping.h"
#include "MicromegasGeometry.h"

#include <onlmon/OnlMon.h>

#include <array>
#include <memory>

class Event;
class OnlMonDB;
class TH1;
class TH2;
class TH2Poly;

class TpotMon : public OnlMon
{
  public:
  TpotMon(const std::string &name);
  ~TpotMon() override = default;

  int process_event(Event *evt) override;
  int Init() override;
  int BeginRun(const int runno) override;
  int Reset() override;

  private:
  int DBVarInit();
    
  //! setup bins in a TH2Poly
  void setup_bins( TH2Poly* );
  
  int evtcnt = 0;
  int idummy = 0;
  std::unique_ptr<OnlMonDB> dbvars;
  
  //! mapping
  MicromegasMapping m_mapping;
    
  //! geometry
  MicromegasGeometry m_geometry;
  
  //! counter
  TH1* m_counters = nullptr;
   
  //! TPOT per/detector multiplicity
  TH2Poly* m_detector_multiplicity_z = nullptr;
  TH2Poly* m_detector_multiplicity_phi = nullptr;
  
  //! TPOT per/detector occupancy
  TH2Poly* m_detector_occupancy_z = nullptr;
  TH2Poly* m_detector_occupancy_phi = nullptr;

  //@name per detector structure
  //@{
  class detector_histograms_t 
  {
    public:

    /// adc counts vs sample id in each detector
    TH2* m_adc_vs_sample = nullptr;
    
    /// total charge per hit in each detector
    TH1* m_hit_charge = nullptr;
    
    /// number of hits per event in each detector
    TH1* m_hit_multiplicity = nullptr;
    
    /// m_hit_vs_channel
    TH1* m_hit_vs_channel = nullptr;    
  };
  //@}
  
  //@name map tile centers (from MicromegasGeometry) to fee_id
  std::map<int, MicromegasGeometry::point_t> m_tile_centers;
  
  //@name map detector histograms to fee id
  std::map<int, detector_histograms_t> m_detector_histograms;
  
};

#endif /* TPOT_TPOTMON_H */

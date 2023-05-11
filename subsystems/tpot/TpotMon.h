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
   
  //! TPOT status histogram
  TH2Poly* m_global_occupancy_phi = nullptr;
  TH2Poly* m_global_occupancy_z = nullptr;
  
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
  
  //@name detector histograms
  std::array<detector_histograms_t, MicromegasDefs::m_nfee> m_detector_histograms;

  //! map fee id to index
  std::map<int, size_t> m_det_index_map;
  
};

#endif /* TPOT_TPOTMON_H */

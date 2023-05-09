#ifndef TPOT_TPOTMON_H
#define TPOT_TPOTMON_H

#include "MicromegasDefs.h"
#include "MicromegasMapping.h"

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
  
  //! create tiles
  void setup_tiles();
  
  //! setup bins in a TH2Poly
  void setup_bins( TH2Poly* );
  
  int evtcnt = 0;
  int idummy = 0;
  std::unique_ptr<OnlMonDB> dbvars;
  
  // mapping
  MicromegasMapping m_mapping;
    
  //! TPOT status histogram
  TH2Poly* m_global_occupancy_phi = nullptr;
  TH2Poly* m_global_occupancy_z = nullptr;
  
  ///@name raw hits histograms
  //@{
  /// adc counts vs sample id in each detector
  std::array<TH1*, MicromegasDefs::m_nfee> m_adc_vs_sample = {{nullptr}};

  /// total charge per hit in each detector
  std::array<TH1*, MicromegasDefs::m_nfee> m_hit_charge= {{nullptr}};
  
  /// number of hits per event in each detector
  std::array<TH1*, MicromegasDefs::m_nfee> m_hit_multiplicity= {{nullptr}};

  /// total number of hits per channel in each detector
  std::array<TH1*, MicromegasDefs::m_nfee> m_hit_vs_channel = {{nullptr}};

  //@}

  //! tile definitions
  static constexpr double m_tile_length = 54.2; // cm
  static constexpr double m_tile_width = 31.6;  // cm
  
  //! tile centers
  using point_t = std::pair<double, double>;
  using point_list_t = std::vector<point_t>; 
  point_list_t m_tile_centers;
  
};

#endif /* TPOT_TPOTMON_H */

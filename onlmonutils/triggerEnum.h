#ifndef __TRIGGERENUM_H__
#define __TRIGGERENUM_H__

#include <map>
#include <string>

// ! updated : 2025-05-12

namespace TriggerEnum {

enum BitCodes
{
    CLOCK = 0,
    ZDC_NS = 1,
    DONUT = 2,
    TPC_LASER = 5,
    ZDC_S = 6,
    ZDC_N = 7,
    MBD_S2 = 8, // mbd south >= 2
    MBD_N2 = 9, // mbd north >= 2
    MBD_NS2 = 10, // mbd north+south >= 2
    MBD_NS1 = 11, // mbd north+south >= 1
    MBD_NS2_ZVRTX10 = 12, // mbd north+south >= 2, zvertex < 10 cm
    MBD_NS2_ZVRTX30 = 13, // mbd north+south >= 2, zvertex < 30 cm
    MBD_NS2_ZVRTX150 = 14, // mbd north+south >= 2, zvertex < 150 cm
    MBD_NS1_ZVRTX10 = 15,  // mbd north+south >= 1, zvertex < 10 cm
    PHOTON6_MBD_NS2 = 16, // photon 6 GeV, mbd north+south >= 2
    PHOTON8_MBD_NS2 = 17, // photon 8 GeV, mbd north+south >= 2
    PHOTON10_MBD_NS2 = 18, // photon 10 GeV, mbd north+south >= 2
    PHOTON12_MBD_NS2 = 19, // photon 12 GeV, mbd north+south >= 2
    HCAL_SINGLES = 24,// hcal singles
    HCAL_NARROW_VERT = 25, // hcal narrow vertical coincidence
    HCAL_WIDE_VERT = 26, // hcal wide vertical coincidence
    HCAL_NARROW_HORZ = 27, // hcal narrow horizontal coincidence
    HCAL_WIDE_HORZ = 28, // hcal wide horizontal coincidence
    MBD_LASER = 40,
    RANDOM = 41,
};

const TriggerEnum::BitCodes MBTriggers[] = {
    TriggerEnum::MBD_NS2, // 10
    TriggerEnum::MBD_NS1, // 11
    TriggerEnum::MBD_NS2_ZVRTX10,
    TriggerEnum::MBD_NS2_ZVRTX30,
    TriggerEnum::MBD_NS2_ZVRTX150,
    TriggerEnum::MBD_NS1_ZVRTX10
};

const char * MBTriggerNames[] = {
    "MBDNS>=2",
    "MBDNS>=1",
    "MBDNS>=2 |z|<10",
    "MBDNS>=2 |z|<30",
    "MBDNS>=2 |z|<150",
    "MBDNS>=1 |z|<10"
};

const int nMBTriggers = sizeof(MBTriggers) / sizeof(TriggerEnum::BitCodes);


const std::map< TriggerEnum::BitCodes, std::string> TriggerNames = 
{
   { TriggerEnum::BitCodes::CLOCK, "Clock"},
   { TriggerEnum::BitCodes::DONUT, "Donut"},
   { TriggerEnum::BitCodes::TPC_LASER, "TPC Laser"},
   { TriggerEnum::BitCodes::MBD_LASER, "MBD Laser"},
   { TriggerEnum::BitCodes::ZDC_S, "ZDC South"},
   { TriggerEnum::BitCodes::ZDC_N, "ZDC North"},
   { TriggerEnum::BitCodes::ZDC_NS, "ZDC Coincidence"},
   { TriggerEnum::BitCodes::RANDOM, "Random"},
   { TriggerEnum::BitCodes::MBD_S2, "MBD S >= 2"},
   { TriggerEnum::BitCodes::MBD_N2, "MBD N >= 2"},
   { TriggerEnum::BitCodes::MBD_NS2, "MBD N&S >= 2"},
   { TriggerEnum::BitCodes::MBD_NS1, "MBD N&S >= 1"},
   { TriggerEnum::BitCodes::MBD_NS2_ZVRTX10, "MBD N&S >= 2, vtx < 10 cm"},
   { TriggerEnum::BitCodes::MBD_NS2_ZVRTX30, "MBD N&S >= 2, vtx < 30 cm"},
   { TriggerEnum::BitCodes::MBD_NS2_ZVRTX150, "MBD N&S >= 2, vtx < 150 cm"},
   { TriggerEnum::BitCodes::MBD_NS1_ZVRTX10, "MBD N&S >= 1, vtx < 10 cm"},
   { TriggerEnum::BitCodes::PHOTON6_MBD_NS2, "Photon 6 GeV + MBD NS >= 2"},
   { TriggerEnum::BitCodes::PHOTON8_MBD_NS2, "Photon 8 GeV + MBD NS >= 2"},
   { TriggerEnum::BitCodes::PHOTON10_MBD_NS2, "Photon 10 GeV + MBD NS >= 2"},
   { TriggerEnum::BitCodes::PHOTON12_MBD_NS2, "Photon 12 GeV + MBD NS >= 2"},
   { TriggerEnum::BitCodes::HCAL_SINGLES, "HCAL Singles"},
   { TriggerEnum::BitCodes::HCAL_NARROW_VERT, "HCAL Narrow Vertical Coincidence"},
   { TriggerEnum::BitCodes::HCAL_WIDE_VERT, "HCAL Wide Vertical Coincidence"},
   { TriggerEnum::BitCodes::HCAL_NARROW_HORZ, "HCAL Narrow Horizontal Coincidence"},
   { TriggerEnum::BitCodes::HCAL_WIDE_HORZ, "HCAL Wide Horizontal Coincidence"},
};

std::string GetTriggerName(TriggerEnum::BitCodes code);


} // namespace TriggerEnum



#endif // __TRIGGERENUM_H__

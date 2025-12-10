#ifndef __TRIGGERENUM_H__
#define __TRIGGERENUM_H__

#include <map>
#include <string>

// ! updated : 2025-12-10

namespace TriggerEnum {

enum BitCodes
{
    CLOCK = 0,
    
    DONUT = 4,
    TPC_LASER = 5,
    
    ZDC_NS = 1,
    ZDC_N = 6,
    ZDC_S = 7,

    MBD_S1 = 8, // mbd south >= 2
    MBD_N1 = 9, // mbd north >= 2

    MBD_NS1 = 10, // mbd north+south >= 2
    MBD_NS2 = 11, // mbd north+south >= 1
    MBD_NS1_ZVRTX10 = 12, // mbd north+south >= 1, zvertex < 10 cm
    MBD_NS1_ZVRTX13 = 13, // mbd north+south >= 1, zvertex < 13.3 cm
    MBD_NS1_ZVRTX150 = 14, // mbd north+south >= 1, zvertex < 150 cm
    MBD_NS2_ZVRTX10 = 15,  // mbd north+south >= 2, zvertex < 10 cm
    
    JET6_MBD_NS1 = 16, // jet 6 GeV, mbd north+south >= 1
    JET8_MBD_NS1 = 17, // jet 8 GeV, mbd north+south >= 1
    JET10_MBD_NS1 = 18, // jet 10 GeV, mbd north+south >= 1
    JET12_MBD_NS1 = 19, // jet 12 GeV, mbd north+south >= 1

    JET6 = 20, // jet 6 GeV
    JET8 = 21, // jet 8 GeV
    JET10 = 22, // jet 10 GeV
    JET12 = 23, // jet 12 GeV
  
    PHOTON2_MBD_NS1 = 24, // photon 3 GeV, mbd north+south >= 1
    PHOTON3_MBD_NS1 = 25, // photon 4 GeV, mbd north+south >= 1
    PHOTON4_MBD_NS1 = 26, // photon 5 GeV, mbd north+south >= 1
    PHOTON5_MBD_NS1 = 27, // photon 6 GeV, mbd north+south >= 1
    
    PHOTON2 = 28, // photon 3 GeV
    PHOTON3 = 29, // photon 4 GeV
    PHOTON4 = 30, // photon 5 GeV
    PHOTON5 = 31, // photon 6 GeV

    JET6_MBD_NS1_ZVRTX10 = 32, // jet 6 GeV, mbd north+south >= 1, zvertex < 10 cm
    JET8_MBD_NS1_ZVRTX10 = 33, // jet 8 GeV, mbd north+south >= 1, zvertex < 10 cm
    JET10_MBD_NS1_ZVRTX10 = 34, // jet 10 GeV, mbd north+south >= 1, zvertex < 10 cm
    JET12_MBD_NS1_ZVRTX10 = 35, // jet 12 GeV, mbd north+south >= 1, zvertex < 10 cm

    PHOTON3_MBD_NS1_ZVRTX10 = 36, // photon 4 GeV, mbd north+south >= 1, zvertex < 10 cm
    PHOTON4_MBD_NS1_ZVRTX10 = 37, // photon 5 GeV, mbd north+south >= 1, zvertex < 10 cm
    PHOTON5_MBD_NS1_ZVRTX10 = 38, // photon 6 GeV, mbd north+south >= 1, zvertex < 10 cm
    
    NA = 39,

    MBD_LASER = 40,
    RANDOM = 41,

    HCAL_SINGLES = 42,// hcal singles
    HCAL_NARROW_VERT = 43, // hcal narrow vertical coincidence
    HCAL_WIDE_VERT = 44, // hcal wide vertical coincidence
    HCAL_NARROW_HORZ = 45, // hcal narrow horizontal coincidence
    HCAL_WIDE_HORZ = 46, // hcal wide horizontal coincidence

};

const int NUM_MBD_TRIGGERS = 7;
const TriggerEnum::BitCodes MBTriggers[] = {
    TriggerEnum::MBD_NS1,
    TriggerEnum::MBD_NS2,
    TriggerEnum::MBD_NS1_ZVRTX10,
    TriggerEnum::MBD_NS1_ZVRTX13,
    TriggerEnum::MBD_NS1_ZVRTX150,
    TriggerEnum::MBD_NS2_ZVRTX10,
    TriggerEnum::ZDC_NS,
};
const char * MBTriggerNames[] = {
    "MBD N&S >= 1",
    "MBD N&S >= 2",
    "MBD N&S >= 1, |z| < 10 cm",
    "MBD N&S >= 1, |z| < 13.3 cm",
    "MBD N&S >= 1, |z| < 150 cm",
    "MBD N&S >= 2, |z| < 10 cm",
    "ZDC Coincidence",
};
const int nMBTriggers = sizeof(MBTriggers) / sizeof(TriggerEnum::BitCodes);


const std::map< TriggerEnum::BitCodes, std::string> TriggerNames = 
{
    {CLOCK, "Clock"},
    {DONUT, "Donut"},
    {TPC_LASER, "TPC Laser"},

    {ZDC_NS, "ZDC N&S"},
    {ZDC_N, "ZDC N"},
    {ZDC_S, "ZDC S"},

    {MBD_S1, "MBD S >= 2"},
    {MBD_N1, "MBD N >= 2"},

    {MBD_NS1, "MBD N&S >= 1"},
    {MBD_NS2, "MBD N&S >= 2"},
    {MBD_NS1_ZVRTX10, "MBD N&S >= 1, |z| < 10 cm"},
    {MBD_NS1_ZVRTX13, "MBD N&S >= 1, |z| < 13.3 cm"},
    {MBD_NS1_ZVRTX150, "MBD N&S >= 1, |z| < 150 cm"},
    {MBD_NS2_ZVRTX10, "MBD N&S >= 2, |z| < 10 cm"},

    {JET6_MBD_NS1, "Jet 6 GeV + MBD N&S >= 1"},
    {JET8_MBD_NS1, "Jet 8 GeV + MBD N&S >= 1"},
    {JET10_MBD_NS1, "Jet 10 GeV + MBD N&S >= 1"},
    {JET12_MBD_NS1, "Jet 12 GeV + MBD N&S >= 1"},

    {JET6, "Jet 6 GeV"},
    {JET8, "Jet 8 GeV"},
    {JET10, "Jet 10 GeV"},
    {JET12, "Jet 12 GeV"},

    {PHOTON2_MBD_NS1, "Photon 3 GeV + MBD N&S >= 1"},
    {PHOTON3_MBD_NS1, "Photon 4 GeV + MBD N&S >= 1"},
    {PHOTON4_MBD_NS1, "Photon 5 GeV + MBD N&S >= 1"},
    {PHOTON5_MBD_NS1, "Photon 6 GeV + MBD N&S >= 1"},

    {PHOTON2, "Photon 3 GeV"},
    {PHOTON3, "Photon 4 GeV"},
    {PHOTON4, "Photon 5 GeV"},
    {PHOTON5, "Photon 6 GeV"},  
    
    {JET6_MBD_NS1_ZVRTX10, "Jet 6 GeV + MBD N&S >= 1, |z| < 10 cm"},
    {JET8_MBD_NS1_ZVRTX10, "Jet 8 GeV + MBD N&S >= 1, |z| < 10 cm"},
    {JET10_MBD_NS1_ZVRTX10, "Jet 10 GeV + MBD N&S >= 1, |z| < 10 cm"},
    {JET12_MBD_NS1_ZVRTX10, "Jet 12 GeV + MBD N&S >= 1, |z| < 10 cm"},

    {PHOTON3_MBD_NS1_ZVRTX10, "Photon 4 GeV + MBD N&S >= 1, |z| < 10 cm"},
    {PHOTON4_MBD_NS1_ZVRTX10, "Photon 5 GeV + MBD N&S >= 1, |z| < 10 cm"},
    {PHOTON5_MBD_NS1_ZVRTX10, "Photon 6 GeV + MBD N&S >= 1, |z| < 10 cm"},
    
    {MBD_LASER, "MBD Laser"},
    {RANDOM, "Random"},

    {HCAL_SINGLES, "HCAL Singles"},
    {HCAL_NARROW_VERT, "HCAL Narrow Vertical Coincidence"},
    {HCAL_WIDE_VERT, "HCAL Wide Vertical Coincidence"},
    {HCAL_NARROW_HORZ, "HCAL Narrow Horizontal Coincidence"},
    {HCAL_WIDE_HORZ, "HCAL Wide Horizontal Coincidence"},

};

std::string GetTriggerName(TriggerEnum::BitCodes code);


} // namespace TriggerEnum



#endif // __TRIGGERENUM_H__

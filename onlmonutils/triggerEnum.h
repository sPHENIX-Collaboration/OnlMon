#ifndef __TRIGGERENUM_H__
#define __TRIGGERENUM_H__

enum TriggerBitCodes
{
    Trigger_CLOCK = 0,
    Trigger_ZDC_S = 1,
    Trigger_ZDC_N = 2,
    Trigger_ZDC_NS = 3, // zdc north+south
    Trigger_RANDOM = 4,
    Trigger_MBD_S2 = 8, // mbd south >= 2
    Trigger_MBD_N2 = 9, // mbd north >= 2
    Trigger_MBD_NS2 = 10, // mbd north+south >= 2
    Trigger_MBD_NS1 = 11, // mbd north+south >= 1
    Trigger_MBD_NS2_ZVRTX10 = 12, // mbd north+south >= 2, zvertex < 10 cm
    Trigger_MBD_NS2_ZVRTX30 = 13, // mbd north+south >= 2, zvertex < 30 cm
    Trigger_MBD_NS2_ZVRTX150 = 14, // mbd north+south >= 2, zvertex < 150 cm
    Trigger_MBD_NS1_ZVRTX10 = 15,  // mbd north+south >= 1, zvertex < 10 cm
    Trigger_PHOTON6_MBD_NS2 = 16, // photon 6 GeV, mbd north+south >= 2
    Trigger_PHOTON8_MBD_NS2 = 17, // photon 8 GeV, mbd north+south >= 2
    Trigger_PHOTON10_MBD_NS2 = 18, // photon 10 GeV, mbd north+south >= 2
    Trigger_PHOTON12_MBD_NS2 = 19, // photon 12 GeV, mbd north+south >= 2
    Trigger_HCAL_SINGLES = 24,// hcal singles
    Trigger_HCAL_NARROW_VERT = 25, // hcal narrow vertical coincidence
    Trigger_HCAL_WIDE_VERT = 26, // hcal wide vertical coincidence
    Trigger_HCAL_NARROW_HORZ = 27, // hcal narrow horizontal coincidence
    Trigger_HCAL_WIDE_HORZ = 28, // hcal wide horizontal coincidence
    Trigger_MBD_LASER = 40,
};



#endif // __TRIGGERENUM_H__
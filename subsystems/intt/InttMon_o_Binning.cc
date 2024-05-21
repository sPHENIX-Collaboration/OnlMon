#include "InttMon.h"

int InttMon::HitBin(
    struct InttMon::HitData_s const& hit_data)
{
  int b = 1;
  int s = 1;

  if (0 < b && 0 <= hit_data.chp && hit_data.chp < 26)
  {
    b += hit_data.chp * s;
  }
  else
  {
    b = -1;
  }
  s *= 26;

  if (0 < b && 0 <= hit_data.fee && hit_data.fee < 14)
  {
    b += hit_data.fee * s;
  }
  else
  {
    b = -1;
  }
  s *= 14;

  // if(0 < b && 3001 <= hit_data.pid && hit_data.pid < 3009) {
  // 	b += (hit_data.pid - 3001) * s;
  // } else {
  // 	b = -1;
  // }
  // s *= 8;

  // return the bin index if the parameters are valid
  // otherwise, return the overflow bin index
  return (0 < b) ? b : ++s;
}

int InttMon::BcoBin(
    struct InttMon::BcoData_s const& bco_data)
{
  int b = 1;
  int s = 1;

  if (0 < b && 0 <= bco_data.bco && bco_data.bco < 128)
  {
    b += bco_data.bco * s;
  }
  else
  {
    b = -1;
  }
  s *= 128;

  if (0 < b && 0 <= bco_data.fee && bco_data.fee < 14)
  {
    b += bco_data.fee * s;
  }
  else
  {
    b = -1;
  }
  s *= 14;

  // if(0 < b && 3001 <= bco_data.pid && bco_data.pid < 3009) {
  // 	b += (bco_data.pid - 3001)  * s;
  // } else {
  // 	b = -1;
  // }
  // s *= 8;

  // return the bin index if the parameters are valid
  // otherwise, return the overflow bin index
  return (0 < b) ? b : ++s;
}

//             Ladder Structure                //
//=============================================//
//      U14     U1   Ladder_z  Type B  North   //
//      U15     U2      .        .       .     //
//      U16     U3      3        .       .     //
//      U17     U4      .        .       .     //
//      U18     U5   Ladder_z  Type B    .     //
//------------------------------------   .     //
//      U19     U6   Ladder_z  Type A    .     //
//      U20     U7      .        .       .     //
//      U21     U8      .        .       .     //
//      U22     U9      2        .       .     //
//      U23     U10     .        .       .     //
//      U24     U11     .        .       .     //
//      U25     U12     .        .       .     //
//      U26     U13  Ladder_z  Type A  North   //
//---------------------------------------------//
//      U13     U26  Ladder_z  Type A  South   //
//      U12     U25     .        .       .     //
//      U11     U24     .        .       .     //
//      U10     U23     .        .       .     //
//      U9      U22     0        .       .     //
//      U8      U21     .        .       .     //
//      U7      U20     .        .       .     //
//      U6      U19  Ladder_z  Type A    .     //
//------------------------------------   .     //
//      U5      U18  Ladder_z  Type B    .     //
//      U4      U17     .        .       .     //
//      U3      U16     1        .       .     //
//      U2      U15     .        .       .     //
//      U1      U14  Ladder_z  Type B  South   //
//=============================================//

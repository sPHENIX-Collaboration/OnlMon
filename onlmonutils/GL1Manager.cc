
#include "GL1Manager.h"

#include <Event/eventReceiverClient.h>
#include <Event/Event.h>
#include <Event/packet.h>

#include <algorithm>

#define coutfl if (verbosity) std::cout << __FILE__<< "  " << __LINE__ << " "
//#define coutfl std::cout << __FILE__<< "  " << __LINE__ << " "

using namespace std;

GL1Manager::GL1Manager (const std::string &hostname, const int h, const int d)
{
  _hostname = hostname;
  _broken = 0;
  pgl1 = 0;

  history_length = h;
  max_discrepancy = d;

  Reset();

  erc = new eventReceiverClient(hostname);
}

GL1Manager::~GL1Manager()
{
  if (pgl1) delete pgl1;
  if ( erc) delete erc;
}


void GL1Manager::Reset()
{
  desireddiff = 0;
  previousdiff = 0;
  clockdiff = 0;
  event_delta = 0;
  eventMatch = 0;
  packetRequested = 0;
  gl1_clock = 0;
  init_done = 0;
  verbosity = 0;

  previous_GL1clocks.clear();
  previous_packetclocks.clear();
  
  diff_gl1clocks.clear();
  diff_packetclocks.clear();

  if (pgl1) delete pgl1;
  pgl1 = 0;

  return;
}


Packet *GL1Manager::getGL1Packet()
{
  if ( ! pgl1) return 0;
  packetRequested = 1;
  
  return pgl1;

}


int GL1Manager::getClockSync(const int evtnr, Packet * p)
{
  return ClockSync ( evtnr, p);
}


Packet * GL1Manager::fetchGL1Packet( const int evtnr)
{
  if (pgl1 && packetRequested == 0)  delete pgl1;
  pgl1 = 0;
  packetRequested = 0;

  coutfl << " asking for event " << evtnr + event_delta  << endl;
  Event *gl1Event = erc->getEvent(evtnr + event_delta);

  if ( !gl1Event) 
    {
      return 0;
    }

  pgl1 = gl1Event->getPacket(14001);
  if ( ! pgl1)
    {
      delete gl1Event;
      return 0;
    }

  pgl1->convert();

  delete gl1Event;

  return pgl1;
}



int GL1Manager::ClockSync(const int evtnr, Packet * p)
{

  pgl1 = fetchGL1Packet(evtnr);
  if ( !pgl1 ) return -1;
  
  // if we are getting to the next event. delete what's left of the previous one, if any
  gl1_clock = pgl1->lValue(0, "BCO"); 
  long long packet_clock = p->lValue(0, "CLOCK");

  // keep the 4 vectors trimmed at the envisioned size
  while ( previous_GL1clocks.size() >= history_length)
    {
      previous_GL1clocks.pop_front();
    }
  while ( previous_packetclocks.size() >= history_length)
    {
      previous_packetclocks.pop_front();
    }


  // we will add to the vector in a moment. Remove the oldest element 
  while ( diff_gl1clocks.size() >= history_length)
    { 
      diff_gl1clocks.pop_front();
    }
  
  while ( diff_packetclocks.size() >= history_length)
    {
      diff_packetclocks.pop_front();
    }




  if (previous_GL1clocks.size())  // this starts at the 2nd event
    {
      uint32_t d = gl1_clock - previous_GL1clocks.back();
      diff_gl1clocks.push_back(d);
    }
  
  if (previous_packetclocks.size())
    {
      uint32_t d = packet_clock - previous_packetclocks.back();
      diff_packetclocks.push_back(d);
    }



  previous_GL1clocks.push_back(gl1_clock);
  previous_packetclocks.push_back(packet_clock);


  // if we reach the desired size, we declare ourselves done with the init 
  if  (init_done == 0 && diff_gl1clocks.size() >= history_length)
    {
      init_done = 1;
      event_delta = findDelta();
      
      auto xx = previous_GL1clocks.begin() + event_delta;
      auto yy = previous_packetclocks.begin();

      desireddiff = ( *xx - *yy ) & 0xffffffff;
      return GL1Manager_INITIALIZING;
    }
  if (init_done ==0 ) return GL1Manager_INITIALIZING;


  previousdiff = clockdiff;
  clockdiff = (gl1_clock - packet_clock) & 0xffffffff;
  
  if ( desireddiff == clockdiff)
    {

      eventMatch = 1;
      return GL1Manager_MATCH;
    }
  else
    {
      // did we perhaps skip an rcdaq event?
      
      Reset();
      //event_delta--;
      eventMatch = 0;
      return GL1Manager_MISMATCH;
    }
}

uint32_t GL1Manager::getPacketClockDifference (Packet * p) const
{

  long long packet_clock = p->lValue(0, "CLOCK");

  uint32_t cd = (gl1_clock - packet_clock) & 0xffffffff;
  return cd;
}


int GL1Manager::checkPacket (Packet * p) const
{
  uint32_t cd = getPacketClockDifference ( p);
  if ( cd == desireddiff) return 0;
  return 1;
}

int GL1Manager::findDelta()
{

  if ( diff_gl1clocks.size() < history_length) return 2;


  // check "packet" against "GL1" first


  auto it = std::search(diff_gl1clocks.begin(), diff_gl1clocks.end(), diff_packetclocks.begin(), diff_packetclocks.begin() + diff_packetclocks.size() - max_discrepancy );

  int ret = 0;

  if ( it != diff_gl1clocks.end())
    {

      ret = std::distance(diff_gl1clocks.begin(), it);
      return ret;

    }
  // else
  //   {
  //     cout << " no match of packets in GL1 " << endl;
  //   }

  auto it2 = std::search(diff_packetclocks.begin(), diff_packetclocks.end(), diff_gl1clocks.begin(), diff_gl1clocks.begin() + diff_gl1clocks.size() - max_discrepancy  );

  if ( it2 != diff_packetclocks.end())
    {
      ret = -1 * std::distance(diff_packetclocks.begin(), it2);
    }
  // else
  //   {
  //     cout << " no match of packets in packet " << endl;
  //   }

  return ret;
}



 



//   uint32_t clockdiff = getPacketClockDifference ( p);
//   if ( clockdiff == desireddiff) return 0;
//   return 1;
// }



  






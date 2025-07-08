#ifndef __GL1MANAGER_H__
#define __GL1MANAGER_H__


#include <iostream>
#include <string>
#include <vector>
#include <deque>


enum GL1ManagerReturnCodes
{
  GL1Manager_MATCH = 0,
  GL1Manager_MISMATCH = 1,
  GL1Manager_INITIALIZING = 2
};




//#include <Event/eventReceiverClient.h>

class eventReceiverClient;
class Packet;


class GL1Manager
{
 public:

  GL1Manager (const char * /*hostname*/, const int /*history*/ = 6, const int /*maxdiscrepancy*/ = 3); // for eventReceiverclient
  ~GL1Manager();

  void Reset();

  int getClockSync(const int /*evtnr*/, Packet * /*p*/);

  int getDelta () const {return event_delta;};
  void setDelta (const int d ) {event_delta = d;};
  void setVerbosity (const int d ) {verbosity = d;};

  int checkPacket (Packet * /*p*/) const;
  uint32_t getPacketClockDifference (Packet * /*p*/) const;

  Packet * getGL1Packet();

 protected:

  // the generic function
  int ClockSync(const int evtnr, Packet * p);
  Packet * fetchGL1Packet(const int /*evtnr*/);
  int findDelta();
  
  Packet *pgl1;

  std::string _hostname;

  int eventMatch;
  int packetRequested;

  eventReceiverClient *erc;
  uint32_t desireddiff;
  uint32_t previousdiff;
  uint32_t clockdiff;
  long long gl1_clock;

  unsigned int history_length;
  int max_discrepancy;
  int init_done;

  std::deque<long long> previous_GL1clocks;
  std::deque<uint32_t> previous_packetclocks;

  std::deque<uint32_t> diff_gl1clocks;
  std::deque<uint32_t> diff_packetclocks;

  int event_delta;
  int _broken;
  int verbosity;
};

#endif


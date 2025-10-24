#include "ServerFuncs.C"

#include <onlmon/daq/DaqMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonldaqmon_server.so)

void run_daq_server(const std::string &name = "DAQMON", unsigned int serverid = 0, const std::string &prdffile = "/bbox/bbox4/HCal/junk/junk_seb16-00039892-0000.prdf")
{
  OnlMon *m = new DaqMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
// If running the eventServer_classic on the local host for offline debugging
// uncomment the m->SetEventReceiverClient("localhost");
//  m->SetEventReceiverClient("localhost");

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

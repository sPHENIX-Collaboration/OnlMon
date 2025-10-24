#include "ServerFuncs.C"

#include <onlmon/localpol/LocalPolMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonllocalpolmon_server.so)

void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/beam/GL1_beam_gl1daq-00041352-0000.evt")
{
  OnlMon *m = new LocalPolMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
// If running the eventServer_classic on the local host for offline debugging
// uncomment the m->SetEventReceiverClient("localhost");
//  m->SetEventReceiverClient("localhost");

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

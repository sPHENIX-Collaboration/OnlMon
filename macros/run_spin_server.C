#include "ServerFuncs.C"

#include <onlmon/spin/SpinMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlspinmon_server.so)
 
void run_spin_server(const std::string &name = "SPINMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/beam/GL1_beam_gl1daq-00041936-0000.evt")
{
  OnlMon *m = new SpinMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

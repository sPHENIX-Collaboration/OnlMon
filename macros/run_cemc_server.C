#include "ServerFuncs.C"

#include <onlmon/cemc/CemcMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_server.so)

void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphnxpro/commissioning/jsh/emcal/eventcombiner/junk-00035646-0000.prdf")
{
  OnlMon *m = new CemcMon(name);                    // create subsystem Monitor object
  m->SetMonitorServerId(serverid);

// If running the eventServer_classic on the local host for offline debugging
// uncomment the m->SetEventReceiverClient("localhost");
//  m->SetEventReceiverClient("localhost");

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  gStyle->SetOptStat(0);
  // cemc_runningmean->SetMinimum(0);
  return;
}

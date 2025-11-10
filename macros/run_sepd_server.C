#include "ServerFuncs.C"

#include <onlmon/sepd/SepdMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlsepdmon_server.so)

void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00068986-0000.prdf")
{
  OnlMon *m = new SepdMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
// If running the eventServer_classic on the local host for offline debugging
// uncomment the m->SetEventReceiverClient("localhost");
//  m->SetEventReceiverClient("localhost");

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

#define ONLINE
#include "ServerFuncs.C"

#include <onlmon/bbc/BbcMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlbbcmon_server.so)

void run_bbc_server(const std::string &name = "BBCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf")
{
  OnlMon *m = new BbcMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
// If running the eventServer_classic on the local host for offline debugging
// uncomment the m->SetEventReceiverClient("localhost");
//  m->SetEventReceiverClient("localhost");

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

#include "ServerFuncs.C"

#include <onlmon/pktsize/PktSizeMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlpktsizemon_server.so)

void run_pktsize_server(const std::string &name = "PKTSIZEMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/aligned/beam-00022982-0000.prdf")
{
  OnlMon *m = new PktSizeMon("PKTSIZEMON");                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

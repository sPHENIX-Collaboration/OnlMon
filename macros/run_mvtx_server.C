#include "ServerFuncs.C"

#include <onlmon/mvtx/MvtxMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlmvtxmon_server.so)

void run_mvtx_server(const std::string &name = "MVTXMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/MVTX/physics/physics_mvtx0-00042402-0000.evt")
{
  OnlMon *m = new MvtxMon(name);  // create subsystem Monitor object
  m->SetMonitorServerId(serverid);

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

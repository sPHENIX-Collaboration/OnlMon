#include "ServerFuncs.C"

#include <onlmon/zdc/ZdcMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlzdcmon_server.so)

void run_zdc_server(const std::string &name = "ZDCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00066522-0000.prdf")
{

  OnlMon *m = new ZdcMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
 
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

#define ONLINE
#include "ServerFuncs.C"

#include <onlmon/bbc/BbcMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlbbcmon_server.so)

void run_bbc_server(const std::string &name = "BBCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf")
{
  OnlMon *m = new BbcMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

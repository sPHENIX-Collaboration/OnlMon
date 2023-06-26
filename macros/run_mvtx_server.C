#include "ServerFuncs.C"

#include <onlmon/mvtx/MvtxMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_server.so)

void run_mvtx_server(const std::string &name = "MVTXMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data01/mvtx/test_00000096-0000.prdf")
{
  OnlMon *m = new MvtxMon(name);                    // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}



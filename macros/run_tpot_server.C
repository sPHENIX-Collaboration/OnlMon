#include <ServerFuncs.C>

#include <onlmon/tpot/TpotMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonltpotmon_server.so)

void run_tpot_server(
  const std::string &name = "TPOTMON",
  unsigned int serverid = 0, 
  const std::string &prdffile = "/phenix/u/hpereira/sphenix/work/g4simulations/RAW/TPOT_junk-00000003-0000.prdf"
  // const std::string &prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf"
  )
{
  OnlMon *m = new TpotMon(name);                    // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
  //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
  //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

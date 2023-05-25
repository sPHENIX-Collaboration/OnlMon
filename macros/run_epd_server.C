#include <ServerFuncs.C>

#include <onlmon/epd/EpdMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlepdmon_server.so)

void run_epd_server(const std::string &name = "EPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphenix/cemc/sepd/onlmon_test/EMCAL_to_SEPD_test.prdf")
{
  OnlMon *m = new EpdMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

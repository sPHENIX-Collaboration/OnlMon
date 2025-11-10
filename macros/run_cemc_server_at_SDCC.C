#include "ServerFuncs.C"

#include <onlmon/cemc/CemcMon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlcemcmon_server.so)

//  run an eventserver on the same machine this way:
//  eventServer [-i] -x -f /sphenix/lustre01/sphnxpro/physics/GL1/physics/GL1_physics_gl1daq-00046767-0000.evt

// void run_cemc_server(const char *prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf")
void run_cemc_server_at_SDCC(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/emcal/physics/physics_seb00-00046767-0000.prdf")
{
  OnlMon *m = new CemcMon(name, "localhost");                    // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  gStyle->SetOptStat(0);
  // cemc_runningmean->SetMinimum(0);
  return;
}

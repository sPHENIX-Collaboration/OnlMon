#include "ServerFuncs.C"

#include <onlmon/sepd/SepdMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlsepdmon_server.so)

//void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/beam/beam_seb20-00040853-0000.prdf")
//void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00053758-0001.prdf")
//void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00067587-0000.prdf")
void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00068986-0000.prdf")
{
  OnlMon *m = new SepdMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

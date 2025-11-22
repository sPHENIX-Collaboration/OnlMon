#include "ServerFuncs.C"

#include <onlmon/gl1/GL1Mon.h>

#include <onlmon/OnlMonServer.h>

R__LOAD_LIBRARY(libonlgl1mon_server.so)
 
void run_gl1_server(const std::string &name = "GL1MON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/GL1/physics/GL1_physics_gl1daq-00076682-0000.evt")
{
  OnlMon *m = new GL1Mon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
//  m->SetEventReceiverClient("localhost"); // for testing with client

  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

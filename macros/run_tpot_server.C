#include "ServerFuncs.C"

#include <onlmon/tpot/TpotMon.h>
#include <onlmon/OnlMonServer.h>

#include <fstream>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpotmon_server.so)

void run_tpot_server(
  const std::string &name = "TPOTMON",
  unsigned int serverid = 0,
  const std::string &evtfile = "/sphenix/lustre01/sphnxpro/physics/TPOT/physics/TPOT_ebdc39_physics-00051926-0000.evt"
  )
{
  // create subsystem Monitor object
  auto m = new TpotMon(name);
  m->SetMonitorServerId(serverid);
  m->set_max_sample(1024);
  m->set_sample_window_signal( {3, 18} );

  // prefer local calibration filename if exists
  const std::string local_calibration_filename( "TPOT_Pedestal-000.root" );
  if( std::ifstream( local_calibration_filename ).good() )
  { m->set_calibration_file( local_calibration_filename ); }

  // get pointer to Server Framework
  auto se = OnlMonServer::instance();

  // register subsystem Monitor with Framework
  se->registerMonitor(m);

  // and start
  start_server(evtfile);
  return;
}

#include <ServerFuncs.C>

#include <onlmon/intt/InttMonConstants.h>
#include <onlmon/intt/InttMon.h>
#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlinttmon_server.so)

//std::string _prdf_file = "/sphenix/u/jbertaux/evt_files/intt_intt0-00000025-0000.evt";
std::string _prdf_file = "/sphenix/u/jbertaux/evt_files/intt_intt2-00000023-0000.evt";

void run_intt_server(const std::string &prdffile = _prdf_file)
{
  const std::string &name = "INTTMON";
  unsigned int serverid = 0;
//  //for debugging
//  InttMon *m = new InttMon();
//  m->MiscDebug();

  std::cout << "Using .evt file:" << std::endl;
  std::cout << "\t" << prdffile << std::endl;

  OnlMon *m = new InttMon(name);      // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
//  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

//**********************************************************
//
// just a comment: This is not how this will be run
//
//**********************************************************
  //m->Init(); this must already be called somewhere above
  m->BeginRun(1);

  //int N = 1;
  //for(int n = 0; n < N; n++)
  //{
  //  m->process_event(static_cast<Event*>(0x0));
  //}

  return;
}

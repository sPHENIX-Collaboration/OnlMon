#include <onlmon/OnlMonServer.h>
#include <pmonitor/pmonitor.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmonserver.so)
R__LOAD_LIBRARY(libonlmonserver_funcs.so)
void CleanUpServer();

void start_server(const std::string &prdffile = "")
{
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  // set the ONLMONBBCLL1 Trigger definition (multiple triggers are possible)
  //  se->OnlTrig()->AddBbcLL1TrigName("BBCLL1(>0 tubes) narrowvtx");
  if (prdffile.empty())
  {
    cout << "No Input file given" << endl;
    return;
  }
  if (prdffile.find("seb") == 0 || prdffile.find("ebdc") == 0)
  {
    rcdaqopen(prdffile.c_str());
    prun();
    //  if the rcdaq server is terminated we execute the EndRun and then
    // save the histograms
    se->EndRun(se->RunNumber());  //
    se->WriteHistoFile();
    //      delete enablecorbabuf;
    CleanUpServer();
  }
  else if (prdffile == "et_test")
  {
    //      petopen("/tmp/Monitor@etpool");
  }
  else
  {
    //         gSystem->Load("libcorbamsgbuffer.so");
    //         corba_msg_buffer *enablecorbabuf = new corba_msg_buffer("monitor_event_channel");
    pfileopen(prdffile.c_str());
  }
  return;
}

void CleanUpServer()
{
  pclose();
  OnlMonServer *se = OnlMonServer::instance();
  delete se;
  gSystem->Exit(0);
}

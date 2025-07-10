#define ONLINE
#include <onlmon/bbc/BbcMonDraw.h>
#include <onlmon/cemc/CemcMonDraw.h>
//#include <onlmon/daq/DaqMonDraw.h>
#include <onlmon/hcal/HcalMonDraw.h>
#include <onlmon/intt/InttMonDraw.h>
#include <onlmon/ll1/LL1MonDraw.h>
//#include <onlmon/localpol/LocalPolMonDraw.h>
#include <onlmon/mvtx/MvtxMonDraw.h>
#include <onlmon/sepd/SepdMonDraw.h>
//#include <onlmon/spin/SpinMonDraw.h>
#include <onlmon/tpc/TpcMonDraw.h>
#include <onlmon/tpot/TpotMonDraw.h>
#include <onlmon/zdc/ZdcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlbbcmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_client.so)
// cppcheck-suppress unknownMacro
//R__LOAD_LIBRARY(libonldaqmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlhcalmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlinttmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlll1mon_client.so)
// cppcheck-suppress unknownMacro
//R__LOAD_LIBRARY(libonllocalpolmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlsepdmon_client.so)
// cppcheck-suppress unknownMacro
//R__LOAD_LIBRARY(libonlspinmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpcmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpotmon_client.so)
// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlzdcmon_client.so)

void makehtml(const std::string &filelist, const std::string &subsystem, int test = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *drawer = nullptr;
  std::cout << "If used with root.exe, use root.exe -q makehtml.C" << endl;
  std::cout << "If you use -q it is batch which does not have an Xserver"
	    << " to create gifs/png and this macro will fail miserably" << std::endl;
  if (subsystem == "BBCMON")
  {
    drawer = new BbcMonDraw("BBCMONDRAW");
  }
  else if (subsystem == "CEMCMON")
  {
    drawer = new CemcMonDraw("CEMCMONDRAW");
  }
  // else if (subsystem == "DAQMON")
  // {
  //   drawer = new DaqMonDraw("DAQMONDRAW");
  // }
  else if (subsystem == "IHCALMON")
  {
    drawer = new HcalMonDraw("IHCALMONDRAW");
  }
  else if (subsystem == "INTTMON")
  {
    drawer = new InttMonDraw("INTTMONDRAW");
  }
  else if (subsystem == "LL1MON")
  {
    drawer = new LL1MonDraw("LL1MONDRAW");
  }
  // else if (subsystem == "LOCALPOLMON")
  // {
  //   drawer = new LocalPolMonDraw("LOCALPOLMONDRAW");
  // }
  else if (subsystem == "MVTXMON")
  {
    drawer = new MvtxMonDraw("MVTXMONDRAW");
  }
  else if (subsystem == "OHCALMON")
  {
    drawer = new HcalMonDraw("OHCALMONDRAW");
  }
  else if (subsystem == "SEPDMON")
  {
    drawer = new SepdMonDraw("SEPDMONDRAW");
  }
  // else if (subsystem == "SPINMON")
  // {
  //   drawer = new SpinMonDraw("SPINMONDRAW");
  // }
  else if (subsystem == "TPCMON")
  {
    drawer = new TpcMonDraw("TPCMONDRAW");
  }
  else if (subsystem == "TPOTMON")
  {
    auto tpotmon = new TpotMonDraw("TPOTMONDRAW");

    // prefer local calibration filename if exists
    const std::string local_calibration_filename( "TPOT_Pedestal-000.root" );
    if( std::ifstream( local_calibration_filename ).good() )
    { tpotmon->set_calibration_file( local_calibration_filename ); }

    // adjust sample and signal windows
    tpotmon->set_sample_window( {0, 50} );
    tpotmon->set_sample_window_signal( {3, 18} );

    // assign
    drawer = tpotmon;
  }
  else if (subsystem == "ZDCMON")
  {
    drawer = new ZdcMonDraw("ZdcMONDRAW");
  }
  else
  {
    std::cout << "Invalid Subsystem " << subsystem << std::endl;
    gSystem->Exit(1);
  }
  cl->registerDrawer(drawer);
  ifstream listfile(filelist);
  if (listfile.is_open())
  {
    std::string line;
    while (std::getline(listfile, line))
    {
      cl->ReadHistogramsFromFile(line, drawer);
    }
    listfile.close();
  }
  else
  {
    std::cout << "Could not open " << filelist << std::endl;
    gSystem->Exit(1);
  }
  cl->Verbosity(1);
  cl->MakeHtml();
  if (test == 0)
  {
    delete cl;
    gSystem->Exit(0);
  }
}

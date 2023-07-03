#include "TpotMonDraw.h"
#include "TpotMonDefs.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

namespace
{
  //! make canvas editable in creator, and non-editable in destructor
  class CanvasEditor
  {
    public:
    CanvasEditor( TCanvas* cv ):m_cv(cv)
    { if( m_cv ) m_cv->SetEditable(true); }

    ~CanvasEditor()
    { if( m_cv ) m_cv->SetEditable(false); }

    private:
    TCanvas* m_cv = nullptr;
  };

  TPad* create_transparent_pad( const std::string& name )
  {
    auto transparent = new TPad( (name+"_transparent").c_str(), "", 0, 0, 1, 1);
    transparent->SetFillStyle(4000);
    transparent->Draw();
    return transparent;
  };

  TPad* get_transparent_pad( TPad* parent, const std::string& name, bool clear = true)
  {
    if( !parent ) return nullptr;
    const std::string transparent_name = name+"_transparent";
    auto out = dynamic_cast<TPad*>( parent->FindObject( transparent_name.c_str() ) );

    if( !out ) std::cout << "get_transparent_pad - " << transparent_name << " not found" << std::endl;
    if( clear ) out->Clear("D");
    return out;

  }

  // divide canvas, adjusting canvas positions to leave room for a banner at the top
  void divide_canvas( TCanvas* cv, int ncol, int nrow )
  {
    static constexpr double max_height = 0.94;

    cv->Divide( ncol, nrow );
    for( int i = 0; i < ncol*nrow; ++i )
    {
      auto pad = cv->GetPad( i+1 );
      int col = i%ncol;
      int row = i/ncol;
      const double xmin = double(col)/ncol;
      const double xmax = double(col+1)/ncol;

      const double ymin = max_height*(1. - double(row+1)/nrow);
      const double ymax = max_height*(1. - double(row)/nrow);
      pad->SetPad( xmin, ymin, xmax, ymax );
    }

  }
}

//__________________________________________________________________________________
TpotMonDraw::TpotMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars.reset( new OnlMonDB(ThisName) );

  // initialize local list of detector names
  for( const auto& fee_id:m_mapping.get_fee_id_list() )
  { m_detnames_sphenix.push_back( m_mapping.get_detname_sphenix( fee_id ) ); }

}

//__________________________________________________________________________________
int TpotMonDraw::Init()
{ return 0; }

//__________________________________________________________________________________
TCanvas* TpotMonDraw::get_canvas(const std::string& name, bool clear )
{
  auto cv = dynamic_cast<TCanvas*>( gROOT->FindObject( name.c_str() ) );
  if( !cv ) cv = create_canvas( name );
  if( cv && clear ) cv->Clear("D");
  return cv;
}

//__________________________________________________________________________________
TCanvas* TpotMonDraw::create_canvas(const std::string &name)
{

  if( Verbosity() )
  { std::cout << "TpotMonDraw::create_canvas - name: " << name << std::endl; }

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  int cv_id = 0;
  if (name == "TPOT_detector_occupancy")
  {

    // xpos (-1) negative: do not draw menu bar
    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TPOT detector occupancy", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 1, 2 );
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;

  } else if (name == "TPOT_resist_occupancy") {

    // xpos (-1) negative: do not draw menu bar
    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TPOT resist occupancy", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 1, 2 );
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;

  } else if (name == "TPOT_adc_vs_sample") {

    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TpotMon adc vs sample", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 4, 4 );
    create_transparent_pad(name)->Draw();
    for( int i = 0; i < 16; ++i )
    {
      cv->GetPad(i+1)->SetLeftMargin(0.15);
      cv->GetPad(i+1)->SetRightMargin(0.02);
    }
    cv->SetEditable(false);
    return cv;

  } else if (name == "TPOT_hit_charge") {

    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TpotMon hit charge", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 4, 4 );
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;

  } else if (name == "TPOT_hit_multiplicity") {

    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TpotMon hit multiplicity", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 4, 4 );
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;

  } else if (name == "TPOT_hit_vs_channel") {

    auto cv = m_canvas[cv_id++] = new TCanvas(name.c_str(), "TpotMon hit vs channel", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas( cv, 4, 4 );
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;

  }
  return nullptr;
}

//_______________________________________________________________________________
int TpotMonDraw::Draw(const std::string &what)
{
  if( Verbosity() )
  { std::cout << "TpotMonDraw::Draw - what: " << what << std::endl; }

  int iret = 0;
  int idraw = 0;

  {
    // get counters
    const auto cl = OnlMonClient::instance();
    const auto m_counters = cl->getHisto("TPOTMON_0","m_counters");
    if( m_counters && Verbosity() )
    {
      const int events = m_counters->GetBinContent( TpotMonDefs::kEventCounter );
      const int valid_events = m_counters->GetBinContent( TpotMonDefs::kValidEventCounter );
      std::cout << "TpotMonDraw::Draw - events: " << events << " valid events: " << valid_events << std::endl;
    }
  }

  if( what == "ALL" || what == "TPOT_detector_occupancy" )
  {
    iret += draw_detector_occupancy();
    ++idraw;
  }

  if( what == "ALL" || what == "TPOT_resist_occupancy" )
  {
    iret += draw_resist_occupancy();
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_adc_vs_sample")
  {
    iret += draw_array("TPOT_adc_vs_sample", get_histograms( "m_adc_sample" ), "col" );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_charge")
  {
    iret += draw_array("TPOT_hit_charge", get_histograms( "m_hit_charge" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_multiplicity")
  {
    iret += draw_array("TPOT_hit_multiplicity", get_histograms( "m_hit_multiplicity" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_vs_channel")
  {
    iret += draw_array("TPOT_hit_vs_channel", get_histograms( "m_hit_vs_channel" ) );
    ++idraw;
  }

  if (!idraw)
  {
    std::cout << "TpotMonDraw::Draw - Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

//_______________________________________________________________________________
int TpotMonDraw::MakePS(const std::string &what)
{
  auto cl = OnlMonClient::instance();
  const int iret = Draw(what);
  if (iret) return iret;

  for( std::size_t i =0; i < m_canvas.size(); ++i )
  {
    const auto& cv = m_canvas[i];
    if( cv )
    {
      std::ostringstream filename;
      filename << ThisName << "_" << i+1 << "_" << cl->RunNumber() << ".ps";
      cv->Print( filename.str().c_str() );
    }
  }

  return 0;
}

//__________________________________________________________________________________
int TpotMonDraw::MakeHtml(const std::string &what)
{
  const int iret = Draw(what);
  if (iret) return iret;

  auto cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  for( std::size_t i =0; i < m_canvas.size(); ++i )
  {
    const auto& cv = m_canvas[i];
    if( cv )
    {
      const auto pngfile = cl->htmlRegisterPage(*this, cv->GetName(), Form("%lu", i+1), "png");
      cl->CanvasToPng(cv, pngfile);
    }
  }

  // log
  {
    const std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
    std::ofstream out(logfile.c_str());
    out
      << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
      << "</TITLE></HEAD>"
      << std::endl
      << "<P>Some log file output would go here."
      << std::endl;
    out.close();
  }

  // status
  {
    const std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
    std::ofstream out(status.c_str());
    out
      << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
      << "</TITLE></HEAD>"
      << std::endl
      << "<P>Some status output would go here."
      << std::endl;
    out.close();
    cl->SaveLogFile(*this);
  }

  return 0;
}

//__________________________________________________________________________________
void TpotMonDraw::draw_time( TPad* pad )
{
  pad->Clear();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment

  std::ostringstream runnostream;
  auto cl = OnlMonClient::instance();
  time_t evttime = cl->EventTime("TPOTMON_0", "CURRENT");

  runnostream
    << ThisName << " Run " << cl->RunNumber()
    << ", Time: " << ctime(&evttime);

  pad->cd();
  PrintRun.DrawText(0.5, 0.98, runnostream.str().c_str());
}

//__________________________________________________________________________________
int TpotMonDraw::draw_detector_occupancy()
{

  if( Verbosity() ) std::cout << "TpotMonDraw::draw_detector_occupancy" << std::endl;

  // get histograms
  auto cl = OnlMonClient::instance();
  auto m_detector_occupancy_phi =  cl->getHisto("TPOTMON_0","m_detector_occupancy_phi");
  auto m_detector_occupancy_z =  cl->getHisto("TPOTMON_0","m_detector_occupancy_z");

  auto cv = get_canvas("TPOT_detector_occupancy");
  auto transparent = get_transparent_pad( cv, "TPOT_detector_occupancy");
  if( !cv )
  {
    if( Verbosity() ) std::cout << "TpotMonDraw::draw_detector_occupancy - no canvas" << std::endl;
    return -1;
  }

  CanvasEditor cv_edit(cv);

  if( m_detector_occupancy_phi && m_detector_occupancy_z )
  {
    cv->cd(1);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    m_detector_occupancy_z->DrawCopy( "colz" );
    draw_detnames_sphenix();

    cv->cd(2);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    m_detector_occupancy_phi->DrawCopy( "colz" );
    draw_detnames_sphenix();

    if( transparent ) draw_time(transparent);
    return 0;

  } else {

    if( transparent ) DrawDeadServer(transparent);
    return -1;

  }
}

//__________________________________________________________________________________
int TpotMonDraw::draw_resist_occupancy()
{

  if( Verbosity() ) std::cout << "TpotMonDraw::draw_resist_occupancy" << std::endl;

  // get histograms
  auto cl = OnlMonClient::instance();
  auto m_resist_occupancy_phi =  cl->getHisto("TPOTMON_0","m_resist_occupancy_phi");
  auto m_resist_occupancy_z =  cl->getHisto("TPOTMON_0","m_resist_occupancy_z");

  auto cv = get_canvas("TPOT_resist_occupancy");
  auto transparent = get_transparent_pad( cv, "TPOT_resist_occupancy");
  if( !cv )
  {
    if( Verbosity() ) std::cout << "TpotMonDraw::draw_resist_occupancy - no canvas" << std::endl;
    return -1;
  }

  CanvasEditor cv_edit(cv);

  if( m_resist_occupancy_phi && m_resist_occupancy_z )
  {
    cv->cd(1);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    m_resist_occupancy_z->DrawCopy( "colz" );
    draw_detnames_sphenix();

    cv->cd(2);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    m_resist_occupancy_phi->DrawCopy( "colz" );
    draw_detnames_sphenix();

    if( transparent ) draw_time(transparent);
    return 0;

  } else {

    if( transparent ) DrawDeadServer(transparent);
    return -1;

  }
}

//__________________________________________________________________________________
void TpotMonDraw::draw_detnames_sphenix()
{
  gPad->Update();
  for( size_t i = 0; i < m_geometry.get_ntiles(); ++i )
  {
    const auto name = m_geometry.get_detname_sphenix(i);
    const auto [x,y] = m_geometry.get_tile_center(i);
    auto text = new TText();
    // text->SetNDC( true );
    text->DrawText( x-0.8*m_geometry.m_tile_length/2, y-0.8*m_geometry.m_tile_width/2, name.c_str() );
    text->Draw();
  }

}

//__________________________________________________________________________________
TpotMonDraw::histogram_array_t TpotMonDraw::get_histograms( const std::string& name )
{
  histogram_array_t out{{nullptr}};

  auto cl = OnlMonClient::instance();
  for( size_t i=0; i<m_detnames_sphenix.size(); ++i)
  {
    const auto& detector_name=m_detnames_sphenix[i];
    const auto hname = name + "_" + detector_name;
    out[i] =  cl->getHisto("TPOTMON_0", hname );
    if( Verbosity() )
    { std::cout << "TpotMonDraw::get_histograms - " << hname << (out[i]?" found":" not found" ) << std::endl; }
  }

  return out;
}

//__________________________________________________________________________________
int TpotMonDraw::draw_array( const std::string& name, const TpotMonDraw::histogram_array_t& histograms, const std::string& option )
{
  if( Verbosity() ) std::cout << "TpotMonDraw::draw_array - name: " << name << std::endl;

  auto cv = get_canvas(name);
  auto transparent = get_transparent_pad( cv, name);
  if( !cv ) return -1;

  bool drawn = false;
  CanvasEditor cv_edit(cv);
  for( size_t i = 0; i < histograms.size(); ++i )
  {
    if( histograms[i] )
    {
      cv->cd(i+1);
      histograms[i]->DrawCopy( option.c_str() );
      gPad->SetBottomMargin(0.12);
      drawn = true;
    }
  }

  if( drawn )
  {
    if( transparent ) draw_time(transparent);
    return 0;
  } else {
    if( transparent ) DrawDeadServer(transparent);
    return -1;
  }
}

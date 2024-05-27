#include "TpotMonDraw.h"
#include "TpotMonDefs.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <micromegas/MicromegasCalibrationData.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPad.h>
#include <TPaveText.h>
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
    // {}
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
    if( out && clear ) out->Clear("D");
    return out;

  }

  // draw an vertical line that extends automatically from both sides of a canvas
  [[maybe_unused]] TLine* vertical_line( TVirtualPad* pad, Double_t x )
  {
    Double_t yMin = pad->GetUymin();
    Double_t yMax = pad->GetUymax();

    if( pad->GetLogy() )
    {
      yMin = std::pow( 10, yMin );
      yMax = std::pow( 10, yMax );
    }

    return new TLine( x, yMin, x, yMax );
  }

  // draw an horizontal line that extends automatically from both sides of a canvas
  [[maybe_unused]] TLine* horizontal_line( TVirtualPad* pad, Double_t y )
  {
    Double_t xMin = pad->GetUxmin();
    Double_t xMax = pad->GetUxmax();

    if( pad->GetLogx() )
    {
      xMin = std::pow( 10, xMin );
      xMax = std::pow( 10, xMax );
    }

    return new TLine( xMin, y, xMax, y );
  }

  // draw text in relative coordinate
  TLatex* draw_text( Double_t x_ndc, Double_t y_ndc, const TString& value, double text_size = 0.1 )
  {
    auto text = new TLatex;
    text->SetNDC( true );
    text->SetTextColor(1);
    text->SetTextSize(text_size);
    text->DrawLatex( x_ndc, y_ndc, value );
    return text;
  }

  void mask_scoz( double xmin, double ymin, double xmax, double ymax )
  {
    auto text = new TPaveText(xmin, ymin, xmax, ymax, "NDC" );
    text->SetFillColor(0);
    text->SetFillStyle(1001);
    text->SetBorderSize(1);
    text->SetTextAlign(11);
    text->AddText( "   Ignore   " );
    text->Draw();
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

  // hide margins between pads
  void hide_margins( TCanvas* cv, const double left_margin = 0.15, const double bottom_margin = 0.17 )
  {
    static constexpr double max_height = 0.94;
    const double height = 1./(4.+bottom_margin);
    const double width = 1./(4.+left_margin);
    for( int row = 0; row < 4; ++row )
    {
      double ymin = row < 3 ? max_height*(1.-double(row+1)*height):0;
      double ymax = max_height*(1.-double(row)*height);

      for( int column = 0; column < 4; ++column )
      {
        double xmin = column == 0 ? 0:width*(column+left_margin);
        double xmax = width*(column+1+left_margin);

        const int i = column + 4*row;
        const auto pad = cv->GetPad(i+1);
        pad->SetPad( xmin, ymin, xmax, ymax );

        pad->SetTopMargin(0);

        if(row<3) pad->SetBottomMargin(0);
        else pad->SetBottomMargin(bottom_margin);

        if(column>0) pad->SetLeftMargin(0);
        else pad->SetLeftMargin(left_margin);

        if(column<3) pad->SetRightMargin(0);
        else pad->SetRightMargin(0.01);

        // draw ticks on both sides
        pad->SetTicky();

      }
    }
  }

  // streamer for sample window
  std::ostream& operator << ( std::ostream&o, const TpotMonDraw::sample_window_t& window )
  {
    o << "{ " << window.first << ", " << window.second << "}";
    return o;
  }

}

//__________________________________________________________________________________
TpotMonDraw::TpotMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  // setup default calibration filename
  // note: this can be overriden by calling set_calibration_filename from the parent macro
  const auto tpotcalib = getenv("TPOTCALIB");
  if (!tpotcalib)
  {
    std::cout << "TpotMon::TpotMon - TPOTCALIB environment variable not set" << std::endl;
    exit(1);
  }

  m_calibration_filename = std::string(tpotcalib) + "/" + "TPOT_Pedestal-000.root";

  // setup default filename for reference histograms
  const auto tpotcalibref = getenv("TPOTCALIBREF");
  if( tpotcalibref )
  {
    m_ref_histograms_filename = std::string(tpotcalibref) + "/" + "Run_00000-TPOTMON_0.root";
    std::cout << "TpotMon::TpotMon - reading reference histograms from: " << m_ref_histograms_filename << std::endl;
    m_ref_histograms_tfile.reset( TFile::Open( m_ref_histograms_filename.c_str(), "READ" ) );
  } else {
    m_ref_histograms_filename = "Run_00000-TPOTMON_0.root";
    std::cout << "TpotMon::TpotMon - TPOTCALIBREF environment variable not set. Reading reference histograms from: " << m_ref_histograms_filename << std::endl;
    m_ref_histograms_tfile.reset( TFile::Open( m_ref_histograms_filename.c_str(), "READ" ) );
  }

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
{
  if( Verbosity() )
  {
    std::cout << "TpotMonDraw::Init - m_calibration_filename: " << m_calibration_filename << std::endl;
    std::cout << "TpotMonDraw::Init - m_sample_window: " << m_sample_window << std::endl;
    std::cout << "TpotMonDraw::Init - m_sample_window_signal: " << m_sample_window_signal << std::endl;
    std::cout << "TpotMon::Init - m_n_sigma: " << m_n_sigma << std::endl;
  }

  // setup calibrations
  if( std::ifstream( m_calibration_filename.c_str() ).good() )
  {

    MicromegasCalibrationData calibration_data;
    calibration_data.read( m_calibration_filename );

    // get fee ids
    const auto fee_id_list = m_mapping.get_fee_id_list();

    // loop over FEES
    for( int i = 0; i < MicromegasDefs::m_nfee; ++i)
    {

      // get fee_id
      const int fee_id = fee_id_list[i];

      // reset mean
      m_mean_thresholds[i] = 0;
      unsigned int count = 0;

      // create histogram
      std::string hname = std::string( "h_threshold_" ) + m_detnames_sphenix[i];
      auto h = new TH1F( hname.c_str(), hname.c_str(), MicromegasDefs::m_nchannels_fee, 0, MicromegasDefs::m_nchannels_fee );

      // set range
      h->SetMinimum(0);
      h->SetMaximum(1024);
      h->SetLineColor(2);

      // store histograms
      m_threshold_histograms[i] = h;

      // set values
      for( int channel = 0; channel < MicromegasDefs::m_nchannels_fee; ++channel )
      {

        // get channel rms and pedestal from calibration data
        const double pedestal = calibration_data.get_pedestal( fee_id, channel );
        const double rms = calibration_data.get_rms( fee_id, channel );
        const double threshold = pedestal + m_n_sigma * rms;
        const auto strip_index = m_mapping.get_physical_strip(fee_id, channel );

        // fill histogram
        h->SetBinContent( strip_index+1, threshold );

        if( rms > 0 )
        {
          // increment average
          m_mean_thresholds[i] += threshold;
          ++count;
        }
      }

      if(count > 0) {m_mean_thresholds[i]/=count;}
    }

  } else {
    std::cout << "TpotMonDraw::Init -"
      << " file " << m_calibration_filename << " cannot be opened."
      << " No calibration loaded"
      << std::endl;
  }

  return 0;
}

//__________________________________________________________________________________
int TpotMonDraw::DrawDeadServer( TPad* pad )
{
  if( !pad )
  {
    if( Verbosity() ) std::cout << "TpotMonDraw::DrawDeadServer - invalid pad" << std::endl;
    return 0;
  } else {
    pad->SetPad(0,0,1,1);
    return OnlMonDraw::DrawDeadServer( pad );
  }
}

//__________________________________________________________________________________
void TpotMonDraw::draw_time( TPad* pad )
{
  if( !pad )
  {
    if( Verbosity() ) std::cout << "TpotMonDraw::draw_time - invalid pad" << std::endl;
    return;
  }

  pad->SetPad( 0, 0.95, 1, 1 );
  pad->Clear();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.6);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment

  std::ostringstream runnostream;
  auto cl = OnlMonClient::instance();
  time_t evttime = cl->EventTime("CURRENT");

  runnostream
    << ThisName << " Run " << cl->RunNumber()
    << ", Time: " << ctime(&evttime);

  pad->cd();
  PrintRun.DrawText(0.5, 0.5, runnostream.str().c_str());
}

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

  if (name == "TPOT_counters")
  {

    auto cv = new TCanvas(name.c_str(), "TPOT event counters", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 1, 1);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_detector_occupancy") {

    auto cv = new TCanvas(name.c_str(), "TPOT detector occupancy", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 1, 2);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_resist_occupancy") {

    auto cv = new TCanvas(name.c_str(), "TPOT resist occupancy", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 1, 2);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_adc_vs_sample") {

    auto cv = new TCanvas(name.c_str(), "TpotMon adc vs sample", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv,0.2);
    create_transparent_pad(name);

    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_adc_vs_channel") {

    auto cv = new TCanvas(name.c_str(), "TpotMon adc vs channel", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv,0.2);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_counts_vs_sample") {

    auto cv = new TCanvas(name.c_str(), "TpotMon counts vs sample", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv,0.2);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_hit_charge") {

    auto cv = new TCanvas(name.c_str(), "TpotMon hit charge", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_hit_multiplicity") {

    auto cv = new TCanvas(name.c_str(), "TpotMon hit multiplicity", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
    return cv;

  } else if (name == "TPOT_hit_vs_channel") {

    auto cv = new TCanvas(name.c_str(), "TpotMon hit vs channel", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    divide_canvas(cv, 4, 4);
    hide_margins(cv,0.2);
    create_transparent_pad(name);
    cv->SetEditable(false);
    m_canvas.push_back( cv );
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
    const auto m_counters = get_histogram( "m_counters");
    if( m_counters )
    {
      m_triggercnt =  m_counters->GetBinContent( TpotMonDefs::kTriggerCounter );
    } else {
      m_triggercnt = 0;
    }

    if( m_counters && Verbosity() )
    {
      const int events = m_counters->GetBinContent( TpotMonDefs::kEventCounter );
      const int valid_events = m_counters->GetBinContent( TpotMonDefs::kValidEventCounter );
      std::cout << "TpotMonDraw::Draw - RCDAQ frames: " << events << " valid RCDAQ frames: " << valid_events << std::endl;
    }
  }

  if( what == "ALL" || what == "TPOT_counters" )
  {
    iret += draw_counters();
    ++idraw;
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

    // adjust histogram range
    auto h_array = get_histograms( "m_adc_sample" );
    for( const auto& h:h_array )
    {
      if( h )
      { h->GetXaxis()->SetRangeUser( m_sample_window.first, m_sample_window.second ); }
    }

    iret += draw_array("TPOT_adc_vs_sample", h_array, DrawOptions::Colz|DrawOptions::Logz );
    auto cv = get_canvas("TPOT_adc_vs_sample");
    if( cv )
    {
      CanvasEditor cv_edit(cv);
      cv->Update();

      for( int i = 0; i < MicromegasDefs::m_nfee; ++i )
      {
        // draw vertical lines that match sample window
        auto&& pad = cv->GetPad(i+1);
        pad->cd();
        pad->Update();
        for( const auto line:{vertical_line( pad, m_sample_window_signal.first ), vertical_line( pad, m_sample_window_signal.second ) } )
        {
          line->SetLineStyle(2);
          line->SetLineColor(2);
          line->SetLineWidth(2);
          line->Draw();
        }

        // also draw horizontal line at average threshold
        if( m_mean_thresholds[i] > 0 )
        {
          auto line = horizontal_line( pad, m_mean_thresholds[i] );
          line->SetLineStyle(2);
          line->SetLineColor(2);
          line->SetLineWidth(2);
          line->Draw();
        }
      }
    }
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_adc_vs_channel")
  {
    iret += draw_array("TPOT_adc_vs_channel", get_histograms( "m_adc_channel" ), DrawOptions::Colz|DrawOptions::Logz );
    auto cv = get_canvas("TPOT_adc_vs_channel");
    if( cv )
    {
      CanvasEditor cv_edit(cv);
      cv->Update();
      for( int i = 0; i < MicromegasDefs::m_nfee; ++i )
      {

        auto&& pad = cv->GetPad(i+1);
        pad->cd();

        // draw threshold
        if( m_threshold_histograms[i] )
        { m_threshold_histograms[i]->Draw("h same"); }

        // draw vertical lines that match HV sectors
        for( const int& channel:{64, 128, 196} )
        {
          const auto line = vertical_line( pad, channel );
          line->SetLineStyle(2);
          line->SetLineColor(1);
          line->SetLineWidth(1);
          line->Draw();
        }
      }

      {
        // maks scoz
        auto&& pad = cv->GetPad(9);
        pad->cd();
        mask_scoz(0.22,0.02,0.58, 0.98);
      }

    }
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_counts_vs_sample")
  {
    // adjust histogram range
    auto h_array = get_histograms( "m_counts_sample" );
    for( const auto& h:h_array )
    {
      if( h )
      {
        h->GetXaxis()->SetRangeUser( m_sample_window.first, m_sample_window.second );
        h->SetMinimum(0);
      }
    }

    // iret += draw_array("TPOT_counts_vs_sample", h_array, get_ref_histograms_scaled( "m_counts_sample" ), DrawOptions::MatchRange );
    iret += draw_array("TPOT_counts_vs_sample", h_array, get_ref_histograms_scaled( "m_counts_sample" ) );
    auto cv = get_canvas("TPOT_counts_vs_sample");
    if( cv )
    {
      CanvasEditor cv_edit(cv);
      cv->Update();
      for( int i = 0; i < MicromegasDefs::m_nfee; ++i )
      {
        // draw vertical lines that match sample window
        auto&& pad = cv->GetPad(i+1);
        pad->cd();
        for( const auto line:{vertical_line( pad, m_sample_window_signal.first ), vertical_line( pad, m_sample_window_signal.second ) } )
        {
          line->SetLineStyle(2);
          line->SetLineColor(2);
          line->SetLineWidth(2);
          line->Draw();
        }
      }
    }
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_charge")
  {
    iret += draw_array("TPOT_hit_charge", get_histograms( "m_hit_charge" ), get_ref_histograms_scaled( "m_hit_charge" ), DrawOptions::Logy|DrawOptions::MatchRange );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_multiplicity")
  {
    iret += draw_array("TPOT_hit_multiplicity", get_histograms( "m_hit_multiplicity" ), get_ref_histograms_scaled( "m_hit_multiplicity" ), DrawOptions::Logy|DrawOptions::MatchRange );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_vs_channel")
  {
    iret += draw_array("TPOT_hit_vs_channel", get_histograms( "m_hit_vs_channel" ), get_ref_histograms_scaled( "m_hit_vs_channel" ), DrawOptions::Logy|DrawOptions::MatchRange|DrawOptions::Normalize);
    auto cv = get_canvas("TPOT_hit_vs_channel");
    if( cv )
    {
      CanvasEditor cv_edit(cv);
      cv->Update();
      for( int i = 0; i < MicromegasDefs::m_nfee; ++i )
      {
        // draw vertical lines that match HV sectors
        // also set log y
        auto&& pad = cv->GetPad(i+1);
        pad->cd();
        pad->Update();
        for( const int& channel:{64, 128, 196} )
        {
          const auto line = vertical_line( pad, channel );
          line->SetLineStyle(2);
          line->SetLineColor(1);
          line->SetLineWidth(1);
          line->Draw();
        }

      }

      {
        // maks scoz
        auto&& pad = cv->GetPad(9);
        pad->cd();
        mask_scoz(0.22,0.02,0.58, 0.98);
        // mask_scoz(0.17,0.02,0.55, 0.98);
      }
    }

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
int TpotMonDraw::SavePlot(const std::string &what, const std::string &type)
{
  auto cl = OnlMonClient::instance();
  const int iret = Draw(what);
  if (iret) return iret;

  for( std::size_t i =0; i < m_canvas.size(); ++i )
  {
    const auto& cv = m_canvas[i];
    if( cv )
    {
    std::string filename = ThisName + "_" + std::to_string(i+1) + "_" +
      std::to_string(cl->RunNumber()) + "." + type;
     cl->CanvasToPng(cv, filename);
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
int TpotMonDraw::draw_counters()
{

  if( Verbosity() ) std::cout << "TpotMonDraw::draw_counters" << std::endl;

  // get histograms
  auto m_counters =  get_histogram( "m_counters");
  std::unique_ptr<TH1> m_counters_ref( normalize( get_ref_histogram( "m_counters" ), get_ref_scale_factor() ) );

  auto cv = get_canvas("TPOT_counters");
  auto transparent = get_transparent_pad( cv, "TPOT_counters");
  if( !cv )
  {
    if( Verbosity() ) std::cout << "TpotMonDraw::draw_counters - no canvas" << std::endl;
    return -1;
  }

  CanvasEditor cv_edit(cv);

  if( m_counters )
  {
    m_counters->SetMinimum(0);

    cv->cd(1);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    m_counters->SetFillStyle(1001);
    m_counters->SetFillColor(kYellow );
    auto copy = m_counters->DrawCopy( "h" );
    copy->SetStats(false);

    if( m_counters_ref )
    {
      m_counters_ref->SetLineColor(2);
      m_counters_ref->DrawCopy( "hist same" );
    }

    draw_time(transparent);
    return 0;
  } else {

    DrawDeadServer(transparent);
    return -1;
  }
}

//__________________________________________________________________________________
int TpotMonDraw::draw_detector_occupancy()
{

  if( Verbosity() ) std::cout << "TpotMonDraw::draw_detector_occupancy" << std::endl;

  // get histograms
  auto m_detector_occupancy_phi =  get_histogram( "m_detector_occupancy_phi");
  auto m_detector_occupancy_z =  get_histogram( "m_detector_occupancy_z");

  // turn off stat panel
  for( const auto& h:{m_detector_occupancy_phi,m_detector_occupancy_z} )
  { if(h) h->SetStats(0); }

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
    auto copy = m_detector_occupancy_z->DrawCopy( "colz" );
    copy->SetStats(false);
    copy->GetXaxis()->SetTitleOffset(1);
    copy->GetYaxis()->SetTitleOffset(0.65);
    draw_detnames_sphenix( "Z" );


    cv->cd(2);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    copy = m_detector_occupancy_phi->DrawCopy( "colz" );
    copy->SetStats(false);
    copy->GetXaxis()->SetTitleOffset(1);
    copy->GetYaxis()->SetTitleOffset(0.65);
    draw_detnames_sphenix( "P" );

    draw_time(transparent);
    return 0;

  } else {

    DrawDeadServer(transparent);
    return -1;

  }
}

//__________________________________________________________________________________
int TpotMonDraw::draw_resist_occupancy()
{

  if( Verbosity() ) std::cout << "TpotMonDraw::draw_resist_occupancy" << std::endl;

  // get histograms
  auto m_resist_occupancy_phi =  get_histogram( "m_resist_occupancy_phi");
  auto m_resist_occupancy_z =  get_histogram( "m_resist_occupancy_z");

  // turn off stat panel
  for( const auto& h:{m_resist_occupancy_phi,m_resist_occupancy_z} )
  { if(h) h->SetStats(0); }

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
    auto copy = m_resist_occupancy_z->DrawCopy( "colz" );
    copy->SetStats(false);
    copy->GetXaxis()->SetTitleOffset(1);
    copy->GetYaxis()->SetTitleOffset(0.65);
    draw_detnames_sphenix( "Z" );

    mask_scoz(0.1,0.4,0.18, 0.6);

    cv->cd(2);
    gPad->SetLeftMargin( 0.07 );
    gPad->SetRightMargin( 0.15 );
    copy = m_resist_occupancy_phi->DrawCopy( "colz" );
    copy->SetStats(false);
    copy->GetXaxis()->SetTitleOffset(1);
    copy->GetYaxis()->SetTitleOffset(0.65);
    draw_detnames_sphenix( "P" );

    draw_time(transparent);
    return 0;

  } else {
    DrawDeadServer(transparent);
    return -1;
  }
}

//__________________________________________________________________________________
void TpotMonDraw::draw_detnames_sphenix( const std::string& suffix)
{
  gPad->Update();
  for( size_t i = 0; i < m_geometry.get_ntiles(); ++i )
  {
    const auto name = m_geometry.get_detname_sphenix(i)+suffix;
    const auto [x,y] = m_geometry.get_tile_center(i);
    auto text = new TText();
    text->DrawText( x-0.8*m_geometry.m_tile_length/2, y-0.8*m_geometry.m_tile_width/2, name.c_str() );
    text->Draw();
  }

}

//__________________________________________________________________________________
TH1* TpotMonDraw::get_histogram( const std::string& name ) const
{
  auto cl = OnlMonClient::instance();
  return cl->getHisto("TPOTMON_0", name );
}

//__________________________________________________________________________________
TpotMonDraw::histogram_array_t TpotMonDraw::get_histograms( const std::string& name ) const
{
  histogram_array_t out{{nullptr}};
  for( size_t i=0; i<m_detnames_sphenix.size(); ++i)
  {
    const auto& detector_name=m_detnames_sphenix[i];
    const auto hname = name + "_" + detector_name;
    out[i] =  get_histogram(  hname );
    if( Verbosity() )
    { std::cout << "TpotMonDraw::get_histograms - " << hname << (out[i]?" found":" not found" ) << std::endl; }
  }

  return out;
}

//__________________________________________________________________________________
TH1* TpotMonDraw::get_ref_histogram( const std::string& name ) const
{ return m_ref_histograms_tfile ? static_cast<TH1*>( m_ref_histograms_tfile->Get( name.c_str() ) ):nullptr; }

//__________________________________________________________________________________
TpotMonDraw::histogram_array_t TpotMonDraw::get_ref_histograms( const std::string& name ) const
{
  histogram_array_t out{{nullptr}};
  for( size_t i=0; i<m_detnames_sphenix.size(); ++i)
  {
    const auto& detector_name=m_detnames_sphenix[i];
    const auto hname = name + "_" + detector_name;
    out[i] =  get_ref_histogram(  hname );
    if( Verbosity() )
    { std::cout << "TpotMonDraw::get_ref_histograms - " << hname << (out[i]?" found":" not found" ) << std::endl; }
  }

  return out;
}

//__________________________________________________________________________________
TpotMonDraw::histogram_array_t TpotMonDraw::get_ref_histograms_scaled( const std::string& name ) const
{
  histogram_array_t source( get_ref_histograms( name ) );
  histogram_array_t out{{nullptr}};

  const double scale = get_ref_scale_factor();
  for( size_t i=0; i<source.size(); ++i)
  { if( source[i] ) out[i]=normalize( source[i], scale ); }

  return out;
}

//__________________________________________________________________________________
double TpotMonDraw::get_ref_scale_factor() const
{
  if( !m_ref_histograms_tfile ) return 0;
  const auto m_counters = get_histogram( "m_counters");
  const auto m_counters_ref = get_ref_histogram( "m_counters");
  if( !( m_counters && m_counters_ref ) ) return 0;

  const double triggercnt = m_counters->GetBinContent( TpotMonDefs::kTriggerCounter );
  const double triggercnt_ref = m_counters_ref->GetBinContent( TpotMonDefs::kTriggerCounter );
  return triggercnt_ref > 0 ? triggercnt/triggercnt_ref : 0;
}

//__________________________________________________________________________________
TH1* TpotMonDraw::normalize( TH1* source, double scale ) const
{
  if( !source ) return nullptr;
  auto destination = static_cast<TH1*>( source->Clone() );
  destination->SetName( TString( source->GetName() )+"_scaled" );
  destination->Scale( scale );
  return destination;
}

//__________________________________________________________________________________
int TpotMonDraw::draw_array( const std::string& name, const TpotMonDraw::histogram_array_t& histograms, const TpotMonDraw::histogram_array_t& ref_histograms, unsigned int options )
{
  if( Verbosity() ) std::cout << "TpotMonDraw::draw_array - name: " << name << std::endl;

  auto cv = get_canvas(name);
  auto transparent = get_transparent_pad( cv, name);
  if( !cv ) return -1;

  bool drawn = false;
  CanvasEditor cv_edit(cv);

  // calculate matched maximum
  double maximum = 0;
  if(options&DrawOptions::MatchRange)
  {
    for( const auto& h:histograms )
    { if( h ) maximum = std::max( maximum, h->GetMaximum() ); }
  }

  // also scale by number of triggers if normalization is required
  if((options&DrawOptions::Normalize) && (m_triggercnt>0))
  { maximum/=m_triggercnt; }

  // draw
  for( size_t i = 0; i < histograms.size(); ++i )
  {
    if( histograms[i] )
    {
      cv->cd(i+1);
      TH1* copy = nullptr;
      if( options&DrawOptions::Colz )
      {
        copy = histograms[i]->DrawCopy( "col" );
      } else {
        histograms[i]->SetFillStyle(1001);
        histograms[i]->SetFillColor(kYellow );
        copy = histograms[i]->DrawCopy( "h" );
      }

      if( copy )
      {
        copy->SetTitle("");
        copy->SetStats(false);
        copy->GetXaxis()->SetTitleOffset(1.);
        copy->GetXaxis()->SetTitleSize( 0.08 );
        copy->GetXaxis()->SetLabelSize( 0.08 );

        copy->GetYaxis()->SetTitleOffset( i<12 ? 1.4:1.6);
        copy->GetYaxis()->SetTitleSize( i<12 ? 0.08:0.07 );
        copy->GetYaxis()->SetLabelSize( i<12 ? 0.08:0.07 );

        // normalize
        if((options&DrawOptions::Normalize) && (m_triggercnt>0))
        {
          copy->Scale( 1./m_triggercnt );
          copy->GetYaxis()->SetTitle("counts/trigger");
        }

        // equalize maximum
        if(options&DrawOptions::MatchRange)
        { copy->SetMaximum( 1.2*maximum ); }

      }

      // also draw reference
      if( ref_histograms[i] )
      {
        ref_histograms[i]->SetLineColor(2);

        const auto& ref_copy = ref_histograms[i]->DrawCopy("hist same" );
        ref_copy->SetStats(false);

        // normalize
        if((options&DrawOptions::Normalize) && (m_triggercnt>0))
        { ref_copy->Scale( 1./m_triggercnt ); }

      }

      // apply log scales
      if( options&DrawOptions::Logx )
      { gPad->SetLogx( true ); }

      if( options&DrawOptions::Logy && histograms[i]->GetEntries() > 0 )
      {
        gPad->SetLogy( true );
        if((options&DrawOptions::Normalize) && (m_triggercnt>0))
        {
          copy->SetMinimum(1./m_triggercnt);
        } else {
          copy->SetMinimum(1);
        }
      }

      if( options&DrawOptions::Logz )
      { gPad->SetLogz( true ); }

      // draw detector name
      draw_text( 0.7, 0.9, m_detnames_sphenix[i].c_str() );
      drawn = true;
    }
  }

  if( drawn )
  {
    draw_time(transparent);
    return 0;
  } else {
    DrawDeadServer(transparent);
    return -1;
  }

  // need to delete reference histograms to avoid leak
  for( auto h:ref_histograms ) { delete h; }
  return 0;
}

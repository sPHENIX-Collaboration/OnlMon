#include "InttMonDraw.h"

InttMonDraw::Options_t InttMonDraw::OPTIONS =
{
	//Chip-Channel
	{"chip_hitmap",		&InttMonDraw::GlobalChipChannelHitmap},

	//Ladder-Chip
	{"ladder_hitmap",	&InttMonDraw::GlobalLadderChipHitmap},
};

void InttMonDraw::GlobalChipChannelHitmap()
{
	DrawGlobalChipMap("chip_hitmap");
}

void InttMonDraw::GlobalLadderChipHitmap()
{
	DrawGlobalLadderMap("ladder_hitmap");
}

InttMonDraw::ExecOptions_t InttMonDraw::EXEC_OPTIONS =
{
	//Chip-Channel
	{"chip_hitmap",		{&InttMonDraw::PrepHitmapGlobalChipHist,	&InttMonDraw::PrepHitmapChannelHist}},

	//Ladder-Chip
	{"ladder_hitmap",	{&InttMonDraw::PrepHitmapGlobalLadderHist,	&InttMonDraw::PrepHitmapChipHist}},
};


InttMonDraw::InttMonDraw(const std::string &name) : OnlMonDraw(name)
{
	//keeping it fairly empty
	//dbvars = new OnlMonDB(ThisName);

	return;
}


InttMonDraw::~InttMonDraw()
{
	return;
}

//===		Inherited Functions		===//
int InttMonDraw::Init()
{
	//===	~Initialize Drawing Histograms	===//

/*
	//===	Register OnlMon Histograms	===//
	OnlMonClient* cl = OnlMonClient::instance();

//	cl->AddServerHost("localhost");
	cl->requestHistoBySubSystem("INTTMON", 1);

	cl->registerHisto("InttHitMap", "INTTMON");
	cl->registerHisto("InttHitMapRef", "INTTMON");
	//...
	//===	~Register OnlMon Histograms	===//
*/

	return 0;
}

int InttMonDraw::Draw(const std::string &what)
{
	bool b = false;
	bool found = false;

	std::string temp = "";
	for(std::size_t s = 0; s < what.length(); ++s)
	{
		temp += (char)std::tolower(what[s]);
	}

	for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		b = false;

		if(temp == "all")b = true;
		if(temp == itr->first)b = true;

		if(!b)continue;

		found = true;
		(*(itr->second))();
	}

	if(!found)
	{
		std::cout << "Option \"" << what << "\" not found" << std::endl;
		std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
		for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
		{
			std::cout << "\t" << itr->first << std::endl;
		}
	}

	return 0;
}

int InttMonDraw::MakePS(const std::string &what)
{
	OnlMonClient* cl = OnlMonClient::instance();
	if(Draw(what))return 1;

	TCanvas* canvas = nullptr;
	std::ostringstream filename;

	bool b = false;
	bool found = false;

	std::string temp = "";
	for(std::size_t s = 0; s < what.length(); ++s)
	{
		temp += (char)std::tolower(what[s]);
	}
	for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		b = false;

		if(temp == "all")b = true;
		if(temp == itr->first)b = true;

		if(!b)continue;

		found = true;

		filename << ThisName << "_" <<  itr->first << "_" << cl->RunNumber() << ".ps";
		canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_Canvas", itr->first.c_str()));
		if(canvas)canvas->Print(filename.str().c_str());
		if(canvas)cl->CanvasToPng(canvas, cl->htmlRegisterPage(*this, itr->first, itr->first, "png"));
		//needs attention
	}

	if(!found)
	{
		std::cout << "Option \"" << what << "\" not found" << std::endl;
		std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
		for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
		{
			std::cout << "\t" << itr->first << std::endl;
		}
	}

	return 0;
}

int InttMonDraw::MakeHtml(const std::string &what)
{
	OnlMonClient* cl = OnlMonClient::instance();
	if(Draw(what))return 1;

	TCanvas* canvas = nullptr;

	bool b = false;
	bool found = false;

	std::string temp = "";
	for(std::size_t s = 0; s < what.length(); ++s)
	{
		temp += (char)std::tolower(what[s]);
	}
	for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		b = false;

		if(temp == "all")b = true;
		if(temp == itr->first)b = true;

		if(!b)continue;

		found = true;

		canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_Canvas", itr->first.c_str()));
		if(canvas)cl->CanvasToPng(canvas, cl->htmlRegisterPage(*this, itr->first, itr->first, "png"));
	}

	if(!found)
	{
		std::cout << "Option \"" << what << "\" not found" << std::endl;
		std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
		for(Options_t::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
		{
			std::cout << "\t" << itr->first << std::endl;
		}
	}

	return 0;
}

void InttMonDraw::DrawPad(TPad* base, TPad* pad)
{
	pad->SetFillStyle(4000); //transparent
	pad->Range(0.0, 0.0, 1.0, 1.0);
	pad->SetTopMargin(T_MARGIN);
	pad->SetBottomMargin(B_MARGIN);
	pad->SetLeftMargin(L_MARGIN);
	pad->SetRightMargin(R_MARGIN);

	base->cd();
	pad->Draw();
	pad->cd();
}

//GlobalChip-Channel idiom
void InttMonDraw::DrawGlobalChipMap(std::string const& option)
{
	double x_lower = KEY_FRAC;
	double y_lower = 0.0;
	double x_upper = 1.0;
	double y_upper = 1.0;

	std::string name;
	struct INTT::Indexes_s indexes;

	TCanvas* cnvs;
	TPad* disp_pad;
	TPad* hist_pad;
	TPad* grid_pad;
	TPad* exec_pad;

	TH2D* client_hist;
	HistPrepFunc_t func = EXEC_OPTIONS.find(option)->second.first;

	name = Form("Intt_%s_GlobalChip_Canvas", option.c_str());
	cnvs = (TCanvas*)gROOT->FindObject(name.c_str());
	if(!cnvs)
	{
		cnvs = new TCanvas
		(
			name.c_str(),
			name.c_str(),
			0,
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
		//cnvs->...
		//...
	}

	name = Form("Intt_%s_GlobalChip_DispPad", option.c_str());
	disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!disp_pad)
	{
		disp_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			0.0,
			x_upper,
			DISP_FRAC
		);
		DrawPad(cnvs, disp_pad);
		TText* disp_text = new TText
		(
			0.5,
			0.5,
			Form("Layer: %2d Ladder: %2d (%s) Chip: %2d", INTT::LAYER_OFFSET, 0, "South", INTT::CHIP_OFFSET)
		);
		disp_text->SetName(Form("Intt_%s_GlobalChip_DispText", option.c_str()));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(DISP_TEXT_SIZE);
		disp_text->Draw();
	}

	for(indexes.lyr = 0; indexes.lyr < INTT::LAYER; ++indexes.lyr)
	{
		y_lower = (INTT::LAYER - indexes.lyr - 1.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC;
		y_upper = (INTT::LAYER - indexes.lyr - 0.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC;

		name = Form("Intt_%s_GlobalChip_HistPad_%d", option.c_str(), indexes.lyr);
		hist_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!hist_pad)
		{
			hist_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower,
				y_lower,
				x_upper,
				y_upper
			);
			DrawPad(cnvs, hist_pad);
		}

		name = Form("Intt_%s_GlobalChip_ClientHist_%d", option.c_str(), indexes.lyr);
		client_hist = (TH2D*)gROOT->FindObject(name.c_str());
		if(!client_hist)
		{
			client_hist = new TH2D
			(
				name.c_str(),
				name.c_str(),
				2 * INTT::LADDER[indexes.lyr],
				-0.5,
				2 * INTT::LADDER[indexes.lyr] - 0.5,
				INTT::CHIP,
				-0.5,
				INTT::CHIP - 0.5
			);
			client_hist->GetXaxis()->SetNdivisions(INTT::LADDER[indexes.lyr], true);
			client_hist->GetYaxis()->SetNdivisions(INTT::CHIP, true);

			client_hist->GetXaxis()->SetLabelSize(0.0);
			client_hist->GetYaxis()->SetLabelSize(0.0);

			client_hist->GetXaxis()->SetTickLength(0.0);
			client_hist->GetYaxis()->SetTickLength(0.0);

			client_hist->SetMinimum(-1.0);
		}

		(*func)(client_hist, indexes);
		hist_pad->cd();
		client_hist->DrawCopy("COLZ");
		delete client_hist;

		name = Form("Intt_%s_GlobalChip_GridPad_%d", option.c_str(), indexes.lyr);
		grid_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!grid_pad)
		{
			grid_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower,
				y_lower,
				x_upper,
				y_upper
			);
			DrawPad(cnvs, grid_pad);

			int i;
			double temp;

			for(i = 0; i < 2 * INTT::LADDER[indexes.lyr] + 1; ++i)
			{
				temp = L_MARGIN + (i / 2.0) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[indexes.lyr];

				TLine* line = new TLine(temp, B_MARGIN, temp, 1.0 - T_MARGIN);			
				line->SetLineStyle((i % 2) ? 3 : 1);
				line->SetLineWidth((i % 2) ? 1 : 2);
				line->Draw();
			}
	
			for(i = 0; i < INTT::CHIP + 1; ++i)
			{
				temp = B_MARGIN + i * (1.0 - T_MARGIN - B_MARGIN) / INTT::CHIP;
		
				TLine* line = new TLine(L_MARGIN, temp, 1.0 - R_MARGIN, temp);	
				line->SetLineStyle((i % (INTT::CHIP / 2)) ? 3 : 1);
				line->SetLineWidth((i % (INTT::CHIP / 2)) ? 1 : 2);
				line->Draw();
			}
		}

		name = Form("Intt_%s_GlobalChip_ExecPad_%d", option.c_str(), indexes.lyr);
		exec_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!exec_pad)
		{
			exec_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower + (x_upper - x_lower) * L_MARGIN,
				y_lower + (y_upper - y_lower) * B_MARGIN,
				x_upper - (x_upper - x_lower) * R_MARGIN,
				y_upper - (y_upper - y_lower) * T_MARGIN
			);
			DrawPad(cnvs, exec_pad);
			exec_pad->AddExec
			(
				Form("Intt_%s_GlobalChip_Exec_%d",option.c_str(), indexes.lyr),
				Form("InttMonDraw::InttGlobalChipExec(\"%s\", %d)", option.c_str(), indexes.lyr)
			);
		}
	}

	cnvs->Show();
	cnvs->SetEditable(0);
}

void InttMonDraw::DrawChannelMap(std::string const& option, struct INTT::Indexes_s indexes)
{
	double x_lower = 0.0;
	double y_lower = DISP_FRAC;
	double x_upper = 1.0;
	double y_upper = 1.0;

	std::string name;

	TCanvas* cnvs;
	TPad* disp_pad;
	TPad* hist_pad;
	TPad* exec_pad;
	
	TH2D* client_hist;
	HistPrepFunc_t func = EXEC_OPTIONS.find(option)->second.second;

	name = Form("Intt_%s_Channel_Canvas_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	cnvs = (TCanvas*)gROOT->FindObject(name.c_str());
	if(!cnvs)
	{
		cnvs = new TCanvas
		(
			name.c_str(),
			name.c_str(),
			0,
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
	}

	name = Form("Intt_%s_Channel_DispPad_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!disp_pad)
	{
		disp_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			0.0,
			x_upper,
			DISP_FRAC
		);
		DrawPad(cnvs, disp_pad);
		TText* disp_text = new TText
		(
			0.5,
			0.5,
			Form("Channel: %3d", 0)
		);
		disp_text->SetName(Form("Intt_%s_Channel_DispText_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(DISP_TEXT_SIZE);
		disp_text->Draw();
	}

	name = Form("Intt_%s_Channel_HistPad_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	hist_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!hist_pad)
	{
		hist_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			y_lower,
			x_upper,
			y_upper
		);
		DrawPad(cnvs, hist_pad);
	}

	name = Form("Intt_%s_Channel_ClientHist_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	client_hist = (TH2D*)gROOT->FindObject(name.c_str());
	if(!client_hist)
	{
		client_hist = new TH2D
		(
			name.c_str(),
			name.c_str(),
			INTT::CHANNEL,
			-0.5,
			INTT::CHANNEL - 0.5,
			INTT::ADC,
			-0.5,
			INTT::ADC - 0.5
		);

		client_hist->GetXaxis()->SetNdivisions(INTT::CHANNEL, true);
		client_hist->GetYaxis()->SetNdivisions(INTT::ADC, true);

		client_hist->GetXaxis()->SetLabelSize(0.0);
		client_hist->GetYaxis()->SetLabelSize(0.0);

		client_hist->GetXaxis()->SetTickLength(0.0);
		client_hist->GetYaxis()->SetTickLength(0.0);

		client_hist->SetMinimum(-1.0);
	}

	(*func)(client_hist, indexes);
	hist_pad->cd();
	client_hist->DrawCopy("COLZ");
	delete client_hist;

	name = Form("Intt_%s_Channel_ExecPad_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	exec_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!exec_pad)
	{
		exec_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower + (x_upper - x_lower) * L_MARGIN,
			y_lower + (y_upper - y_lower) * B_MARGIN,
			x_upper - (x_upper - x_lower) * R_MARGIN,
			y_upper - (y_upper - y_lower) * T_MARGIN
		);
		DrawPad(cnvs, exec_pad);
		exec_pad->AddExec
		(
			Form("Intt_%s_Channel_Exec_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp),
			Form("InttMonDraw::InttChannelExec(\"%s\", %d, %d, %d, %d)", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp)
		);
	}
}

void InttMonDraw::InttGlobalChipExec(std::string const& option, int layer)
{
	int bin_x = gPad->AbsPixeltoX(gPad->GetEventX()) * (2 * INTT::LADDER[layer]);
	int bin_y = gPad->AbsPixeltoY(gPad->GetEventY()) * (INTT::CHIP);
	std::string name;

	if(bin_x < 0)bin_x = 0;
	if(bin_y < 0)bin_y = 0;

	if(bin_x >= 2 * INTT::LADDER[layer])bin_x = 2 * INTT::LADDER[layer] - 1;
	if(bin_y >= INTT::CHIP)bin_y = INTT::CHIP - 1;

	INTT::Indexes_s indexes;
	indexes.lyr = layer;
	indexes.ldr = bin_x / 2;
	indexes.arm = bin_y / (INTT::CHIP / 2);
	indexes.chp = indexes.arm * (INTT::CHIP / 2 - 1) - (2 * indexes.arm - 1) * bin_y % (INTT::CHIP / 2) + ((indexes.arm + bin_x % 2) % 2) * (INTT::CHIP / 2);

	name = Form("Intt_%s_GlobalChip_DispText", option.c_str());
	TText* disp_text = (TText*)gROOT->FindObject(name.c_str());
	if(disp_text)disp_text->SetTitle(Form("Layer: %2d Ladder: %2d (%s) Chip: %2d", indexes.lyr + INTT::LAYER_OFFSET, indexes.ldr, indexes.arm ? "North" : "South", indexes.chp + INTT::CHIP_OFFSET));

	name = Form("Intt_%s_GlobalChip_DispPad", option.c_str());
	TPad* disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(disp_pad)disp_pad->Update();

	if(gPad->GetEvent() != 11)return; //left click

	DrawChannelMap(option, indexes);
}

void InttMonDraw::InttChannelExec(const std::string& option, int layer, int ladder, int northsouth, int chip)
{
	int bin_x = gPad->AbsPixeltoX(gPad->GetEventX()) * INTT::CHANNEL;
	int bin_y = gPad->AbsPixeltoY(gPad->GetEventY()) * INTT::ADC;
	std::string name;

	if(bin_x < 0)bin_x = 0;
	if(bin_y < 0)bin_y = 0;

	if(bin_x >= INTT::CHANNEL - 1)bin_x = INTT::CHANNEL - 1;
	if(bin_y >= INTT::ADC - 1)bin_y = INTT::ADC - 1;

	INTT::Indexes_s indexes;
	indexes.lyr = layer;
	indexes.ldr = ladder;
	indexes.arm = northsouth;
	indexes.chp = chip;
	indexes.chn = bin_x;
	indexes.adc = bin_y;

	name = Form("Intt_%s_Channel_DispText_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	TText* disp_text = (TText*)gROOT->FindObject(name.c_str());
	if(disp_text)disp_text->SetTitle(Form("Channel: %3d", indexes.chn));

	name = Form("Intt_%s_Channel_DispPad_Lyr%02d_Ldr%02d_Arm%02d_Chp%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm, indexes.chp);
	TPad* disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(disp_pad)disp_pad->Update();
}

//GlobalLadder-Chip idiom
void InttMonDraw::DrawGlobalLadderMap(std::string const& option)
{
	double x_lower = KEY_FRAC;
	double y_lower = 0.0;
	double x_upper = 1.0;
	double y_upper = 1.0;

	std::string name;
	struct INTT::Indexes_s indexes;

	TCanvas* cnvs;
	TPad* disp_pad;
	TPad* hist_pad;
	TPad* grid_pad;
	TPad* exec_pad;

	TH2D* client_hist;
	HistPrepFunc_t func = EXEC_OPTIONS.find(option)->second.first;

	name = Form("Intt_%s_GlobalLadder_Canvas", option.c_str());
	cnvs = (TCanvas*)gROOT->FindObject(name.c_str());
	if(!cnvs)
	{
		cnvs = new TCanvas
		(
			name.c_str(),
			name.c_str(),
			0,
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
		//cnvs->...
		//...
	}

	name = Form("Intt_%s_GlobalLadder_DispPad", option.c_str());
	disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!disp_pad)
	{
		disp_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			0.0,
			x_upper,
			DISP_FRAC
		);
		DrawPad(cnvs, disp_pad);
		TText* disp_text = new TText
		(
			0.5,
			0.5,
			Form("Layer: %2d Ladder: %2d (%s)", INTT::LAYER_OFFSET, 0, "South")
		);
		disp_text->SetName(Form("Intt_%s_GlobalLadder_DispText", option.c_str()));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(DISP_TEXT_SIZE);
		disp_text->Draw();
	}

	for(indexes.lyr = 0; indexes.lyr < INTT::LAYER; ++indexes.lyr)
	{
		y_lower = (INTT::LAYER - indexes.lyr - 1.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC;
		y_upper = (INTT::LAYER - indexes.lyr - 0.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC;

		name = Form("Intt_%s_GlobalLadder_HistPad_%d", option.c_str(), indexes.lyr);
		hist_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!hist_pad)
		{
			hist_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower,
				y_lower,
				x_upper,
				y_upper
			);
			DrawPad(cnvs, hist_pad);
		}

		name = Form("Intt_%s_GlobalLadder_ClientHist_%d", option.c_str(), indexes.lyr);
		client_hist = (TH2D*)gROOT->FindObject(name.c_str());
		if(!client_hist)
		{
			client_hist = new TH2D
			(
				name.c_str(),
				name.c_str(),
				INTT::LADDER[indexes.lyr],
				-0.5,
				INTT::LADDER[indexes.lyr] - 0.5,
				INTT::ARM,
				-0.5,
				INTT::ARM - 0.5
			);
			client_hist->GetXaxis()->SetNdivisions(INTT::LADDER[indexes.lyr], true);
			client_hist->GetYaxis()->SetNdivisions(INTT::ARM, true);

			client_hist->GetXaxis()->SetLabelSize(0.0);
			client_hist->GetYaxis()->SetLabelSize(0.0);

			client_hist->GetXaxis()->SetTickLength(0.0);
			client_hist->GetYaxis()->SetTickLength(0.0);

			client_hist->SetMinimum(-1.0);
		}

		(*func)(client_hist, indexes);
		hist_pad->cd();
		client_hist->DrawCopy("COLZ");
		delete client_hist;

		name = Form("Intt_%s_GlobalLadder_GridPad_%d", option.c_str(), indexes.lyr);
		grid_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!grid_pad)
		{
			grid_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower,
				y_lower,
				x_upper,
				y_upper
			);
			DrawPad(cnvs, grid_pad);

			int i;
			double temp;

			for(i = 0; i < INTT::LADDER[indexes.lyr] + 1; ++i)
			{
				temp = L_MARGIN + i * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[indexes.lyr];
				TLine* line = new TLine(temp, B_MARGIN, temp, 1.0 - T_MARGIN);
				line->SetLineStyle(1);
				line->SetLineWidth(2);
				line->Draw();
			}

			for(i = 0; i < 2; ++i)
			{
				temp = B_MARGIN + i * (1.0 - T_MARGIN - B_MARGIN) / 2;
				TLine* line = new TLine(L_MARGIN, temp, 1.0 - R_MARGIN, temp);
				line->SetLineStyle(1);
				line->SetLineWidth(2);
				line->Draw();
			}
		}

		name = Form("Intt_%s_GlobalLadder_ExecPad_%d", option.c_str(), indexes.lyr);
		exec_pad = (TPad*)gROOT->FindObject(name.c_str());
		if(!exec_pad)
		{
			exec_pad = new TPad
			(
				name.c_str(),
				name.c_str(),
				x_lower + (x_upper - x_lower) * L_MARGIN,
				y_lower + (y_upper - y_lower) * B_MARGIN,
				x_upper - (x_upper - x_lower) * R_MARGIN,
				y_upper - (y_upper - y_lower) * T_MARGIN
			);
			DrawPad(cnvs, exec_pad);
			exec_pad->AddExec
			(
				Form("Intt_%s_GlobalLadder_Exec_%d",option.c_str(), indexes.lyr),
				Form("InttMonDraw::InttGlobalLadderExec(\"%s\", %d)", option.c_str(), indexes.lyr)
			);
		}
	}

	cnvs->Show();
	cnvs->SetEditable(0);
}

void InttMonDraw::DrawChipMap(std::string const& option, struct INTT::Indexes_s indexes)
{
	double x_lower = 0.0;
	double y_lower = DISP_FRAC;
	double x_upper = 1.0;
	double y_upper = 1.0;

	std::string name;

	TCanvas* cnvs;
	TPad* disp_pad;
	TPad* hist_pad;
	TPad* grid_pad;
	TPad* exec_pad;
	
	TH2D* client_hist;
	HistPrepFunc_t func = EXEC_OPTIONS.find(option)->second.second;

	name = Form("Intt_%s_Chip_Canvas_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	cnvs = (TCanvas*)gROOT->FindObject(name.c_str());
	if(!cnvs)
	{
		cnvs = new TCanvas
		(
			name.c_str(),
			name.c_str(),
			0,
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
	}

	name = Form("Intt_%s_Chip_DispPad_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!disp_pad)
	{
		disp_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			0.0,
			x_upper,
			DISP_FRAC
		);
		DrawPad(cnvs, disp_pad);
		TText* disp_text = new TText
		(
			0.5,
			0.5,
			Form("(%s) Chip: %2d Channel: %3d", "South", 0, INTT::CHIP_OFFSET)
		);
		disp_text->SetName(Form("Intt_%s_Chip_DispText_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(DISP_TEXT_SIZE);
		disp_text->Draw();
	}

	name = Form("Intt_%s_Chip_HistPad_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	hist_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!hist_pad)
	{
		hist_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			y_lower,
			x_upper,
			y_upper
		);
		DrawPad(cnvs, hist_pad);
	}

	name = Form("Intt_%s_Chip_ClientHist_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	client_hist = (TH2D*)gROOT->FindObject(name.c_str());
	if(!client_hist)
	{
		client_hist = new TH2D
		(
			name.c_str(),
			name.c_str(),
			2 * INTT::CHANNEL,
			-0.5,
			2 * INTT::CHANNEL - 0.5,
			INTT::CHIP / 2,
			-0.5,
			INTT::CHIP / 2 - 0.5
		);

		client_hist->GetXaxis()->SetNdivisions(2 * INTT::CHANNEL, true);
		client_hist->GetYaxis()->SetNdivisions(INTT::CHIP / 2, true);

		client_hist->GetXaxis()->SetLabelSize(0.0);
		client_hist->GetYaxis()->SetLabelSize(0.0);

		client_hist->GetXaxis()->SetTickLength(0.0);
		client_hist->GetYaxis()->SetTickLength(0.0);

		client_hist->SetMinimum(-1.0);
	}

	(*func)(client_hist, indexes);
	hist_pad->cd();
	client_hist->DrawCopy("COLZ");
	delete client_hist;

	name = Form("Intt_%s_Chip_GridPad_Lyr%d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	grid_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!grid_pad)
	{
		grid_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower,
			y_lower,
			x_upper,
			y_upper
		);
		DrawPad(cnvs, grid_pad);

		int i;
		double temp;

		for(i = 0; i < 2 * INTT::CHANNEL + 1; ++i)
		{
			temp = L_MARGIN + (i / 2.0) * (1.0 - R_MARGIN - L_MARGIN) / INTT::CHANNEL;
		
			TLine* line = new TLine(temp, B_MARGIN, temp, 1.0 - T_MARGIN);
			line->SetLineStyle(i == INTT::CHANNEL ? 1 : 3);
			line->SetLineWidth(i == INTT::CHANNEL ? 2 : 1);
			line->Draw();
		}

		for(i = 0; i < INTT::CHIP / 2 + 1; ++i)
		{
			temp = B_MARGIN + 2 * i * (1.0 - T_MARGIN - B_MARGIN) / INTT::CHIP;
	
			TLine* line = new TLine(L_MARGIN, temp, 1.0 - R_MARGIN, temp);	
			line->SetLineStyle(i == INTT::CHIP / 2 ? 1 : 3);
			line->SetLineWidth(i == INTT::CHIP / 2 ? 2 : 1);
			line->Draw();
		}
	}

	name = Form("Intt_%s_Chip_ExecPad_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	exec_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(!exec_pad)
	{
		exec_pad = new TPad
		(
			name.c_str(),
			name.c_str(),
			x_lower + (x_upper - x_lower) * L_MARGIN,
			y_lower + (y_upper - y_lower) * B_MARGIN,
			x_upper - (x_upper - x_lower) * R_MARGIN,
			y_upper - (y_upper - y_lower) * T_MARGIN
		);
		DrawPad(cnvs, exec_pad);
		exec_pad->AddExec
		(
			Form("Intt_%s_Chip_Exec_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm),
			Form("InttMonDraw::InttChipExec(\"%s\", %d, %d, %d)", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm)
		);
	}
}

void InttMonDraw::InttGlobalLadderExec(std::string const& option, int layer)
{
	int bin_x = gPad->AbsPixeltoX(gPad->GetEventX()) * (2 * INTT::LADDER[layer]);
	int bin_y = gPad->AbsPixeltoY(gPad->GetEventY()) * INTT::ARM;
	std::string name;

	if(bin_x < 0)bin_x = 0;
	if(bin_y < 0)bin_y = 0;

	if(bin_x >= 2 * INTT::LADDER[layer])bin_x = 2 * INTT::LADDER[layer] - 1;
	if(bin_y >= INTT::ARM)bin_y = INTT::ARM - 1;

	INTT::Indexes_s indexes;
	indexes.lyr = layer;
	indexes.ldr = bin_x / 2;
	indexes.arm = bin_y;

	name = Form("Intt_%s_GlobalLadder_DispText", option.c_str());
	TText* disp_text = (TText*)gROOT->FindObject(name.c_str());
	if(disp_text)disp_text->SetTitle(Form("Layer: %2d Ladder: %2d (%s)", indexes.lyr + INTT::LAYER_OFFSET, indexes.ldr, indexes.arm ? "North" : "South"));

	name = Form("Intt_%s_GlobalLadder_DispPad", option.c_str());
	TPad* disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(disp_pad)disp_pad->Update();

	if(gPad->GetEvent() != 11)return; //left click

	DrawChipMap(option, indexes);
}

void InttMonDraw::InttChipExec(const std::string& option, int layer, int ladder, int arm)
{
	int bin_x = gPad->AbsPixeltoX(gPad->GetEventX()) * 2 * INTT::CHANNEL;
	int bin_y = gPad->AbsPixeltoY(gPad->GetEventY()) * INTT::CHIP / 2;
	std::string name;

	if(bin_x < 0)bin_x = 0;
	if(bin_y < 0)bin_y = 0;

	if(bin_x >= 2 * INTT::CHANNEL - 1)bin_x = 2 * INTT::CHANNEL - 1;
	if(bin_y >= INTT::CHIP / 2 - 1)bin_y = INTT::CHIP / 2 - 1;

	INTT::Indexes_s indexes;
	indexes.lyr = layer;
	indexes.ldr = ladder;
	indexes.arm = arm;
	indexes.chp = indexes.arm * (INTT::CHIP / 2 - 1) - (2 * indexes.arm - 1) * bin_y + ((indexes.arm + bin_x / INTT::CHANNEL) % 2) * (INTT::CHIP / 2);
	indexes.chn = bin_x % INTT::CHANNEL;

	name = Form("Intt_%s_Chip_DispText_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	TText* disp_text = (TText*)gROOT->FindObject(name.c_str());
	if(disp_text)disp_text->SetTitle(Form("Chip: %2d Channel: %3d", indexes.chp + INTT::CHIP_OFFSET, indexes.chn));

	name = Form("Intt_%s_Chip_DispPad_Lyr%02d_Ldr%02d_Arm%02d", option.c_str(), indexes.lyr, indexes.ldr, indexes.arm);
	TPad* disp_pad = (TPad*)gROOT->FindObject(name.c_str());
	if(disp_pad)disp_pad->Update();
}

//GlobalChip-Channel methods
void InttMonDraw::PrepHitmapGlobalChipHist(TH2D* client_hist, struct INTT::Indexes_s& indexes)
{
	client_hist->Reset();

	int bin;
	double temp;

	int bin_x;
	int bin_y;

	int felix = 0;
	int felix_channel = 0;
	struct INTT_Felix::Ladder_s ldr_struct;

	indexes.ldr = 0;
	indexes.arm = 0;
	indexes.chp = 0;
	indexes.chn = 0;
	indexes.adc = 0;

	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

	while(true)
	{
		if(server_hist)break;
		++felix;

		if(felix >= INTT::FELIX)break;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
	}
	while(felix < INTT::FELIX)
	{
		INTT_Felix::FelixMap(felix, felix_channel, ldr_struct);
		if(2 * ldr_struct.barrel + ldr_struct.layer == indexes.lyr)
		{
			indexes.ldr = ldr_struct.ladder;
			indexes.arm = felix / 4;

			bin_x = 2 * indexes.ldr + (indexes.arm + indexes.chp / (INTT::CHIP / 2)) % 2 + 1;
			bin_y = (1 - 2 * indexes.arm) * (indexes.chp % (INTT::CHIP / 2)) + indexes.arm * (INTT::CHIP - 1) + 1;

			//client_hist->SetBinContent(client_hist->GetBin(bin_x, bin_y), indexes.chp); //for debugging
			//...

			INTT::GetFelixBinFromIndexes(bin, felix_channel, indexes);
			temp = server_hist->GetBinContent(bin);
			bin = client_hist->GetBin(bin_x, bin_y);
			temp += client_hist->GetBinContent(bin);
			client_hist->SetBinContent(bin, temp);
		}

		++indexes.adc;
		if(indexes.adc < INTT::ADC)continue;
		indexes.adc = 0;

		++indexes.chn;
		if(indexes.chn < INTT::CHANNEL)continue;
		indexes.chn = 0;

		++indexes.chp;
		if(indexes.chp < INTT::CHIP)continue;
		indexes.chp = 0;

		++felix_channel;
		if(felix_channel < INTT::FELIX_CHANNEL)continue;
		felix_channel = 0;

		++felix;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

		while(true)
		{
			if(server_hist)break;
			++felix;

			if(felix >= INTT::FELIX)break;
			server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
		}
	}
}

void InttMonDraw::PrepHitmapChannelHist(TH2D* client_hist, struct INTT::Indexes_s& indexes)
{
	client_hist->Reset();

	int bin;
	double temp;

	int bin_x;
	int bin_y;

	int felix = 0;
	int felix_channel = 0;
	struct INTT_Felix::Ladder_s ldr_struct;

	indexes.chn = 0;
	indexes.adc = 0;

	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

	while(true)
	{
		if(server_hist)break;
		++felix;

		if(felix >= INTT::FELIX)break;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
	}
	while(felix < INTT::FELIX)
	{
		INTT_Felix::FelixMap(felix, felix_channel, ldr_struct);
		if(2 * ldr_struct.barrel + ldr_struct.layer == indexes.lyr && ldr_struct.ladder == indexes.ldr && felix / 4 == indexes.arm)
		{
			bin_x = indexes.chn + 1;
			bin_y = indexes.adc + 1;

			//client_hist->SetBinContent(client_hist->GetBin(bin_x, bin_y), indexes.chn); //for debugging
			//client_hist->SetBinContent(client_hist->GetBin(bin_x, bin_y), indexes.adc); //for debugging
			//...

			INTT::GetFelixBinFromIndexes(bin, felix_channel, indexes);
			temp = server_hist->GetBinContent(bin);
			bin = client_hist->GetBin(bin_x, bin_y);
			temp += client_hist->GetBinContent(bin);
			client_hist->SetBinContent(bin, temp);
		}

		++indexes.adc;
		if(indexes.adc < INTT::ADC)continue;
		indexes.adc = 0;

		++indexes.chn;
		if(indexes.chn < INTT::CHANNEL)continue;
		indexes.chn = 0;

		++felix_channel;
		if(felix_channel < INTT::FELIX_CHANNEL)continue;
		felix_channel = 0;

		++felix;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

		while(true)
		{
			if(server_hist)break;
			++felix;

			if(felix >= INTT::FELIX)break;
			server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
		}
	}
}

//GlobalLadder-Chip methods
void InttMonDraw::PrepHitmapGlobalLadderHist(TH2D* client_hist, struct INTT::Indexes_s& indexes)
{
	client_hist->Reset();

	int bin;
	double temp;

	int bin_x;
	int bin_y;

	int felix = 0;
	int felix_channel = 0;
	struct INTT_Felix::Ladder_s ldr_struct;

	indexes.ldr = 0;
	indexes.arm = 0;
	indexes.chp = 0;
	indexes.chn = 0;
	indexes.adc = 0;

	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

	while(true)
	{
		if(server_hist)break;
		++felix;

		if(felix >= INTT::FELIX)break;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
	}
	while(felix < INTT::FELIX)
	{
		INTT_Felix::FelixMap(felix, felix_channel, ldr_struct);
		if(2 * ldr_struct.barrel + ldr_struct.layer == indexes.lyr)
		{
			indexes.ldr = ldr_struct.ladder;
			indexes.arm = felix / 4;

			bin_x = indexes.ldr + 1;
			bin_y = indexes.arm + 1;

			//client_hist->SetBinContent(client_hist->GetBin(bin_x, bin_y), indexes.ldr); //for debugging
			//...

			INTT::GetFelixBinFromIndexes(bin, felix_channel, indexes);
			temp = server_hist->GetBinContent(bin);
			bin = client_hist->GetBin(bin_x, bin_y);
			temp += client_hist->GetBinContent(bin);
			client_hist->SetBinContent(bin, temp);
		}

		++indexes.adc;
		if(indexes.adc < INTT::ADC)continue;
		indexes.adc = 0;

		++indexes.chn;
		if(indexes.chn < INTT::CHANNEL)continue;
		indexes.chn = 0;

		++indexes.chp;
		if(indexes.chp < INTT::CHIP)continue;
		indexes.chp = 0;

		++felix_channel;
		if(felix_channel < INTT::FELIX_CHANNEL)continue;
		felix_channel = 0;

		++felix;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

		while(true)
		{
			if(server_hist)break;
			++felix;

			if(felix >= INTT::FELIX)break;
			server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
		}
	}
}

void InttMonDraw::PrepHitmapChipHist(TH2D* client_hist, struct INTT::Indexes_s& indexes)
{
	client_hist->Reset();

	int bin;
	double temp;

	int bin_x;
	int bin_y;

	int felix = 0;
	int felix_channel = 0;
	struct INTT_Felix::Ladder_s ldr_struct;

	indexes.chp = 0;
	indexes.chn = 0;
	indexes.adc = 0;

	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

	while(true)
	{
		if(server_hist)break;
		++felix;

		if(felix >= INTT::FELIX)break;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
	}
	while(felix < INTT::FELIX)
	{
		INTT_Felix::FelixMap(felix, felix_channel, ldr_struct);
		if(2 * ldr_struct.barrel + ldr_struct.layer == indexes.lyr && ldr_struct.ladder == indexes.ldr && felix / 4 == indexes.arm)
		{
			bin_x = indexes.chn + ((indexes.arm + indexes.chp / (INTT::CHIP / 2)) % 2) * INTT::CHANNEL + 1;
			bin_y = (1 - 2 * indexes.arm) * (indexes.chp % (INTT::CHIP / 2)) + indexes.arm * (INTT::CHIP - 1) + 1;

			//client_hist->SetBinContent(client_hist->GetBin(bin_x, bin_y), indexes.chp); //for debugging
			//...

			INTT::GetFelixBinFromIndexes(bin, felix_channel, indexes);
			temp = server_hist->GetBinContent(bin);
			bin = client_hist->GetBin(bin_x, bin_y);
			temp += client_hist->GetBinContent(bin);
			client_hist->SetBinContent(bin, temp);
		}

		++indexes.adc;
		if(indexes.adc < INTT::ADC)continue;
		indexes.adc = 0;

		++indexes.chn;
		if(indexes.chn < INTT::CHANNEL)continue;
		indexes.chn = 0;

		++indexes.chp;
		if(indexes.chp < INTT::CHIP)continue;
		indexes.chp = 0;

		++felix_channel;
		if(felix_channel < INTT::FELIX_CHANNEL)continue;
		felix_channel = 0;

		++felix;
		server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");

		while(true)
		{
			if(server_hist)break;
			++felix;

			if(felix >= INTT::FELIX)break;
			server_hist = (TH1D*)cl->getHisto(Form("INTTMON_%d", felix), "InttMap");
		}
	}
}

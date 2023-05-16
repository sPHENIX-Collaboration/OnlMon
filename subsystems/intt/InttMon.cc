#include "InttMon.h"

//===	Public Methods		===//
InttMon::InttMon(const std::string &name) : OnlMon(name)
{
	//leaving fairly empty
	return;
}

InttMon::~InttMon()
{
	delete dbvars;
}

int InttMon::Init()
{
	OnlMonServer *omc = OnlMonServer::instance();

	//dbvars
	dbvars = new OnlMonDB(ThisName);
	DBVarInit();

	//histograms
	//DeadMap = ...
	NumEvents = new TH1D(Form("InttNumEvents"), Form("InttNumEvents"), 1, 0, 1);
	HitMap = new TH1D(Form("InttHitMap"), Form("InttHitMap"), INTT::CHANNELS, 0, INTT::CHANNELS);
	HitMapRef = new TH1D(Form("InttHitMapRef"), Form("InttHitMapRef"), INTT::CHANNELS, 0, INTT::CHANNELS);
	ADCMap = new TH1D(Form("InttADCMap"), Form("InttADCMap"), INTT::ADCS, 0, INTT::ADCS);
	ADCMapRef = new TH1D(Form("InttADCMapRef"), Form("InttADCMapRef"), INTT::ADCS, 0, INTT::ADCS);
	//...

	omc->registerHisto(this, NumEvents);
	omc->registerHisto(this, HitMap);
	omc->registerHisto(this, HitMapRef);
	omc->registerHisto(this, ADCMap);
	omc->registerHisto(this, ADCMapRef);
	//...

	//Read in calibrartion data from InttMonData.dat
	const char *inttcalib = getenv("INTTCALIB");
	if (!inttcalib)
	{
	  std::cout << "INTTCALIB environment variable not set" << std::endl;
	  exit(1);
	}
	std::string fullfile = std::string(inttcalib) + "/" + "InttMonData.dat";
	std::ifstream calib(fullfile);
	//probably need to do stuff here (maybe write to expectation maps)
	//or reimplment in BeginRun()
	calib.close();

	// for testing/debugging without unpacker, remove later
	InitExpectationHists();
	rng = new TRandom(1234);
	//~for testing/debugging without unpacker, remove later

	Reset();

	return 0;
}

int InttMon::BeginRun(const int /* run_num */)
{
	//per-run calibrations; don't think we need to do anything here yet

	return 0;
}

int InttMon::process_event(Event* evt)
{
	int N;

	int bin;

	int felix;
	int felix_channel;

	int layer;
	int ladder;
	int northsouth;
	int chip;
	int channel;
	int adc;

	struct INTT_Felix::Ladder_s lddr_s;

	int id;
	int n;

	for(id = 3001; id < 3009; ++id)
	{
		Packet* p = evt->getPacket(id);
		if(!p)continue;

		N = p->iValue(0, "NR_HITS");

		if(N)std::cout << N << std::endl;

		for(n = 0; n < N; ++n)
		{
			felix = id - 3001;
			felix_channel = p->iValue(n, "FEE");

			INTT_Felix::FelixMap(felix, felix_channel, lddr_s);

			layer = lddr_s.barrel * 2 + lddr_s.layer;
			ladder = lddr_s.ladder;

			northsouth = ((id - 3001) / 4) % 2;

			chip = p->iValue(n, "CHP_ID");
			chip = (chip - 1) % 26 + INTT::CHIP_OFFSET;

			channel = p->iValue(n, "CHANNEL_ID");

			adc = p->iValue(n, "ADC");
			if(INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel))continue;
			HitMap->AddBinContent(bin);

			if(INTT::ADCN::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel, adc))continue;
			ADCMap->AddBinContent(bin);
		}
	}

	////dummy method since unpacker is not done yet
	//double temp;


	////...

	//int hits = rng->Poisson(HITS_PER_EVENT);
	//for(int hit = 0; hit < hits; hit++)
	//{
	//	//randomly choose a chip/channel for the hit to occur
	//	//including guards in case upper bound of TRandom::Uniform(Double_t) is inclusive
	//	layer = rng->Uniform(INTT::LAYER);
	//	if(layer == INTT::LAYER)layer -= 1;

	//	ladder = rng->Uniform(INTT::LADDER[layer]);
	//	if(ladder == INTT::LADDER[layer])ladder -= 1;

	//	northsouth = rng->Uniform(INTT::NORTHSOUTH);
	//	if(northsouth == INTT::NORTHSOUTH)northsouth -= 1;

	//	chip = rng->Uniform(INTT::CHIP);
	//	if(chip == INTT::CHIP)chip -= 1;

	//	channel = rng->Uniform(INTT::CHANNEL);
	//	if(channel == INTT::CHANNEL)channel -= 1;

	//	adc = rng->Uniform(INTT::ADC);
	//	if(adc == INTT::ADC)adc -= 1;

	//	//Set temp to be the unscaled z position of the chip, to introduce eta dependence for the dummy method
	//	//currently not used; hitrates are flat
	//	temp = (2.0 * northsouth - 1.0) * ( (INTT::CHIP / 2) - chip % (INTT::CHIP / 2) - 0.5 );

	//	//get the bin these indexes correspond to
	//	//INTT::FindBin returns 1 if there is an error; extra guard statement for typos
	//	if(INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel))continue;
	//	HitMap->AddBinContent(bin);
	//	if(INTT::ADCN::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel, adc))continue;
	//	ADCMap->AddBinContent(bin);

	//	std::string northsouth_str = "North";
	//	if(northsouth)northsouth_str = "South";

	//	printf("Layer:%2d\tLadder:%3d (%s)\tChip:%3d\tChannel:%4d\n", layer + INTT::LAYER_OFFSET, ladder, northsouth_str.c_str(), chip + INTT::CHIP_OFFSET, channel);
	//	//...

	//	bin = temp;//dummy line so it will compile; temp is "used"
	//}

	NumEvents->AddBinContent(1);

	DBVarUpdate();

	return 0;
}

int InttMon::Reset()
{
	//reset our DBVars
	evtcnt = 0;
	
	//clear our histogram entries
	NumEvents->Reset();
	HitMap->Reset();
	ADCMap->Reset();

	return 0;
}
//===	~Public Methods		===//

//===	Private Methods		===//
int InttMon::DBVarInit()
{
	std::string var_name;

	var_name = "intt_evtcnt";
	dbvars->registerVar(var_name);

	dbvars->DBInit();

	return 0;
}

int InttMon::DBVarUpdate()
{
	dbvars->SetVar("intt_evtcnt", (float)evtcnt, 0.1 * evtcnt, (float)evtcnt);

	return 0;
}
//===	~Private Methods		===//

// for testing/debugging without unpacker, remove later
int InttMon::MiscDebug()
{
	//write methods to debug/test here
	int bin;
	int b;

	int layer;
	int ladder;
	int northsouth;
	int chip;
	int channel;
	//int adc;

	int ly;
	int ld;
	int ns;
	int cp;
	int cn;
	//int a;

	bool round_trip_1 = true;
	bool round_trip_2 = true;

	//global checks
	for(bin = 1; bin <= INTT::CHANNELS; bin++)
	{
		b = -1;
		INTT::HitMap::FindGlobalIndices(bin, layer, ladder, northsouth, chip, channel);
		INTT::HitMap::FindGlobalBin(b, layer, ladder, northsouth, chip, channel);

		if(b != bin)round_trip_1 = false;
		
		if(!round_trip_1)
		{
			std::cout << "layer: " << layer << "\tladder: " << ladder << "\tnorthsouth: " << northsouth << "\tchip: " << chip << "\tchannel: " << channel << std::endl;
			std::cout << "\tbin: " << bin << std::endl;
			std::cout << "\t\tto" << std::endl;
			std::cout << "\tbin: " << b << std::endl;
			std::cout << std::endl;
			goto label1;
		}
	}


	//local checks
//	for(layer = 0; layer < INTT::LAYER; layer++)
//	{
//		for(bin = 1; bin <= INTT::LADDER[layer] * INTT::NORTHSOUTH * INTT::CHIP; bin++)
//		{
//			INTT::HitMap::FindLayerIndices(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip);
//			INTT::HitMap::FindLayerBin(b, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip);
//
//			if(b != bin)round_trip_1 = false;
//
//			if(!round_trip_1)
//			{
//				std::cout << bin << "->" << b << std::endl;
//				std::cout << "\tlayer: " << layer << "\tladder: " << ladder << "\tnorthsouth: " << northsouth << "\tchip: " << chip << std::endl;
//				//goto label1;
//			}
//		}
//	}

	label1:
	std::cout << "bin to bin round trip: ";
	if(round_trip_1)std::cout << "\tworks" << std::endl;
	else std::cout << "\tdoesn't work :^(" << std::endl;

	for(layer = 0; layer < INTT::LAYER; layer++)
	{
		for(ladder = 0; ladder < INTT::LADDER[layer]; ladder++)
		{
			for(northsouth = 0; northsouth < INTT::NORTHSOUTH; northsouth++)
			{
				for(chip = 0; chip < INTT::CHIP; chip++)
				{
					for(channel = 0; channel < INTT::CHIP; channel++)
					{
						//for(adc = 0; adc < INTT::ADC; adc++)
						//{
							ly = -1;
							ld = -1;
							ns = -1;
							cp = -1;
							cn = -1;
							INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel);
							INTT::HitMap::FindGlobalIndices(bin, ly, ld, ns, cp, cn);

							if(!(1 <= bin and bin < INTT::CHANNELS))round_trip_2 = false;

							if(ly != layer + INTT::LAYER_OFFSET)round_trip_2 = false;
							if(ld != ladder)round_trip_2 = false;
							if(ns != northsouth)round_trip_2 = false;
							if(cp != chip + INTT::CHIP_OFFSET)round_trip_2 = false;
							if(cn != channel)round_trip_2 = false;

							if(!round_trip_2)
							{
								std::cout << "bin: " << bin << std::endl;
								std::cout << "\tlayer: " << layer << "\tladder: " << ladder << "\tnorthsouth: " << northsouth << "\tchip: " << chip << "\tchannel: " << channel << std::endl;
								std::cout << "\t\tto" << std::endl;
								std::cout << "\tlayer: " << ly << "\tladder: " << ld << "\tnorthsouth: " << ns << "\tchip: " << cp << "\tchannel: " << cn << std::endl;
								std::cout << std::endl;
								goto label2;
							}
						//}
					}
				}
			}
		}
	}

//	for(layer = 0; layer < INTT::LAYER; layer++)
//	{
//		for(ladder = 0; ladder < INTT::LADDER[layer]; ladder++)
//		{
//			for(northsouth = 0; northsouth < INTT::NORTHSOUTH; northsouth++)
//			{
//				for(chip = 0; chip < INTT::CHIP; chip++)
//				{
//					//for(adc = 0; adc < INTT::ADC; adc++)
//					//{
//						INTT::HitMap::FindLayerBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
//						INTT::HitMap::FindLayerIndices(bin, layer + INTT::LAYER_OFFSET, ld, ns, cp);
//
//						if(ld != ladder)round_trip_2 = false;
//						if(ns != northsouth)round_trip_2 = false;
//						if(cp != chip + INTT::CHIP_OFFSET)round_trip_2 = false;
//
//						if(!round_trip_2)
//						{
//							std::cout << "layer: " << layer + INTT::LAYER_OFFSET << "bin: " << bin << std::endl;
//							std::cout << "\tladder: " << ladder << "\tnorthsouth: " << northsouth << "\tchip: " << chip + INTT::CHIP_OFFSET << std::endl;
//							std::cout << "\t\tto" << std::endl;
//							std::cout << "\tladder: " << ld << "\tnorthsouth: " << ns << "\tchip: " << cp << std::endl;
//							goto label2;
//						}
//					//}
//				}
//			}
//		}
//	}


	label2:
	std::cout << "indices to indices round trip: ";
	if(round_trip_2)std::cout << "\tworks" << std::endl;
	else std::cout << "\tdoesn't work :^(" << std::endl;

	int out_counts = 0;
	int max_counts = 10;

	Init();
	std::cout << HitMapRef->GetNcells() << std::endl;
	std::cout << INTT::CHANNELS << std::endl;
	for(bin = 1; bin < HitMapRef->GetNcells(); bin++)
	{
		if(HitMapRef->GetBinContent(bin) == 0.0)
		{
			if(out_counts < max_counts)
			{
				if(INTT::HitMap::FindGlobalIndices(bin, ly, ld, ns, cp, cn))std::cout << "bad bin to indices" << std::endl;
				if(INTT::HitMap::FindGlobalBin(b, ly, ld, ns, cp, cn))std::cout << "bad indices to bin" << std::endl;
				std::cout << bin << std::endl;
				std::cout << b << std::endl;
				std::cout << "\tlayer: " << ly << std::endl;
				std::cout << "\tladder: " << ld << std::endl;
				std::cout << "\tnorthsouth: " << ns << std::endl;
				std::cout << "\tchip: " << cp << std::endl;
				std::cout << "\tchannel: " << cn << std::endl;

				out_counts++;
			}
			else
			{
				break;
			}
		}
	}

	return 0;
}
//~for testing/debugging without unpacker, remove later

int InttMon::InitExpectationHists()
{
	HitMapRef->SetMinimum(-1);
	ADCMapRef->SetMinimum(-1);

	double temp;

	int bin;

	int layer;
	int ladder;
	int northsouth;
	int chip;
	int channel;
	int adc;

	for(layer = 0; layer < INTT::LAYER; layer++)
	{
		for(ladder = 0; ladder < INTT::LADDER[layer]; ladder++)
		{
			for(northsouth = 0; northsouth < INTT::NORTHSOUTH; northsouth++)
			{
				for(chip = 0; chip < INTT::CHIP; chip++)
				{
					for(channel = 0; channel < INTT::CHANNEL; channel++)
					{
						//set temp to be z coordinate of chip (in units of number of chips from z = 0)
						temp = (2.0 * northsouth - 1.0) * ( (INTT::CHIP / 2) - chip % (INTT::CHIP / 2) - 0.5 );

						//Pertenant to option "hitmap"
						INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel);
						HitMapRef->SetBinContent(bin, HITS_PER_EVENT / INTT::CHANNELS);

						//Pertenant to option "adc"
						for(adc = 0; adc < INTT::ADC; adc++)
						{
							INTT::ADCN::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel, adc);
							ADCMapRef->SetBinContent(bin, HITS_PER_EVENT / INTT::ADCS);
						}

						//...
						bin = temp;//dummy line so it will compile; temp is "used"
					}
				}
			}
		}
	}


	return 0;
}

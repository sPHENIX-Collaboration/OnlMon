#ifndef INTT_MON_CONSTANTS_H
#define INTT_MON_CONSTANTS_H

#include <iostream>

namespace INTT
{
	constexpr int LAYER_OFFSET = 0;
	constexpr int CHIP_OFFSET = 1;

	const int FELIX = 8;
	const int FELIX_CHANNEL = 14;

	constexpr int LAYER = 4;
	constexpr int LADDER[LAYER] = {12, 12, 16, 16};
	constexpr int ARM = 2;

	constexpr int CHIP = 26;
	constexpr int CHANNEL = 128;
	constexpr int ADC = 8;

	constexpr int LADDERS_()
	{
		int L = 0;
	
		for(int layer = 0; layer < LAYER; layer++)
		{
			L += LADDER[layer];
		}
	
		return L;
	}
	
	constexpr int LADDERS = LADDERS_();

	constexpr int FELIX_CHANNELS = FELIX * FELIX_CHANNEL;
	constexpr int CHIPS =  CHIP * FELIX_CHANNELS;
	constexpr int CHANNELS = CHANNEL * CHIPS;
	constexpr int ADCS = CHANNELS * ADC;

	struct Indexes_s
	{
		int lyr = 0;
		int ldr = 0;
		int arm = 0;
		int chp = 0;
		int chn = 0;
		int adc = 0;
	};

	void GetGlobalBinFromIndexes(int&, struct Indexes_s const&);
	void GetGlobalIndexesFromBin(int, struct Indexes_s&);

	void GetFelixBinFromIndexes(int&, int, struct Indexes_s const&);
	void GetFelixIndexesFromBin(int, int&, struct Indexes_s&);

	//namespace HitMap
	//{
	//	int FindGlobalBin(int& bin, int layer, int ladder, int northsouth, int chip, int channel)
	//	{
	//		layer -= LAYER_OFFSET;
	//		chip -= CHIP_OFFSET;
	//		
	//		if(!(0 <= layer && layer < LAYER))return 1;
	//		if(!(0 <= ladder && ladder < LADDER[layer]))return 1;
	//		if(!(0 <= northsouth && northsouth < ARM))return 1;
	//		if(!(0 <= chip && chip < CHIP))return 1;
	//		if(!(0 <= channel && channel < CHANNEL))return 1;
	//		
	//		while(layer > 0)
	//		{
	//			ladder += LADDER[layer - 1];
	//			layer--;
	//		}
	//		
	//		bin = channel + CHANNEL * (chip + CHIP * (northsouth + ARM * (ladder)));
	//		if(!(0 <= bin && bin < CHANNELS))
	//		{
	//			std::cout << "Bad FindGlobalBin()" << std::endl;
	//			return 1;
	//		}
	//		
	//		bin += 1;
	//		
	//		return 0;
	//	}

	//	int FindGlobalIndices(int bin, int& layer, int& ladder, int& northsouth, int& chip, int& channel)
	//	{
	//		bin -= 1;
	//		if(!(0 <= bin && bin < CHANNELS))return 1;
	//		
	//		channel = bin % CHANNEL;
	//		bin /= CHANNEL;
	//		
	//		chip = bin % CHIP;
	//		bin /= CHIP;
	//		
	//		northsouth = bin % ARM;
	//		bin /= ARM;
	//		
	//		ladder = bin;
	//		
	//		layer = 0;
	//		while(ladder >= LADDER[layer])
	//		{
	//			ladder -= LADDER[layer];
	//			layer++;
	//		}

	//		bool b = false;
	//		if(!(0 <= layer && layer < LAYER))b = true;
	//		if(!(0 <= ladder && ladder < LADDER[layer]))b = true;
	//		if(!(0 <= northsouth && northsouth < ARM))b = true;
	//		if(!(0 <= chip && chip < CHIP))b = true;
	//		if(!(0 <= channel && channel < CHANNEL))b = true;

	//		if(b)
	//		{
	//			std::cout << "Bad FindGlobalIndices()" << std::endl;
	//			return 1;
	//		}
	//		
	//		layer += LAYER_OFFSET;
	//		chip += CHIP_OFFSET;
	//		
	//		return 0;
	//	}

	//	int FindLayerBin(int& bin, int layer, int ladder, int northsouth, int chip)
	//	{
	//		layer -= LAYER_OFFSET;
	//		chip -= CHIP_OFFSET;

	//		if(!(0 <= layer && layer < LAYER))return 1;
	//		if(!(0 <= ladder && ladder < LADDER[layer]))return 1;
	//		if(!(0 <= northsouth && northsouth < ARM))return 1;
	//		if(!(0 <= chip && chip < CHIP))return 1;

	//		if(northsouth == 0) //North
	//		{
	//			//binx = 2 * ladder + 2 - chip / (CHIP / 2);
	//			//biny = CHIP - chip % (CHIP / 2);

	//				//binx					//biny				//BINX
	//			bin =	(2 * ladder + 2 - chip / (CHIP / 2)) +	(CHIP - chip % (CHIP / 2)) *	(2 * LADDER[layer] + 2);
	//		}

	//		if(northsouth == 1) //South
	//		{
	//			//binx = 2 * ladder + 1 + chip / (CHIP / 2);
	//			//biny = chip % (CHIP / 2) + 1;

	//				//binx					//biny				//BINX
	//			bin =	(2 * ladder + 1 + chip / (CHIP / 2)) +	(chip % (CHIP / 2) + 1) *	(2 * LADDER[layer] + 2);
	//		}

	//		return 0;
	//	}

	//	int FindLayerIndices(int bin, int layer, int& ladder, int& northsouth, int& chip)
	//	{
	//		layer -= LAYER_OFFSET;

	//		if(!(1 <= bin && bin < (2 * LADDER[layer] + 2) * (CHIP + 2)))return 1;
	//		if(!(0 <= layer && layer < LAYER))return 1;

	//		int binx = bin % (2 * LADDER[layer] + 2) - 1;
	//		int biny = bin / (2 * LADDER[layer] + 2) - 1;

	//		ladder = binx / 2;
	//		northsouth = 1 - biny / (CHIP / 2);

	//		//ladder = (bin % (2 * LADDER[layer] + 2) - 1) / 2;
	//		//northsouth = 1 - (bin / (2 * LADDER[layer] + 2) - 1) / (CHIP / 2);

	//		if(northsouth == 0) //North
	//		{
	//			chip = CHIP / 2 - biny % (CHIP / 2) + (1 - binx % 2) * (CHIP / 2) - 1;
	//			//chip = CHIP / 2 - (bin / (2 * LADDER[layer] + 2) - 1) % (CHIP / 2) + ((1 - (bin % (2 * LADDER[layer] + 2) - 1)) % 2) * (CHIP / 2) - 1;
	//		}

	//		if(northsouth == 1) //South
	//		{
	//			chip = biny % (CHIP / 2) + (binx % 2) * (CHIP / 2);
	//			//chip = (bin / (2 * LADDER[layer] + 2) - 1) % (CHIP / 2) + ((bin % (2 * LADDER[layer] + 2) - 1) % 2) * (CHIP / 2);
	//		}

	//		chip += CHIP_OFFSET;

	//		return 0;
	//	}
	//}

	//namespace ADCN
	//{
	//	int FindGlobalBin(int& bin, int layer, int ladder, int northsouth, int chip, int channel, int adc)
	//	{
	//		layer -= LAYER_OFFSET;
	//		chip -= CHIP_OFFSET;
	//		
	//		if(!(0 <= layer && layer < LAYER))return 1;
	//		if(!(0 <= ladder && ladder < LADDER[layer]))return 1;
	//		if(!(0 <= northsouth && northsouth < ARM))return 1;
	//		if(!(0 <= chip && chip < CHIP))return 1;
	//		if(!(0 <= channel && channel < CHANNEL))return 1;
	//		if(!(0 <= adc && adc < ADC))return 1;
	//		
	//		while(layer > 0)
	//		{
	//			ladder += LADDER[layer - 1];
	//			layer--;
	//		}
	//		
	//		bin = adc + ADC * (channel + CHANNEL * (chip + CHIP * (northsouth + ARM * (ladder))));
	//		
	//		bin += 1;
	//		
	//		return 0;
	//	}

	//	int FindGlobalIndices(int bin, int& layer, int& ladder, int& northsouth, int& chip, int& channel, int& adc)
	//	{
	//		bin -= 1;
	//		if(!(0 <= bin && bin < ADCS))return 1;
	//	
	//		adc = bin % ADC;
	//		bin /= ADC;
	//
	//		channel = bin % CHANNEL;
	//		bin /= CHANNEL;
	//		
	//		chip = bin % CHIP;
	//		bin /= CHIP;
	//		
	//		northsouth = bin % ARM;
	//		bin /= ARM;
	//		
	//		ladder = bin;
	//		
	//		layer = 0;
	//		while(ladder >= LADDER[layer])
	//		{
	//			ladder -= LADDER[layer];
	//			layer++;
	//		}
	//		
	//		layer += LAYER_OFFSET;
	//		chip += CHIP_OFFSET;
	//		
	//		return 0;
	//	}

	//	int FindLayerBin(int& bin, int layer, int ladder, int northsouth, int chip, int adc)
	//	{
	//		layer -= LAYER_OFFSET;
	//		chip -= CHIP_OFFSET;

	//		if(!(0 <= layer && layer < LAYER))return 1;
	//		if(!(0 <= ladder && ladder < LADDER[layer]))return 1;
	//		if(!(0 <= northsouth && northsouth < ARM))return 1;
	//		if(!(0 <= chip && chip < CHIP))return 1;
	//		if(!(0 <= adc && adc < ADC))return 1;

	//		if(northsouth == 0) //North
	//		{
	//			//binx = 2 * ladder + 2 - chip / (CHIP / 2);
	//			//biny = CHIP - chip % (CHIP / 2);

	//				//binx					//biny				//BINX			//binz	//BINX * BINY
	//			bin =	(2 * ladder + 2 - chip / (CHIP / 2)) +	(CHIP - chip % (CHIP / 2)) *	(LADDER[layer] + 2) +	adc *	(2 * LADDER[layer] + 2) * (CHIP + 2);
	//		}

	//		if(northsouth == 1) //South
	//		{
	//			//binx = 2 * ladder + 1 + chip / (CHIP / 2);
	//			//biny = chip % (CHIP / 2) + 1;

	//				//binx					//biny				//BINX			//binz	//BINX * BINY
	//			bin =	(2 * ladder + 1 + chip / (CHIP / 2)) +	(chip % (CHIP / 2) + 1) *	(LADDER[layer] + 2) +	adc *	(2 * LADDER[layer] + 2) * (CHIP + 2);
	//		}

	//		return 0;
	//	}

	//	int FindLayerIndices(int bin, int layer, int& ladder, int& northsouth, int& chip, int& adc)
	//	{
	//		layer -= LAYER_OFFSET;

	//		if(!(1 <= bin && bin < (2 * LADDER[layer] + 2) * (CHIP + 2)))return 1;
	//		if(!(0 <= layer && layer < LAYER))return 1;

	//		//int binx = bin % (2 * LADDER[layer] + 2) - 1;
	//		//int biny = bin / (2 * LADDER[layer] + 2) - 1;

	//		ladder = (bin % (2 * LADDER[layer] + 2) - 1) / 2;
	//		northsouth = 1 - (bin / (2 * LADDER[layer] + 2) - 1) / (CHIP / 2);

	//		if(northsouth == 0) //North
	//		{
	//			//chip = CHIP / 2 - biny % (CHIP / 2) + (1 - binx % 2) * (CHIP / 2) - 1;
	//			chip = CHIP / 2 - (bin / (2 * LADDER[layer] + 2) - 1) % (CHIP / 2) + ((1 - (bin % (2 * LADDER[layer] + 2) - 1)) % 2) * (CHIP / 2) - 1;
	//		}

	//		if(northsouth == 1) //South
	//		{
	//			//chip = biny % (CHIP / 2) + (binx % 2) * (CHIP / 2);
	//			chip = (bin / (2 * LADDER[layer] + 2) - 1) % (CHIP / 2) + ((bin % (2 * LADDER[layer] + 2) - 1) % 2) * (CHIP / 2);
	//		}

	//		adc = bin / (2 * LADDER[layer] + 2) * (CHIP + 2) - 1;

	//		chip += CHIP_OFFSET;

	//		return 0;	
	//	}
	//}
}

//      Ladder Structure                //
//======================================//
//      U14     U1      Type B  North   //
//      U15     U2        .       .     //
//      U16     U3        .       .     //
//      U17     U4        .       .     //
//      U18     U5      Type B    .     //
//------------------------------  .     //
//      U19     U6      Type A    .     //
//      U20     U7        .       .     //
//      U21     U8        .       .     //
//      U22     U9        .       .     //
//      U23     U10       .       .     //
//      U24     U11       .       .     //
//      U25     U12       .       .     //
//      U26     U13     Type A  North   //
//--------------------------------------//
//      U13     U26     Type A  South   //
//      U12     U25       .       .     //
//      U11     U24       .       .     //
//      U10     U23       .       .     //
//      U9      U22       .       .     //
//      U8      U21       .       .     //
//      U7      U20       .       .     //
//      U6      U19     Type A    .     //
//------------------------------  .     //
//      U5      U18     Type B    .     //
//      U4      U17       .       .     //
//      U3      U16       .       .     //
//      U2      U15       .       .     //
//      U1      U14     Type B  South   //
//======================================//

#endif

#include "InttMonConstants.h"

void INTT::GetGlobalBinFromIndexes(int& b, struct Indexes_s const& indexes)
{
	int s = 1;
	b = 0;

	b += indexes.adc * s;
	s *= ADC;

	b += indexes.chn * s;
	s *= CHANNEL;

	b += indexes.chp * s;
	s *= CHIP;

	b += indexes.arm * s;
	s *= ARM;

	for(int i = 0; i < indexes.lyr; ++i)
	{
		b += LADDER[i] * s;
	}
	b += indexes.ldr * s;

	++b;
}

void INTT::GetGlobalIndexesFromBin(int b, struct Indexes_s& indexes)
{
	--b;

	indexes.adc = b % ADC;
	b /= ADC;

	indexes.chn = b % CHANNEL;
	b /= CHANNEL;

	indexes.chp = b % CHIP;
	b /= CHIP;

	indexes.arm = b % ARM;
	b /= ARM;

	indexes.lyr = 0;	
	while(b >= LADDER[indexes.lyr])
	{
		b -= LADDER[indexes.lyr];
		++indexes.lyr;
	}

	indexes.ldr = b;
}

void INTT::GetFelixBinFromIndexes(int& b, int felix_channel, struct Indexes_s const& indexes)
{
	int s = 1;
	b = 0;

	b += indexes.adc * s;
	s *= ADC;

	b += indexes.chn * s;
	s *= CHANNEL;

	b += indexes.chp * s;
	s *= CHIP;

	b += felix_channel * s;

	++b;
}

void INTT::GetFelixIndexesFromBin(int b, int& felix_channel, struct Indexes_s& indexes)
{
	--b;

	indexes.adc = b % ADC;
	b /= ADC;

	indexes.chn = b % CHANNEL;
	b /= CHANNEL;

	indexes.chp = b % CHIP;
	b /= CHIP;

	felix_channel = b;
}

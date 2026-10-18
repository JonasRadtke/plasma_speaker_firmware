/*
 * oszillatoren.c
 *
 * Created: 23.05.2017 12:30:01
 *  Author: Radtke
 */ 

#include "oszillatoren.h"

chan channel[NO_OSCI];
noiseChan singleNoise;

//uint32_t sustainVolume = 0x0000FFFF/2;
uint32_t delayzeit = 10;  //
uint32_t releasezeit = 15; //STANDARD MINIMUM VALUES
uint32_t noisezeit = 11; // 
uint32_t attackzeit = 15; //

// Noten
float notes[] =		{8.1758  ,		8.662 ,		 9.177 ,	9.7227,		10.3009,	10.9134,	11.5623,	12.2499,	12.9783,	 13.75,	   14.5676,		15.4339,
					16.3516  ,	   17.3239,	   	18.354 ,   19.4454,     20.6017,	21.8268,	23.1247,	24.4997,	25.9565,	  27.5,	   29.1352,		30.8677,
					32.7032  ,	   34.6478,		36.7081,   38.8909,		41.2034,	43.6535,	46.2493,	48.9994,	51.9131,		55,	   58.2705,		61.7354,
					65.4064  ,	   69.2957,		73.4162,   77.7817,		82.4069,	87.3071,	92.4986,	97.9989,   103.8262,	   110,   116.5409,	   123.4708,
					130.8128 ,	  138.5913,	   146.8324,  155.5635,    164.8138,   174.6141,   184.9972,   195.9977,   207.6523,       220,   233.0819,    246.9417,
					261.6256 ,    277.1826,    293.6648,  311.127 ,    329.6276,   349.2282,   369.9944,   391.9954,   415.3047,       440,   466.1638,    493.8833,
					523.2511 ,    554.3653,    587.3295,  622.254 ,    659.2551,   698.4565,   739.9888,   783.9909,   830.6094,       880,   932.3275,    987.7666,
					1046.5023,   1108.7305,   1174.6591, 1244.5079,   1318.5102,  1396.9129,  1479.9777,  1567.9817,  1661.2188,      1760,  1864.655 ,   1975.5332,
					2093.0045,   2217.461 ,   2349.3181, 2489.0159,   2637.0205,  2793.8259,  2959.9554,  3135.9635,  3322.4376,      3520,  3729.3101,   3951.0664,
					4186.009 ,   4434.9221,   4698.6363, 4978.0317,   5274.0409,  5587.6517,  5919.9108,  6271.927 ,  6644.8752,      7040,  7458.6202,   7902.1328,
					8372.0181,   8869.8442,   9397.2726, 9956.0635,  10548.0818, 11175.3034, 11839.8215, 12543.854};

// Triangle LUT
uint32_t triangletab[TRITAB] = {15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
// Sine LUT
uint32_t sinwavetab[SINTAB] = {	1024,1049,1074,1099,1124,1149,1174,1199,1223,1248,1272,1297,1321,1345,1368,1392,1415,1438,1461,1484,1506,1528,1550,1571,1592,1613,
								1633,1653,1673,1692,1711,1730,1748,1765,1782,1799,1815,1831,1846,1861,1875,1889,1902,1914,1927,1938,1949,1960,1970,1979,1988,1996,
								2003,2010,2017,2023,2028,2032,2036,2040,2043,2045,2046,2047,2048,2047,2046,2045,2043,2040,2036,2032,2028,2023,2017,2010,2003,1996,
								1988,1979,1970,1960,1949,1938,1927,1914,1902,1889,1875,1861,1846,1831,1815,1799,1782,1765,1748,1730,1711,1692,1673,1653,1633,1613,
								1592,1571,1550,1528,1506,1484,1461,1438,1415,1392,1368,1345,1321,1297,1272,1248,1223,1199,1174,1149,1124,1099,1074,1049,1024,998,
								973,948,923,898,873,848,824,799,775,750,726,702,679,655,632,609,586,563,541,519,497,476,455,434,414,394,374,355,336,317,299,282,
								265,248,232,216,201,186,172,158,145,133,120,109,98,87,77,68,59,51,44,37,30,24,19,15,11,7,4,2,1,0,0,0,1,2,4,7,11,15,19,24,30,37,
								44,51,59,68,77,87,98,109,120,133,145,158,172,186,201,216,232,248,265,282,299,317,336,355,374,394,414,434,455,476,497,519,541,
								563,586,609,632,655,679,702,726,750,775,799,824,848,873,898,923,948,973,998};

uint32_t divider[] = { 1, 2, 4, 8, 16, 24, 32, 40, 50, 63, 95, 127, 190, 254, 508, 1017 }; // Divider for the Noise LFSR

void TC2_Handler()
{
	// LFSR for Noise Channel
	noise(channel, &singleNoise);
}

void oscillator(chan *x){

	if (x->oscillator_on >= 1) // Falls der Oszillator eingeschaltet ist wird das Signal generiert
	{
		switch(x->waveform) {
			//rectangle
			// Hier wird das Rechtecktsignal erzeugt. Es wird mit Z�hlwert 0 High gesetzt und mit dem rect_low signal auf Low gesetzt
			// Rect_low wurde in der Main berechnet, je nach Duty Cycle
			// Der Endwert bestimmt die Frequenz. Der Z�hlwert wird auf 0 gesetzt.
			case RECTANGLE:
				if (x->rect_count <= x->rect_low) // Wenn Dutycycle noch nicht erreicht Rechteck High!
				{
					x->chan_out = (x->envelopeVolume >> 1);
				}
				else
				{
					x->chan_out = -(x->envelopeVolume >> 1);
				}
		
				x->rect_count++; // Zaehlwert erhoehen
		
				if (x->rect_count >= x->rect_end) // Am Ende wieder zu�cksetzen
				{
					x->rect_count = 0;
				}
			break;
			//triangle
			// Phaseaccu for Triangle Table (DDS)
			// Der Phasenakkumulator ist 32 bit, die h�chsten 8 bit werden zum springen der Tabelle benutzt, 24bit sind quasi Nachkommastellen.
			// In einer eigenen ISR, vermutlich 100khz, werden zu den 24bit immer die Stepsize dazu addiert. Diese wird einfach vorberechnet.
			// Bei jeden �berlauf der 24bit erh�ht sich dadurch der Index der Tabelle. 0000 0001 + 24bit. Durch anpassen der Stepsize geschieht dies schneller oder langsamer.
			case TRIANGLE:
			    x->dds_tri_counter.counter += x->tri_stepsize;
			    if (x->dds_tri_counter.phase > 31)
			    {
			        x->dds_tri_counter.phase = 0;
			    }

			    int32_t tri_centered = (int32_t)triangletab[x->dds_tri_counter.phase] - 7;// - 7;  // Bereich ca. -7 bis +8

			    x->chan_out = (int32_t)(((int64_t)tri_centered * x->envelopeVolume) / (2 * 15));
			break;

			case SINEWAVE:
			    x->dds_sin_counter.counter += x->sin_stepsize;
			    if (x->dds_sin_counter.phase > SINTAB - 1)
			    {
			        x->dds_sin_counter.phase = 0;
			    }

			    int32_t sin_centered = (int32_t)sinwavetab[x->dds_sin_counter.phase] - 1024;

			    x->chan_out = (int32_t)(((int64_t)sin_centered * x->envelopeVolume) / (2 * 1024));
			break;
		
			//noise
			case NOISE:
			    if ((x->noise_lfsr & 0x01))
			    {
			        x->chan_out = (int32_t)(x->envelopeVolume >> 1);
			    }
			    else
			    {
			        x->chan_out = -(int32_t)(x->envelopeVolume >> 1);
			    }
			break;
		
			default:
			break;
		}
	}
	else //Falls der Oszillator nicht belegt ist wird der Ausgangswert gel�scht.
	{
		x->chan_out = 0;
	}

}


void noise(chan x[], noiseChan *y){
	y->noise_cnt++;
	if (y->noise_cnt >= y->noise_divider)
	{
		y->noise_cnt = 0;
		// Hier LFSR weiter t�deln
		if ((y->noise_metal) >= 1)
		{
			y->noise_bit  = ((y->noise_lfsr >> 0) ^ (y->noise_lfsr >> 6)) & 1; // 0 Bit XOR 6 Bit ( Metallic Noise )
		}
		else
		{
			y->noise_bit  = ((y->noise_lfsr >> 0) ^ (y->noise_lfsr >> 1)) & 1;	// 0 bit XOR 1 bit ( Normal Noise )
		}
		if (y->noise_bit)
		{
			y->noise_lfsr =  (y->noise_lfsr >> 1) | 0x20000; // Faster than shift 14 times left
		}
		else
		{
			y->noise_lfsr =  (y->noise_lfsr >> 1);
		}
	//	y->noise_lfsr =  (y->noise_lfsr >> 1) | (y->noise_bit << 14);	// Little bit slower than if else 
	}
	
	uint32_t i;
	for (i=0; i<7; i++)
	{
		x[i].noise_lfsr = y->noise_lfsr; // Copy General Noise in every Channel (Necessary in this Project, in further projects not used)
	}
	
}

void activateChannel(keyMidi key[],Settings set[], chan x[], float note[], uint32_t div[]){

	int8_t freeChannel = 0;
	uint8_t midiChannel = 0;
	
	// Search for pressed Key
	for (uint8_t keyNumber = 0; keyNumber<127; keyNumber++)
	{
		if (key[keyNumber].key) // If Key/Note active
		{
			freeChannel = _searchFreeChannel(x, keyNumber);	// looking for free channel
			midiChannel = key[keyNumber].channelNumber;
			if (freeChannel == -1)	// If no Channel is free, return
			{
				return;
			}
			else // If free channel is found
			{
				_calculateChannelSettings(x, set[midiChannel] ,freeChannel, keyNumber, note, divider); // Calculate the Settings for the specific channel
			}
		}
	}
}

int8_t _searchFreeChannel(chan x[], uint8_t key){
	uint8_t i = 0;
	
	// Looking for pressed key has already a active channel
	for (i = 0; i<NO_OSCI; i++)
	{
		if(x[i].pushed_key == key)
		{
			return 101;
		}
	}
	
	// Looking for Free Channel
	for (i = 0; i<NO_OSCI; i++)
	{
		if (x[i].oscillator_on == 0)
		{
			return i; // Return Number of free Channel
		}
	}
	return -1; // Return no free Channel found
}

void _calculateChannelSettings(chan x[], Settings set ,uint8_t channelIndex, uint8_t key, float note[], uint32_t div[]){

	switch (set.waveform){
		
		// Settings for Rectangle Waveform
		case RECTANGLE:
		channel[channelIndex].dutycycle = set.dutyValue*100 / 1024;
		channel[channelIndex].frequency = note[key]; // normally +37, but the first key is not used
		channel[channelIndex].rect_end = SAMPLEFREQ / channel[channelIndex].frequency;
		channel[channelIndex].rect_low = ((uint32_t)channel[channelIndex].rect_end * channel[channelIndex].dutycycle )/ 100;
		channel[channelIndex].waveform = RECTANGLE;
		break;
		
		// Settings for Triangle Waveform
		case TRIANGLE:
		channel[channelIndex].frequency = note[key];
		channel[channelIndex].tri_stepsize = (BIT24*channel[channelIndex].frequency*TRITAB)/SAMPLEFREQ; //Zaehler f�r Phasenakkumulator
		channel[channelIndex].waveform = TRIANGLE;
		break;
		
		// Settings for Sine Waveform
		case SINEWAVE:
		channel[channelIndex].frequency = note[key];
		channel[channelIndex].sin_stepsize = (BIT24*channel[channelIndex].frequency*SINTAB)/SAMPLEFREQ; //Zaehler f�r Phasenakkumulator
		channel[channelIndex].waveform = SINEWAVE;
		break;

		// Settings for Noise
		case NOISE:
		channel[channelIndex].noise_divider = div[(key-1)%16]; // Only 16 divider, repeats after 16 Keys
		channel[channelIndex].waveform = NOISE;
		break;
		
		default:
		break;
	}
	
	if (set.burst)
	{
		channel[channelIndex].waveform = NOISE;
		channel[channelIndex].noise_divider = div[2];
		channel[channelIndex].burstTime = noisezeit;
	}
	else
	{
		channel[channelIndex].burstTime = 0;
		channel[channelIndex].noise_divider = div[15];
	}

	channel[channelIndex].envelopeVolume = 0; // Preset for envelope Volume
	channel[channelIndex].sustainTime = 0;	//Standard Sustaintime for not-arpeggio
	channel[channelIndex].delayTime = delayzeit;
	channel[channelIndex].releaseTime = 300+set.releaseTime;
	channel[channelIndex].sustainVol = set.sustainVolume;

	channel[channelIndex].attackTime = attackzeit;
	channel[channelIndex].envelopeStepAttack = set.attackVolume/channel[channelIndex].attackTime;
	channel[channelIndex].envelopeStep = (set.attackVolume-channel[channelIndex].sustainVol) / channel[channelIndex].delayTime;
	channel[channelIndex].adsrCnt = channel[channelIndex].burstTime + channel[channelIndex].releaseTime + channel[channelIndex].sustainTime + channel[channelIndex].delayTime + channel[channelIndex].attackTime; // adsrCnt counts to Zero, Burst -> decay -> release
	
	channel[channelIndex].pushed_key = key;  // Write pushed key in Channel struct
	channel[channelIndex].oscillator_on = 1;	// OSC on as last command!
}

void envelopChannel(keyMidi key[] ,chan x[]){

	// Looking for activated Channel
	for (uint8_t chanNumber = 0; chanNumber<8; chanNumber++)
	{
		if (!(key[x[chanNumber].pushed_key].key && key[x[chanNumber].pushed_key].key != 0)) // If Key released, calculate new step
		{
			if ((x[chanNumber].oscillator_on >= 1) & !(x[chanNumber].releaseActive == 1) ) // If Oscillator is On
			{
				// If Sustain already active
				if (x[chanNumber].adsrCnt <= x[chanNumber].delayTime)
				{
					x[chanNumber].envelopeStep = x[chanNumber].sustainVol / x[chanNumber].releaseTime;
				}
				else // Sustain not active, release Volume from actual Delay Volume
				{
					x[chanNumber].envelopeStep = x[chanNumber].envelopeVolume / x[chanNumber].releaseTime;
					x[chanNumber].adsrCnt = x[chanNumber].releaseTime;
				}
				x[chanNumber].releaseActive = 1;
			}
		}
		else if (x[chanNumber].releaseActive)
		{
			x[chanNumber].releaseActive=0;
			x[chanNumber].envelopeStepAttack=(0x0000FFFF-x[chanNumber].envelopeVolume)/x[chanNumber].attackTime;
			x[chanNumber].adsrCnt = x[chanNumber].releaseTime + x[chanNumber].sustainTime + x[chanNumber].delayTime + x[chanNumber].attackTime; // adsrCnt counts to Zero, attack -> decay -> sustain -> release
		}
	}	
			
	// Generate ADSR

	for (uint8_t chanNumber = 0; chanNumber<8; chanNumber++) // 8 Channel
	{
		if (x[chanNumber].oscillator_on >= 1) // If Oscillator is On
		{
			// Burst active
			if (x[chanNumber].adsrCnt > (x[chanNumber].releaseTime + x[chanNumber].sustainTime  + x[chanNumber].delayTime + x[chanNumber].attackTime))
			{
				x[chanNumber].adsrCnt--;
			}
			//Attack active
			else if (x[chanNumber].adsrCnt > (x[chanNumber].releaseTime + x[chanNumber].sustainTime + x[chanNumber].delayTime))
			{
				x[chanNumber].envelopeVolume += x[chanNumber].envelopeStepAttack;
				x[chanNumber].adsrCnt--;
			}
			// Decay active
			else if (x[chanNumber].adsrCnt > (x[chanNumber].releaseTime + x[chanNumber].sustainTime)) // Delayphase
			{
				x[chanNumber].envelopeVolume -= x[chanNumber].envelopeStep;
				x[chanNumber].adsrCnt--;
			//	x[chanNumber].waveform = set.waveform;
			}
			// Sustain active
			else if ((x[chanNumber].adsrCnt <= x[chanNumber].releaseTime + x[chanNumber].sustainTime) & !x[chanNumber].releaseActive) // Sustainphase (Constant Volume)
			{
				x[chanNumber].envelopeVolume = x[chanNumber].sustainVol;
				x[chanNumber].adsrCnt = x[chanNumber].releaseTime;
			}
			// Release active
			else if (x[chanNumber].releaseActive)		// Releasephase
			{
				x[chanNumber].adsrCnt--;
				x[chanNumber].envelopeVolume -= x[chanNumber].envelopeStep;
			}
			// Counter End
			if (x[chanNumber].adsrCnt <= 0) // End of Tone
			{
				x[chanNumber].loadNextNote=1;
				x[chanNumber].oscillator_on = 0;
				x[chanNumber].releaseActive = 0;
				x[chanNumber].pushed_key = 0;
				x[chanNumber].waveform = 0;
				x[chanNumber].chan_out = 0;
			}
		}	
	}
	
}

void sortInArpegNote(uint8_t arpegNotes[], uint8_t newkey, uint8_t position)
{
	uint8_t i=0;
	uint8_t keytemp1,keytemp2;
	
	//sort in new Note or overwrite old one if new Note is higher
	
	if(arpegNotes[position]>newkey)
	{
		
		keytemp1=arpegNotes[position];
		arpegNotes[position]=newkey;
		
			while(arpegNotes[position+i]>0)
			{
				keytemp2=arpegNotes[position+i+1];
				arpegNotes[position+i+1]=keytemp1;
				keytemp1=keytemp2;
				i++;
			}
	
		}
	else
	{
		arpegNotes[position]=newkey;
	}
	
}

void arpeggiator(chan channel[], Settings settings)
{

if(channel[0].arpegModeActive && channel[0].loadNextNote)
{
	
	if(channel[0].arpegPlayCounter<channel[0].arpegNoteNumber)
	{
		
		//channel[0].frequency = notes[channel[0].arpegNotes[channel[0].arpegPlayCounter]-2 + 29]; // normally +37, but the first key is not used
		//channel[0].rect_end = SAMPLEFREQ / channel[0].frequency;
		//channel[0].rect_low = ((uint32_t)channel[0].rect_end * channel[0].dutycycle )/ 100;
		//channel[0].adsrCnt = channel[0].burstTime + channel[0].releaseTime + channel[0].sustainTime + channel[0].delayTime + channel[0].attackTime; // adsrCnt counts to Zero, Burst -> decay -> release
		
		_calculateChannelSettings(channel,settings,0,channel[0].arpegNotes[channel[0].arpegPlayCounter], notes, divider);

		channel[0].loadNextNote=0;
		channel[0].arpegPlayCounter++;
	}
	else
	{
		channel[0].arpegPlayCounter=0;
	}
	
	
}

return;
}

/*
 * oszillatoren.h
 *
 * Created: 23.05.2017 12:30:14
 *  Author: Radtke
 */ 

#ifndef OSZILLATOREN_H_
#define OSZILLATOREN_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define SAMPLEFREQ			(44100)
//#define PHASEAKKU_FREQ		(100000)
#define NOISEFREQ			(100000) // At NES 223721
#define NO_OSCI				(8)	// Anzahl Channels
#define ENVELOP_TIME		(1)      // in milliseconds
#define DAC_NWRITE			(PIO_PA4) 
#define D0					(PIO_PA5)
#define D1				    (PIO_PA6)
#define D2					(PIO_PA0)//(PIO_PA7)
#define D3					(PIO_PA1)//(PIO_PA8)
#define D4					(PIO_PA9)
#define D5					(PIO_PA10)
#define D6					(PIO_PA11)
#define D7					(PIO_PA12)
#define TRITAB				(32)
#define SINTAB				(256)
#define BIT24				(16777216)
#define LDAC				(PIO_PA12)

#define RECTANGLE			(1)
#define TRIANGLE			(2)
#define SINEWAVE			(3)
#define NOISE				(4)

union dds_cnt_t{
	struct{
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
		uint8_t phase;
	};
	uint32_t counter;
} ;

typedef struct chan1 {
	// General
	uint8_t oscillator_on;	// Chanel On/off
	uint8_t waveform;		// Waveform, RECTANGLE, TRIANGLE, NOISE
	uint8_t pushed_key;		// Number of pushed Key in this Oscillator
	float frequency;		// Frequency of Note
	
	// Envelope
	uint32_t envelopeVolume;	// Actual Volume
	uint32_t envelopeStep;
	uint32_t envelopeStepAttack;
	
	int32_t adsrCnt;			// Counter for Attack,Delay,Sustain,Release - in milliseconds, counts to zero
	
	int32_t delayTime;		// Length of Delay - in milliseconds
	
	uint32_t sustainVol;	// Sustain Volume (constant)
	int32_t arpegSustainTime; //Sustaintime for Arpeggiator
	
	uint32_t releaseActive;   // Release is Active
	int32_t releaseTime;	// Length of Release - in milliseconds
	
	int32_t burstTime;
	
	int32_t attackTime;
	
	uint8_t arpegNotes[50];
	uint8_t arpegNoteNumber;
	uint8_t arpegPlayCounter;
	uint8_t arpegModeActive;
	uint8_t loadNextNote;
	uint8_t arpegAttackActive;
	uint8_t arpegReleaseActive;
	int32_t sustainTime;
	
	// Waveforms
	uint32_t dutycycle;		    // Duty Cycle in percent 0-100%
	uint32_t rect_low;			// Rectangle, this is the Dutycycle 
	uint32_t rect_end;			// Rectangle, end
	uint32_t rect_count;		// Rectangle counter, for internal counting
	
	uint32_t tri_table_index;	// Indexing the Table of Triangle
	uint32_t tri_stepsize;		 // Stepsize for DDS
	uint32_t sin_table_index;	// Indexing the Table of Triangle
	uint32_t sin_stepsize;		 // Stepsize for DDS
	union dds_cnt_t dds_tri_counter;
	union dds_cnt_t dds_sin_counter;
	
	uint32_t noise_lfsr;
	uint32_t noise_cnt;
	uint32_t noise_divider;
	uint32_t noise_bit;
	uint32_t noise_metal; // Metallic Noise
	
	// Outputs
	int32_t chan_out;		// Output of Channel
} chan;

typedef struct noiseChannel {
	// General
	uint32_t noise_lfsr;
	uint32_t noise_cnt;
	uint32_t noise_divider;
	uint32_t noise_bit;
	uint32_t noise_metal; // Metallic Noise
} noiseChan;

typedef struct
{
	uint8_t waveform;
	uint8_t burst;
	uint8_t arpeggio;
	uint8_t Release;
	uint8_t Sustain;
	uint32_t dutyValue;
	uint32_t arpValue;

	// ADSR Settings
	uint32_t delayTime;
	uint32_t attackTime;
	uint32_t attackVolume;
	uint32_t decayTime;
	uint32_t sustainVolume;
	uint32_t releaseTime;

} Settings;

typedef struct
{
	uint8_t key;	// MSB 1 = On, 0-6 Velocity
	uint8_t channelNumber; // Midi Channel (for Settings)

} keyMidi;

void timerInit (void);
void oscillator(chan *);
void noise(chan [], noiseChan *);

void activateChannel(keyMidi key[],Settings set[],chan x[], float note[], uint32_t div[]);
int8_t _searchFreeChannel(chan x[], uint8_t key);
void _calculateChannelSettings(chan x[],Settings, uint8_t channelIndex, uint8_t key, float note[], uint32_t div[]);
void envelopChannel(keyMidi key[] ,chan x[]);
void sortInArpegNote(uint8_t*,uint8_t,uint8_t);
void arpeggiator(chan channel[],Settings settings);

#endif /* OSZILLATOREN_H_ */

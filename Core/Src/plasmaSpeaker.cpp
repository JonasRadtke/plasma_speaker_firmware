/*
 * plasmaSpeaker.cpp
 *
 *  Created on: Jul 8, 2026
 *      Author: Sentry
 */

#include "plasmaSpeaker.h"


extern UART_HandleTypeDef huart1;
extern chan channel[NO_OSCI];
extern float notes[88];
extern uint32_t divider[];

uint32_t arrRight = 4000;
uint32_t arrLeft = 4000;
uint16_t adc_values[2];
uint32_t lastPotiRead = 0;



plasmaSpeaker::plasmaSpeaker() {
	for(uint32_t i = 0; i < 16; i++){
		settings[i].burst = 0;
		settings[i].arpeggio = 0;
		settings[i].Release = 0;
		settings[i].Sustain = 0;
		settings[i].arpValue = 0;

		settings[i].decayTime = 10;

		settings[i].waveform = RECTANGLE; //RECTANGLE;
		settings[i].attackTime = 10;
		settings[i].attackVolume = 0x1FFFFFFF;
		settings[i].releaseTime = 100;
		settings[i].sustainVolume = 0x1FFFFFFF;
		settings[i].dutyValue = 512;
	}
}

void plasmaSpeaker::run(){
	if (newNote.task(HAL_GetTick(), 1)) {
		activateChannel(this->keyVelo, settings ,channel, notes, divider);
		envelopChannel(this->keyVelo ,channel);
	}
	this->readPotis();
}

void plasmaSpeaker::readPotis(){
	  if (HAL_GetTick() - lastPotiRead > 100) {
		  LL_ADC_REG_StartConversionSWStart(ADC1);

	      for (int i = 0; i < 2; i++) {
	    	  while (!LL_ADC_IsActiveFlag_EOCS(ADC1));  // warten auf Ende der Konvertierung
	    	  adc_values[i] = LL_ADC_REG_ReadConversionData12(ADC1);
	    	  LL_ADC_ClearFlag_EOCS(ADC1);
	      }
	      lastPotiRead = HAL_GetTick();

	      uint32_t freq_min = 22000;
	      uint32_t freq_max = 40000;
	      uint32_t arr = freq_min + ((uint32_t)adc_values[0] * (freq_max - freq_min)) / 4095;
	      arrRight = (96000000 / arr) - 1;
	      arr = freq_min + ((uint32_t)adc_values[1] * (freq_max - freq_min)) / 4095;
	      arrLeft = (96000000 / arr) - 1;
	  }
}

void plasmaSpeaker::newMidi(uint8_t midiPacket[]){
	  uint8_t status = midiPacket[1] & 0xF0;
	  uint8_t midiChannel = midiPacket[1] & 0x0F;

	  switch (status) {
	      case 0x80: // Note Off
	      {
	          uint8_t note = midiPacket[2] & 0x7F;
	          //uint8_t velocity = midiPacket[3] & 0x7F;
	          this->keyVelo[note].key = 0;
	          this->keyVelo[note].channelNumber = midiChannel;
	          break;
	      }
	      case 0x90: // Note On
	      {
	          uint8_t note = midiPacket[2] & 0x7F;
	          uint8_t velocity = midiPacket[3] & 0x7F;
	          if (velocity == 0) {
	              this->keyVelo[note].key = 0;  // Note On mit Velocity 0 = Note Off
	              this->keyVelo[note].channelNumber = midiChannel;
	          } else {
	              this->keyVelo[note].key = 0x80 | velocity;
	              this->keyVelo[note].channelNumber = midiChannel;
	          }
	          break;
	      }
	      case 0xB0: // Control Change
	      {
	    	  switch (midiPacket[2]) {
				case 0x07: // Lautstärke
					this->settings[midiChannel].sustainVolume = (0x1FFFFFFF / 127) * midiPacket[3];
					break;
				default:
			//		HAL_UART_Transmit(&huart1, midiPacket, 4, HAL_MAX_DELAY);
					break;
			}
	    	  break;
	      }
	      case 0xC0: // Programm Change gibt es eigentlich nicht
	      {
	    	  uint8_t programmNumber = midiPacket[2];
	     	  break;
	      }
	      default:
		//	  HAL_UART_Transmit(&huart1, midiPacket, 4, HAL_MAX_DELAY);
	          break;
	  }
}

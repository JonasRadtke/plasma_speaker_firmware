/*
 * plasmaSpeaker.h
 *
 *  Created on: Jul 8, 2026
 *      Author: Sentry
 */

#ifndef SRC_PLASMASPEAKER_H_
#define SRC_PLASMASPEAKER_H_


#include "crapScheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "oszillatoren.h"

#ifdef __cplusplus
}
#endif


class plasmaSpeaker {
private:
	crapScheduler newNote;
	keyMidi keyVelo[127] = {0};
	Settings settings[16];
	void readPotis();



public:
	plasmaSpeaker();
	void newMidi(uint8_t midiPacket[]);
	uint8_t keys[6] = {0};	//Array für die Nummern der gedrückten tasten

	void run();
};

#endif /* SRC_PLASMASPEAKER_H_ */

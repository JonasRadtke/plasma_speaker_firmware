/*
 * crapScheduler.h
 *
 *  Created on: Apr 29, 2025
 *      Author: Sentry
 */

#ifndef SRC_CRAPSCHEDULER_H_
#define SRC_CRAPSCHEDULER_H_
#include <stdint.h>
#include "main.h"

class crapScheduler {
private:
	uint32_t lastCall = 0;
public:
	crapScheduler() {
	}
	uint32_t task(uint32_t actualTime, uint32_t taskIntervallMs){
		if(this->lastCall + taskIntervallMs < actualTime){
			this->lastCall = actualTime;
			return 1;
		}
		return 0;
	}

};

void crapDelayMS(uint32_t delayMs);


#endif /* SRC_CRAPSCHEDULER_H_ */

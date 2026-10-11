/*
 * crapScheduler.h
 *
 *  Created on: Apr 29, 2025
 *      Author: Sentry
 */

#include <stdint.h>
#include "main.h"
#include "crapScheduler.h"

void crapDelayMS(uint32_t delayMs){
	uint32_t actualTime = HAL_GetTick();
	while(!((HAL_GetTick() - actualTime) >= delayMs));
}

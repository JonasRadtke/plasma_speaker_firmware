/*
 * bandpass.h
 *
 *  Created on: Oct 3, 2026
 *      Author: Sentry
 */

#ifndef SRC_BANDPASS_H_
#define SRC_BANDPASS_H_


#include <stdint.h>

#define NUM_STAGES 4

typedef struct {
    float b0, b1, b2, a1, a2;
    float z1, z2;
} Biquad;

int32_t process_sample(int32_t input);

#endif /* SRC_BANDPASS_H_ */

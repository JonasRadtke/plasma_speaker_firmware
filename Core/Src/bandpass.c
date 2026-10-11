/*
 * bandpass.c
 *
 *  Created on: Oct 3, 2026
 *      Author: Sentry
 */

#include "bandpass.h"

#include <stdint.h>

#define NUM_STAGES 4

Biquad filter[NUM_STAGES] = {
    {0.0040840906f, 0.0081681812f, 0.0040840906f, -1.2113984311f, 0.4290762017f, 0, 0},
    {1.0000000000f, 2.0000000000f, 1.0000000000f, -1.2682861203f, 0.6999849731f, 0, 0},
    {1.0000000000f, -2.0000000000f, 1.0000000000f, -1.6990530912f, 0.7350880964f, 0, 0},
    {1.0000000000f, -2.0000000000f, 1.0000000000f, -1.8991700979f, 0.9218972955f, 0, 0},
};

static inline float biquad_process(Biquad *f, float x) {
    float y = f->b0 * x + f->z1;
    f->z1 = f->b1 * x - f->a1 * y + f->z2;
    f->z2 = f->b2 * x - f->a2 * y;
    return y;
}

int32_t process_sample(int32_t input) {
    float x = (float)input;
    for (int i = 0; i < NUM_STAGES; i++) {
        x = biquad_process(&filter[i], x);
    }
    return (int32_t)x;
}

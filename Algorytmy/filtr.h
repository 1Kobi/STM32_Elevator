#ifndef FILTR_H_
#define FILTR_H_

#include "arm_math.h"

#define FIR_TAPS 7
#define FIR_BLOCK_SIZE 1

typedef struct {
    arm_fir_instance_f32 S;
    float32_t coeffs[FIR_TAPS];
    float32_t state[FIR_TAPS + FIR_BLOCK_SIZE - 1]; // Pamięć podręczna
    float32_t output; // Wynik (przefiltrowany)
} DistanceFilter_t;

void DistanceFilter_Init(DistanceFilter_t *filtr);
float32_t DistanceFilter_Update(DistanceFilter_t *filtr, float32_t raw_input);

#endif /* FILTR_H_ */

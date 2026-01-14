#include "filtr.h"
#include <string.h>

void DistanceFilter_Init(DistanceFilter_t *filtr) {
	static const float32_t my_coeffs[FIR_TAPS] = {0.00000f, 0.00930f, 0.04758f,
			0.12236f, 0.20225f, 0.23702f, 0.20225f,
			0.12236f, 0.04758f, 0.00930f, 0.00000f};

	memcpy(filtr->coeffs, my_coeffs, sizeof(my_coeffs));
	memset(filtr->state, 0, sizeof(filtr->state));

    arm_fir_init_f32(&filtr->S, FIR_TAPS, filtr->coeffs, filtr->state, FIR_BLOCK_SIZE);
}

float32_t DistanceFilter_Update(DistanceFilter_t *filtr, float32_t raw_input) {
    arm_fir_f32(&filtr->S, &raw_input, &filtr->output, 1);
    return filtr->output;
}

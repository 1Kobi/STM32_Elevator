#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f7xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim;

    volatile float targetHeight;
    int32_t offset;

    int32_t minHeight;
    int32_t maxHeight;

    float pulsesPerCm;

    uint32_t lastCounterValue;
} Encoder_t;

void Encoder_Init(Encoder_t *enc, TIM_HandleTypeDef *htim, int32_t min, int32_t max, int32_t start_val);
void Encoder_Update(Encoder_t *enc);
int32_t Encoder_GetTarget(Encoder_t *enc);

#endif /* ENCODER_H_ */

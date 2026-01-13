#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f7xx_hal.h"

typedef struct {
    // Hardware
    TIM_HandleTypeDef *htim;

    // Logika windy
    volatile int32_t targetHeight; // Zadana wysokość w cm

    // Ograniczenia
    int32_t minHeight;
    int32_t maxHeight;

    // Kalibracja (ile impulsów enkodera to 1 cm)
    float pulsesPerCm;

    // Stan poprzedni (do obliczania delty)
    uint32_t lastCounterValue;
} Encoder_t;

void Encoder_Init(Encoder_t *enc, TIM_HandleTypeDef *htim, int32_t min, int32_t max);
void Encoder_Update(Encoder_t *enc); // Tę funkcję będziesz wołał w pętli/wątku
int32_t Encoder_GetTarget(Encoder_t *enc);

#endif /* ENCODER_H_ */

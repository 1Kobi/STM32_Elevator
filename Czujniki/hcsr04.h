#ifndef HCSR04_H_
#define HCSR04_H_

#include "stm32f7xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    volatile float distance;
} HCSR04_t;

void HCSR04_Init(HCSR04_t *dev, TIM_HandleTypeDef *htim_echo, TIM_HandleTypeDef *htim_trig);
void HCSR04_ProcessISR(HCSR04_t *dev, TIM_HandleTypeDef *htim);


#endif /* HCSR04_H_ */

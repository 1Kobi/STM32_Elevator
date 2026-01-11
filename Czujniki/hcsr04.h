#ifndef HCSR04_H_
#define HCSR04_H_

#include "stm32f7xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    GPIO_TypeDef *trigPort;
    uint16_t trigPin;

    volatile uint32_t startTick;
    volatile uint32_t endTick;
    volatile uint8_t isFirstCapture;

    volatile float distance;
} HCSR04_t;

void HCSR04_Init(HCSR04_t *dev, TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *trigPort, uint16_t trigPin);
void HCSR04_Trigger(HCSR04_t *dev);
void HCSR04_ProcessISR(HCSR04_t *dev, TIM_HandleTypeDef *htim);


#endif /* HCSR04_H_ */

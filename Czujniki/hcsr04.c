#include "hcsr04.h"

static void HCSR04_DelayUs(TIM_HandleTypeDef *htim, uint16_t us) {
    __HAL_TIM_SET_COUNTER(htim, 0);
    while (__HAL_TIM_GET_COUNTER(htim) < us);
}

void HCSR04_Init(HCSR04_t *dev, TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *trigPort, uint16_t trigPin) {
    dev->htim = htim;
    dev->channel = channel;
    dev->trigPort = trigPort;
    dev->trigPin = trigPin;

    dev->isFirstCapture = 1;
    dev->distance = 0.0f;

    HAL_TIM_Base_Start(htim);
}

void HCSR04_Trigger(HCSR04_t *dev) {

    dev->isFirstCapture = 1;
    __HAL_TIM_SET_CAPTUREPOLARITY(dev->htim, dev->channel, TIM_INPUTCHANNELPOLARITY_RISING);

    HAL_TIM_IC_Start_IT(dev->htim, dev->channel);

    HAL_GPIO_WritePin(dev->trigPort, dev->trigPin, GPIO_PIN_SET);
    HCSR04_DelayUs(dev->htim, 10);
    HAL_GPIO_WritePin(dev->trigPort, dev->trigPin, GPIO_PIN_RESET);
}

void HCSR04_ProcessISR(HCSR04_t *dev, TIM_HandleTypeDef *htim) {

    if (htim->Instance == dev->htim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {

        if (dev->isFirstCapture) {
            // Rising edge
            dev->startTick = HAL_TIM_ReadCapturedValue(htim, dev->channel);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, dev->channel, TIM_INPUTCHANNELPOLARITY_FALLING);
            dev->isFirstCapture = 0;
        } else {
            // Falling edge
            dev->endTick = HAL_TIM_ReadCapturedValue(htim, dev->channel);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, dev->channel, TIM_INPUTCHANNELPOLARITY_RISING);

            uint32_t diff;
            if (dev->endTick >= dev->startTick) {
                diff = dev->endTick - dev->startTick;
            } else {
                diff = (0xFFFFFFFF - dev->startTick) + dev->endTick;
            }

            // Wynik w cm (dzielenie przez 58.0f daje float)
            dev->distance = diff / 58.0f;
            if(dev->distance>400||dev->distance<1){
            	dev->distance = 0;
            }
            dev->isFirstCapture = 1;

        }
    }
}


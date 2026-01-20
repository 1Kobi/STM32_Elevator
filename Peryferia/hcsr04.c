#include "../Peryferia/hcsr04.h"


void HCSR04_Init(HCSR04_t *dev, TIM_HandleTypeDef *htim_echo, TIM_HandleTypeDef *htim_trig) {
	dev->htim = htim_echo;

    dev->distance = 0.0f;

    HAL_TIM_PWM_Start(htim_trig, TIM_CHANNEL_1);

    HAL_TIM_IC_Start_IT(htim_echo, TIM_CHANNEL_2);
    HAL_TIM_IC_Start(htim_echo, TIM_CHANNEL_1);
}

void HCSR04_ProcessISR(HCSR04_t *dev, TIM_HandleTypeDef *htim) {

	if (htim->Instance == dev->htim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {

		uint32_t pulseWidth = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

        // Wynik w cm
		dev->distance = pulseWidth / 58.0f;
		if(dev->distance>400||dev->distance<1){
			dev->distance = 0;
		}
	}
}


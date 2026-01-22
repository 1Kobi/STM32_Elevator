#include "../Peryferia/encoder.h"

void Encoder_Init(Encoder_t *enc, TIM_HandleTypeDef *htim, int32_t min, int32_t max) {
    enc->htim = htim;
    enc->minHeight = min;
    enc->maxHeight = max;
    enc->targetHeight = min;
    enc->pulsesPerCm = 4.0f;

    __HAL_TIM_SET_COUNTER(htim, 0);
    enc->lastCounterValue = 0;
    enc->offset = 0;

    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

void Encoder_Update(Encoder_t *enc) {
    uint32_t currentCounter = __HAL_TIM_GET_COUNTER(enc->htim);
    int16_t delta = (int16_t)(currentCounter - enc->lastCounterValue);

	if (delta != 0) {
        enc->lastCounterValue = currentCounter;

        enc->offset += delta;

        int32_t newHeight = enc->offset / (int32_t)enc->pulsesPerCm;

        if (newHeight < enc->minHeight) {
        	enc->targetHeight = enc->minHeight;
        	enc->offset = (enc->minHeight * (int32_t)enc->pulsesPerCm);
        }
        else if (newHeight > enc->maxHeight) {
        	enc->targetHeight = enc->maxHeight;
        	enc->offset = (enc->maxHeight * (int32_t)enc->pulsesPerCm);
        }
        else {
        	enc->targetHeight = newHeight;
        }
    }
}

int32_t Encoder_GetTarget(Encoder_t *enc) {
    return enc->targetHeight;
}

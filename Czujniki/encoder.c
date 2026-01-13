#include "encoder.h"

void Encoder_Init(Encoder_t *enc, TIM_HandleTypeDef *htim, int32_t min, int32_t max) {
    enc->htim = htim;
    enc->minHeight = min;
    enc->maxHeight = max;
    enc->targetHeight = min; // Startujemy od parteru
    enc->pulsesPerCm = 2.0f; // Np. 2 kliknięcia to 1 cm (dobierz eksperymentalnie)

    enc->lastCounterValue = 0;

    // Ustawiamy licznik na połowę zakresu, żeby uniknąć problemów z przekręceniem przy starcie (underflow)
    __HAL_TIM_SET_COUNTER(htim, 30000);
    enc->lastCounterValue = 30000;

    // Start sprzętowego licznika
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

void Encoder_Update(Encoder_t *enc) {
    // 1. Pobierz aktualną wartość z rejestru timera
    uint32_t currentCounter = __HAL_TIM_GET_COUNTER(enc->htim);

    // 2. Oblicz różnicę (Delta) jako int16_t (rzutowanie obsłuży przekręcenie licznika!)
    // To jest trik na obsługę overflow/underflow timera.
    int16_t delta = (int16_t)(currentCounter - enc->lastCounterValue);

    // Jeśli była zmiana
    if (delta != 0) {
        // Zaktualizuj ostatnią pozycję
        enc->lastCounterValue = currentCounter;

        // Przelicz na cm (to tylko przykład logiki)
        // Możesz dodać warunek, że np. zmiana musi być o min. 4 impulsy (1 skok enkodera to często 4 zbocza)
        int32_t changeInCm = delta / (int32_t)enc->pulsesPerCm; // Uproszczone

        // Jeśli dzielimy i reszta znika, to można sterować bezpośrednio 'raw':
        enc->targetHeight += changeInCm;

        // 3. CLAMPING (Ograniczenie zakresu) - Bezpieczeństwo!
        if (enc->targetHeight > enc->maxHeight) enc->targetHeight = enc->maxHeight;
        if (enc->targetHeight < enc->minHeight) enc->targetHeight = enc->minHeight;
    }
}

int32_t Encoder_GetTarget(Encoder_t *enc) {
    return enc->targetHeight;
}

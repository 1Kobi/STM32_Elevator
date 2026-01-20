#ifndef CRC_H_
#define CRC_H_

#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE 64

// Struktura przechowująca stan komunikacji
typedef struct {
    uint8_t rx_byte;                // Ostatni odebrany znak
    char rx_buffer[RX_BUFFER_SIZE]; // Bufor na całe zdanie
    uint8_t rx_index;               // Licznik znaków
    volatile uint8_t msg_ready;     // Flaga: mamy gotową komendę
} CommandState_t;

// Deklaracje funkcji
void CMD_Init(UART_HandleTypeDef *huart);
void CMD_Process(void); // Funkcja wywoływana w pętli while(1)
void CMD_RxCpltCallback(UART_HandleTypeDef *huart); // Callback przerwania

#endif /* CRC_H_ */

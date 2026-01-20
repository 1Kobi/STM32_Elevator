#include "crc.h"
#include "encoder.h"
#include <stdio.h>

extern UART_HandleTypeDef huart3;
extern Encoder_t encoder;

static CommandState_t cmdState;

void CMD_Init(UART_HandleTypeDef *huart) {
    cmdState.rx_index = 0;
    cmdState.msg_ready = 0;
    __HAL_UART_CLEAR_OREFLAG(huart);

    HAL_UART_Receive_IT(huart, &cmdState.rx_byte, 1);
}


static uint32_t Soft_CalculateCRC(char* data_ptr, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < length; i++) {
        crc ^= (uint8_t)data_ptr[i];

        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}

void CMD_Process(void) {
    if (cmdState.msg_ready) {
        cmdState.msg_ready = 0;

        char* separator = strchr(cmdState.rx_buffer, ':');

        if (separator != NULL) {
            *separator = '\0';

            char* cmd_part = cmdState.rx_buffer;
            char* crc_part = separator + 1;

            uint32_t calced_crc = Soft_CalculateCRC(cmd_part, strlen(cmd_part));
            uint32_t received_crc = (uint32_t)strtoul(crc_part, NULL, 16);

            if (calced_crc == received_crc) {
                if (cmd_part[0] == 'H') {
                    int val = atoi(cmd_part + 1);
                    if (val >= 0 && val <= 200) {
                    	encoder.targetHeight = val;
                    	encoder.offset = val * (int32_t)encoder.pulsesPerCm;
                    	encoder.lastCounterValue = __HAL_TIM_GET_COUNTER(encoder.htim);

                    }
                }
            }
        }
    }
}

void CMD_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        if (cmdState.rx_byte == '\n' || cmdState.rx_byte == '\r') {
            if (cmdState.rx_index > 0) {
                cmdState.rx_buffer[cmdState.rx_index] = '\0';
                cmdState.msg_ready = 1;
                cmdState.rx_index = 0;
            }
        } else {
            if (cmdState.rx_index < RX_BUFFER_SIZE - 1) {
                cmdState.rx_buffer[cmdState.rx_index++] = cmdState.rx_byte;
            }
        }
        HAL_UART_Receive_IT(huart, &cmdState.rx_byte, 1);
    }
}

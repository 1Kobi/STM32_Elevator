#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>

#define LCD_ADDR (0x27 << 1)

typedef struct {
	I2C_HandleTypeDef *hi2c; // Uchwyt do I2C
	uint8_t lines;           // Liczba linii (2 lub 4)
	uint8_t columns;         // Liczba kolumn (16 lub 20)
	uint8_t backlight_val;   // Stan podświetlenia
} LCD_t;

// Funkcje
void LCD_Init(LCD_t *lcd, I2C_HandleTypeDef *hi2c, uint8_t cols, uint8_t rows);
void LCD_Clear(LCD_t *lcd);
void LCD_SetCursor(LCD_t *lcd, uint8_t col, uint8_t row);
void LCD_Print(LCD_t *lcd, char *str);
void LCD_Backlight(LCD_t *lcd, uint8_t state); // 1 = ON, 0 = OFF

#endif /* LCD_I2C_H_ */

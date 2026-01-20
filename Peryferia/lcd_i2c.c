#include "lcd_i2c.h"

#define LCD_CMD_CLEAR_DISPLAY 0x01
#define LCD_CMD_RETURN_HOME 0x02
#define LCD_CMD_ENTRY_MODE_SET 0x04
#define LCD_CMD_DISPLAY_CONTROL 0x08
#define LCD_CMD_CURSOR_SHIFT 0x10
#define LCD_CMD_FUNCTION_SET 0x20
#define LCD_CMD_SET_CGRAM_ADDR 0x40
#define LCD_CMD_SET_DDRAM_ADDR 0x80

#define LCD_FLAG_DISPLAY_ON 0x04
#define LCD_FLAG_CURSOR_OFF 0x00
#define LCD_FLAG_BLINK_OFF 0x00
#define LCD_FLAG_4BIT_MODE 0x00
#define LCD_FLAG_2LINE 0x08
#define LCD_FLAG_5x8DOTS 0x00
#define LCD_FLAG_BACKLIGHT 0x08
#define LCD_FLAG_ENABLE 0x04
#define LCD_FLAG_RS 0x01 // 0 = Command, 1 = Data


static void LCD_Write_I2C(LCD_t *lcd, uint8_t data) {
	HAL_I2C_Master_Transmit(lcd->hi2c, LCD_ADDR, &data, 1, 10);
}


static void LCD_Write_Nibble(LCD_t *lcd, uint8_t nibble, uint8_t rs) {
	uint8_t data = nibble << 4;
	data |= rs;
	data |= lcd->backlight_val;

	// 1. Ustaw dane, Enable = 1
	data |= LCD_FLAG_ENABLE;
	LCD_Write_I2C(lcd, data);

	// 3. Ustaw dane, Enable = 0 (Zatrzask danych)
	data &= ~LCD_FLAG_ENABLE;
	LCD_Write_I2C(lcd, data);
}


static void LCD_Send(LCD_t *lcd, uint8_t value, uint8_t rs) {
	uint8_t high_nibble = (value >> 4) & 0x0F;
	uint8_t low_nibble = value & 0x0F;
	LCD_Write_Nibble(lcd, high_nibble, rs);
	LCD_Write_Nibble(lcd, low_nibble, rs);
}

static void LCD_SendCommand(LCD_t *lcd, uint8_t cmd) {
	LCD_Send(lcd, cmd, 0); // RS = 0 dla komend
}

static void LCD_SendData(LCD_t *lcd, uint8_t data) {
	LCD_Send(lcd, data, LCD_FLAG_RS); // RS = 1 dla danych
}


void LCD_Init(LCD_t *lcd, I2C_HandleTypeDef *hi2c, uint8_t cols, uint8_t rows) {
	lcd->hi2c = hi2c;
	lcd->columns = cols;
	lcd->lines = rows;
	lcd->backlight_val = LCD_FLAG_BACKLIGHT; // Podświetlenie włączone

	HAL_Delay(50);

	LCD_Write_Nibble(lcd, 0x03, 0);
	HAL_Delay(5);
	LCD_Write_Nibble(lcd, 0x03, 0);
	HAL_Delay(1);
	LCD_Write_Nibble(lcd, 0x03, 0);
	HAL_Delay(1);

	LCD_Write_Nibble(lcd, 0x02, 0);

	LCD_SendCommand(lcd, LCD_CMD_FUNCTION_SET | LCD_FLAG_4BIT_MODE | LCD_FLAG_2LINE | LCD_FLAG_5x8DOTS);
	LCD_SendCommand(lcd, LCD_CMD_DISPLAY_CONTROL | LCD_FLAG_DISPLAY_ON | LCD_FLAG_CURSOR_OFF | LCD_FLAG_BLINK_OFF);
	LCD_Clear(lcd);
	LCD_SendCommand(lcd, LCD_CMD_ENTRY_MODE_SET | 0x02);
}

void LCD_Clear(LCD_t *lcd) {
	LCD_SendCommand(lcd, LCD_CMD_CLEAR_DISPLAY);
	HAL_Delay(2);
}

void LCD_SetCursor(LCD_t *lcd, uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	if (row >= lcd->lines) row = lcd->lines - 1;
	LCD_SendCommand(lcd, LCD_CMD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void LCD_Print(LCD_t *lcd, char *str) {
	while (*str) {
		LCD_SendData(lcd, (uint8_t)(*str));
		str++;
	}
}

void LCD_Backlight(LCD_t *lcd, uint8_t state) {
	if (state) lcd->backlight_val = LCD_FLAG_BACKLIGHT;
	else lcd->backlight_val = 0;
	LCD_Write_I2C(lcd, lcd->backlight_val);
}

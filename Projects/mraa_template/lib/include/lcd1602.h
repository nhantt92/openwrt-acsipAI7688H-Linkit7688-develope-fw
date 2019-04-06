#ifndef LCD1602_H_
#define LCD1602_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <upm.h>

#include <mraa/gpio.h>

typedef struct _lcd1602_context {
	mraa_gpio_context gpioRS;
	mraa_gpio_context gpioEN;
	mraa_gpio_context gpioD0;
	mraa_gpio_context gpioD1;
	mraa_gpio_context gpioD2;
	mraa_gpio_context gpioD3;

	unsigned int columns;
	unsigned int rows;

	uint8_t displayControl;
	uint8_t entryDisplayMode;
	uint8_t backlight;
} *lcd1602_context;

lcd1602_context lcd1602_gpio_init(int rs, int enable, 
									int d0, int d1, int d2, int d3, 
									uint8_t num_columns, uint8_t num_rows);
void lcd1602_close(lcd1602_context dev);
upm_result_t lcd1602_write(const lcd1602_context dev, char *buffer, int len);
upm_result_t lcd1602_set_cursor(const lcd1602_context dev, 
									unsigned int row, unsigned int column);
upm_result_t lcd1602_clear(const lcd1602_context dev);
upm_result_t lcd1602_home(const lcd1602_context dev);
upm_result_t lcd1602_create_char(const lcd1602_context dev, 
									unsigned int slot,
									char *data);
upm_result_t lcd1602_display_on(const lcd1602_context dev, bool on);
upm_result_t lcd1602_cursor_on(const lcd1602_context dev, bool on);
upm_result_t lcd1602_cursor_blink_on(const lcd1602_context dev, bool on);
upm_result_t lcd1602_scroll_display_left(const lcd1602_context dev);
upm_result_t lcd1602_scroll_display_right(const lcd1602_context dev);
upm_result_t lcd1602_entry_left_to_right(const lcd1602_context dev, bool on);
upm_result_t lcd1602_lcd1602_autoscroll_on(const lcd1602_context dev, bool on);
upm_result_t lcd1602_command(const lcd1602_context dev, uint8_t cmd);
upm_result_t lcd1602_data(const lcd1602_context dev, uint8_t data);

#endif
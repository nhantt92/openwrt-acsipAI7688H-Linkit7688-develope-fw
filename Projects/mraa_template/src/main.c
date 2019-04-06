#include "main.h"

int main(int argc, char** argv)
{
	lcd1602_context lcd = lcd1602_gpio_init(0, 1, 2, 3, 4, 5, 16, 2);
	if(!lcd)
	{
		printf("lcd1602_gpio_init() failed\n");
		return 1;
	}
	int status;
	lcd1602_set_cursor(lcd, 0, 0);
	status = lcd1602_write(lcd, "Hello World 1", 13);
	lcd1602_set_cursor(lcd, 1, 0);
	status = lcd1602_write(lcd, "Hello World 2", 13);
	printf("status: %d\n", status);
	sleep(1);
	lcd1602_close(lcd);
	return 0;
}
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <upm_utilities.h>
#include "lcd1602.h"
#include <mraa.h>
#include <mraa/gpio.h>


// commands
const uint8_t HD44780_CLEARDISPLAY = 0x01;
const uint8_t HD44780_RETURNHOME = 0x02;
const uint8_t HD44780_ENTRYMODESET = 0x04;
const uint8_t HD44780_DISPLAYCONTROL = 0x08;
const uint8_t HD44780_CURSORSHIFT = 0x10;
const uint8_t HD44780_FUNCTIONSET = 0x20;

// flags for display entry mode
const uint8_t HD44780_ENTRYRIGHT = 0x00;
const uint8_t HD44780_ENTRYLEFT = 0x02;
const uint8_t HD44780_ENTRYSHIFTINCREMENT = 0x01;
const uint8_t HD44780_ENTRYSHIFTDECREMENT = 0x00;

// flags for display on/off control
const uint8_t HD44780_DISPLAYON = 0x04;
const uint8_t HD44780_DISPLAYOFF = 0x00;
const uint8_t HD44780_CURSORON = 0x02;
const uint8_t HD44780_CURSOROFF = 0x00;
const uint8_t HD44780_BLINKON = 0x01;
const uint8_t HD44780_BLINKOFF = 0x00;

// flags for display/cursor shift
const uint8_t HD44780_DISPLAYMOVE = 0x08;
const uint8_t HD44780_MOVERIGHT = 0x04;
const uint8_t HD44780_MOVELEFT = 0x00;

// flags for function set
const uint8_t HD44780_8BITMODE = 0x10;
const uint8_t HD44780_4BITMODE = 0x00;
const uint8_t HD44780_2LINE = 0x08;
const uint8_t HD44780_1LINE = 0x00;
const uint8_t HD44780_5x10DOTS = 0x04;
const uint8_t HD44780_5x8DOTS = 0x00;

// flags for CGRAM
const uint8_t HD44780_SETCGRAMADDR = 0x40;

// may be implementation specific
const uint8_t HD44780_EN = 0x04; // Enable bit
const uint8_t HD44780_RW = 0x02; // Read/Write bit
const uint8_t HD44780_RS = 0x01; // Register select bit
const uint8_t HD44780_DATA = 0x40;
const uint8_t HD44780_CMD = 0x80;

const uint8_t HD44780_BACKLIGHT = 0x08;
const uint8_t HD44780_NOBACKLIGHT = 0x00;

static upm_result_t send(const lcd1602_context dev, uint8_t value, int mode);
static upm_result_t write4bits(const lcd1602_context dev, uint8_t value);
//static upm_result_t expandWrite(const lcd1602_context dev, uint8_t value);
//static upm_result_t pulseEnable(const lcd1602_context dev, uint8_t value);

lcd1602_context lcd1602_gpio_init(int rs, int enable, 
									int d0, int d1, int d2, int d3, 
									uint8_t num_columns, uint8_t num_rows)
{
	lcd1602_context dev = (lcd1602_context)malloc(sizeof(struct _lcd1602_context));
	if(!dev)
		return NULL;
	memset((void *)dev, 0, sizeof(struct _lcd1602_context));
	int mraa_rv = mraa_init();
	// if(mraa_rv != MRAA_SUCCESS)
	// {
	// 	printf("%s: mraa_init() failed (%d).\n", __FUNCTION__, mraa_rv);
	// 	lcd1602_close(dev);
	// 	return NULL;
	// }

	// initialize the MRAA contexts
    
	if(!(dev->gpioRS = mraa_gpio_init(rs)))
	{
		printf("%s: mraa_gpio_init(rs) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioRS, MRAA_GPIO_OUT);


	if(!(dev->gpioEN = mraa_gpio_init(enable)))
	{
		printf("%s: mraa_gpio_init(enable) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioEN, MRAA_GPIO_OUT);

	if(!(dev->gpioD0 = mraa_gpio_init(d0)))
	{
		printf("%s: mraa_gpio_init(d0) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioD0, MRAA_GPIO_OUT);

	if(!(dev->gpioD1 = mraa_gpio_init(d1)))
	{
		printf("%s: mraa_gpio_init(d0) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioD1, MRAA_GPIO_OUT);

	if(!(dev->gpioD2 = mraa_gpio_init(d2)))
	{
		printf("%s: mraa_gpio_init(d0) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioD2, MRAA_GPIO_OUT);

	if(!(dev->gpioD3 = mraa_gpio_init(d3)))
	{
		printf("%s: mraa_gpio_init(d0) failed.\n", __FUNCTION__);
		lcd1602_close(dev);
		return NULL;
	}
	mraa_gpio_dir(dev->gpioD3, MRAA_GPIO_OUT);

	dev->backlight = 0;
	dev->columns = num_columns;
	dev->rows = num_rows;

	// set RS and Enable low to begin issuing commands
	mraa_gpio_write(dev->gpioRS, 0);
	mraa_gpio_write(dev->gpioEN, 0);

	// wait to stabilize
	upm_delay_us(1000);

    write4bits(dev, 0x03);
    upm_delay_ms(10);
    write4bits(dev, 0x03);
    upm_delay_ms(150);
    write4bits(dev, 0x03);
    upm_delay_ms(10);
    write4bits(dev, 0x02);
    upm_delay_ms(10);


    // Set number of lines
    lcd1602_command(dev, HD44780_FUNCTIONSET | HD44780_2LINE | HD44780_4BITMODE
                    | HD44780_5x8DOTS);


    dev->displayControl = HD44780_DISPLAYON | HD44780_CURSOROFF
        | HD44780_BLINKOFF;
    lcd1602_command(dev, HD44780_DISPLAYCONTROL | dev->displayControl);
    upm_delay_us(2000);
    lcd1602_clear(dev);

    // Set entry mode.
    dev->entryDisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    lcd1602_command(dev, HD44780_ENTRYMODESET | dev->entryDisplayMode);

    upm_delay_ms(100);

    lcd1602_home(dev);

    return dev;
}

static upm_result_t write4bits(const lcd1602_context dev,
                               uint8_t value)
{
    assert(dev != NULL);

    upm_result_t rv = UPM_SUCCESS;
    mraa_result_t mrv = MRAA_SUCCESS;

    mrv = mraa_gpio_write(dev->gpioEN, 0);
    (value & 0x08) ? mraa_gpio_write(dev->gpioD3, 1) : mraa_gpio_write(dev->gpioD3, 0);
    (value & 0x04) ? mraa_gpio_write(dev->gpioD2, 1) : mraa_gpio_write(dev->gpioD2, 0);
    (value & 0x02) ? mraa_gpio_write(dev->gpioD1, 1) : mraa_gpio_write(dev->gpioD1, 0);
    (value & 0x01) ? mraa_gpio_write(dev->gpioD0, 1) : mraa_gpio_write(dev->gpioD0, 0);
    mrv = mraa_gpio_write(dev->gpioEN, 1);
    upm_delay_us(100);
    mrv = mraa_gpio_write(dev->gpioEN, 0);
    // mrv = mraa_gpio_write(dev->gpioD0, ((value >> 0) & 0x01) );
    // mrv = mraa_gpio_write(dev->gpioD1, ((value >> 1) & 0x01) );
    // mrv = mraa_gpio_write(dev->gpioD2, ((value >> 2) & 0x01) );
    // mrv = mraa_gpio_write(dev->gpioD3, ((value >> 3) & 0x01) );
    if (mrv)
    {
        printf("%s: mraa_gpio_write() failed\n", __FUNCTION__);
        rv = UPM_ERROR_OPERATION_FAILED;
    }

    return rv;
}

upm_result_t lcd1602_command(const lcd1602_context dev, uint8_t cmd)
{
    assert(dev != NULL);
    upm_result_t rv = UPM_SUCCESS;
    mraa_result_t mrv = MRAA_SUCCESS;
    mrv = mraa_gpio_write(dev->gpioRS, 0);
    if (mrv)
    {
        printf("%s: mraa_gpio_write() failed\n", __FUNCTION__);
        rv = UPM_ERROR_OPERATION_FAILED;
    }
    rv = write4bits(dev, cmd>>4);
    rv = write4bits(dev, cmd & 0x0F);
    return rv;
}

upm_result_t lcd1602_data(const lcd1602_context dev, uint8_t data)
{
    assert(dev != NULL);
    upm_result_t rv = UPM_SUCCESS;
    mraa_result_t mrv = MRAA_SUCCESS;
    mrv = mraa_gpio_write(dev->gpioRS, 1);
    if (mrv)
    {
        printf("%s: mraa_gpio_write() failed\n", __FUNCTION__);
        rv = UPM_ERROR_OPERATION_FAILED;
    }
    rv = write4bits(dev, data>>4);
    rv = write4bits(dev, data & 0x0F);
    return rv;
}

void lcd1602_close(lcd1602_context dev)
{
	assert(dev != NULL);

	if (dev->gpioRS)
        mraa_gpio_close(dev->gpioRS);
    if (dev->gpioEN)
        mraa_gpio_close(dev->gpioEN);
    if (dev->gpioD0)
        mraa_gpio_close(dev->gpioD0);
    if (dev->gpioD1)
        mraa_gpio_close(dev->gpioD1);
    if (dev->gpioD2)
        mraa_gpio_close(dev->gpioD2);
    if (dev->gpioD3)
        mraa_gpio_close(dev->gpioD3);

    free(dev);
}

upm_result_t lcd1602_write(const lcd1602_context dev, char *buffer, int len)
{
	assert(dev != NULL);
	upm_result_t error = UPM_SUCCESS;

	int i;
	for(i = 0; i < len; ++i)
		error = lcd1602_data(dev, buffer[i]);
	return error;
}

upm_result_t lcd1602_set_cursor(const lcd1602_context dev, 
									unsigned int row, unsigned int column)
{
	assert(dev != NULL);

	column = column % dev->columns;
	uint8_t offset = column;

switch (dev->rows)
    {
    case 1:
        // Single row displays with more than 8 columns usually have their
        // DDRAM split in two halves. The first half starts at address 00.
        // The second half starts at address 40. E.g. 16x2 DDRAM mapping:
        // 00 01 02 03 04 05 06 07 40 41 42 43 44 45 46 47
        if (dev->columns > 8)
        {
            offset = (column % (dev->columns / 2)) +
                (column / (dev->columns / 2)) * 0x40;
        }
        break;

    case 2:
        // this should work for any display with two rows
        // DDRAM mapping:
        // 00 .. 27
        // 40 .. 67
        offset += row * 0x40;
        break;

    case 4:
        if (dev->columns == 16)
        {
            // 16x4 display
            // DDRAM mapping:
            // 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
            // 40 41 42 43 43 45 46 47 48 49 4A 4B 4C 4D 4E 4F
            // 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
            // 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F
            int row_addr[] = { 0x00, 0x40, 0x10, 0x50 };
            offset += row_addr[row];
        }
        else
        {
            // 20x4 display
            // DDRAM mapping:
            // 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
            // 40 41 42 43 43 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53
            // 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27
            // 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67
            int row_addr[] = { 0x00, 0x40, 0x14, 0x54 };
            offset += row_addr[row];
        }
        break;
    }

    return lcd1602_command(dev, HD44780_CMD | offset);
}

upm_result_t lcd1602_clear(const lcd1602_context dev)
{
	assert(dev != NULL);

    upm_result_t ret;
    ret = lcd1602_command(dev, HD44780_CLEARDISPLAY);
    upm_delay_us(2000); // this command takes awhile
    return ret;
}

upm_result_t lcd1602_home(const lcd1602_context dev)
{
	assert(dev != NULL);

    upm_result_t ret;
    ret = lcd1602_command(dev, HD44780_RETURNHOME);
    upm_delay_us(2000); // this command takes awhile
    return ret;
}

upm_result_t lcd1602_create_char(const lcd1602_context dev, 
									unsigned int slot,
									char *data)
{
	assert(dev != NULL);

    upm_result_t error = UPM_SUCCESS;

    slot &= 0x07; // only have 8 positions we can set

    error = lcd1602_command(dev, HD44780_SETCGRAMADDR | (slot << 3));

    if (error == UPM_SUCCESS)
    {
        int i;
        for (i = 0; i < 8; i++) {
            error = lcd1602_data(dev, data[i]);
        }
    }

    return error;
}

upm_result_t lcd1602_display_on(const lcd1602_context dev, bool on)
{
    assert(dev != NULL);

    if (on)
        dev->displayControl |= HD44780_DISPLAYON;
    else
        dev->displayControl &= ~HD44780_DISPLAYON;

    return lcd1602_command(dev, HD44780_DISPLAYCONTROL | dev->displayControl);
}

upm_result_t lcd1602_cursor_on(const lcd1602_context dev, bool on)
{
    assert(dev != NULL);

    if (on)
        dev->displayControl |= HD44780_CURSORON;
    else
        dev->displayControl &= ~HD44780_CURSORON;

    return lcd1602_command(dev, HD44780_DISPLAYCONTROL | dev->displayControl);
}

upm_result_t lcd1602_cursor_blink_on(const lcd1602_context dev, bool on)
{
    assert(dev != NULL);

    if (on)
        dev->displayControl |= HD44780_BLINKON;
    else
        dev->displayControl &= ~HD44780_BLINKON;

    return lcd1602_command(dev, HD44780_DISPLAYCONTROL | dev->displayControl);
}

upm_result_t lcd1602_scroll_display_left(const lcd1602_context dev)
{
    assert(dev != NULL);

    return lcd1602_command(dev, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE
                           | HD44780_MOVELEFT);
}

upm_result_t lcd1602_scroll_display_right(const lcd1602_context dev)
{
    assert(dev != NULL);

    return lcd1602_command(dev, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE
                           | HD44780_MOVERIGHT);
}

upm_result_t lcd1602_entry_left_to_right(const lcd1602_context dev, bool on)
{
    assert(dev != NULL);

    if (on)
        dev->entryDisplayMode |= HD44780_ENTRYLEFT;
    else
        dev->entryDisplayMode &= ~HD44780_ENTRYLEFT;

    return lcd1602_command(dev, HD44780_ENTRYMODESET | dev->entryDisplayMode);
}

upm_result_t lcd1602_autoscroll_on(const lcd1602_context dev, bool on)
{
    assert(dev != NULL);

    if (on)
        dev->entryDisplayMode |= HD44780_ENTRYSHIFTINCREMENT;
    else
        dev->entryDisplayMode &= ~HD44780_ENTRYSHIFTINCREMENT;

    return lcd1602_command(dev, HD44780_ENTRYMODESET | dev->entryDisplayMode);
}

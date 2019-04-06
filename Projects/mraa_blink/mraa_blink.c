#include "mraa/gpio.h"
#include "mraa/common.h"

int
main(int argc, char** argv)
{
    mraa_platform_t platform = mraa_get_platform_type();
    mraa_gpio_context gpio;
    const char* board_name = mraa_get_platform_name();
    int ledstate = 0;
    mraa_result_t r = MRAA_SUCCESS;
    // for LinkIt MT7688 use WiFi LED
    gpio = mraa_gpio_init(44);


    fprintf(stdout, "Welcome to libmraa\n Version: %s\n Running on %s (Ctrl+C to exit)\n", mraa_get_version(), board_name);


    if (gpio == NULL) {
        fprintf(stdout, "Could not initilaize gpio\n");
        return 1;
    }

    mraa_gpio_dir(gpio, MRAA_GPIO_OUT);

    while (1) {
        ledstate = !ledstate;
        mraa_gpio_write(gpio, !ledstate);
        sleep(1);
    }

    r = mraa_gpio_close(gpio);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    return r;
}

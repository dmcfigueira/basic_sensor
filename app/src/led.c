/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to interact with the board leds.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "led.h"

#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int led_init(void) {
    // Check if the led GPIO pin is ready
    if (!gpio_is_ready_dt(&led)) {
        return -1;
    }

    // Configure the led GPIO pin as an output
    int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        return ret;
    }

    return 0;
}

int led_toggle(void) { return gpio_pin_toggle_dt(&led); }
int led_on(void) { return gpio_pin_set_dt(&led, true); }
int led_off(void) { return gpio_pin_set_dt(&led, false); }
/**
 * Created on Wed Nov 20 2024
 *
 * @brief Application entry point.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */

#include "led.h"
#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int init_board(void) {
    // Initialize the board leds
    int ret = led_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize the board leds");
        return ret;
    }

    // Wait for a USB connection and initialize USB communications
    ret = usb_comm_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize the USB communications");
        return ret;
    }

    return 0;
}

int main(void) {
    // Initialize the board
    int ret = init_board();
    if (ret != 0) {
        LOG_ERR("Failed to initialize the board");
        return ret;
    }

    // Turn on the board led (to indicate the board is ready)
    led_on();

    // Start the main loop
    while (true) {
        uint8_t buffer[64] = {0};
        size_t n_bytes     = 0;

        // For now we read some data and echo it back 5 times
        usb_comm_read(buffer, 64, &n_bytes);

        if (n_bytes > 0) {
            usb_comm_write(buffer, n_bytes);
            usb_comm_write(buffer, n_bytes);
            usb_comm_write(buffer, n_bytes);
            usb_comm_write(buffer, n_bytes);
            usb_comm_write(buffer, n_bytes);
        }

        k_msleep(1000);
    }

    return 0;
}
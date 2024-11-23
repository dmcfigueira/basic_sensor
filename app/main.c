/**
 * Created on Wed Nov 20 2024
 *
 * @brief Application entry point.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */

#include "led.h"
#include "sim_sensor.h"
#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <math.h>
#include <stdio.h>

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

        // Check if any data was received over USB
        ret = usb_comm_read(buffer, 64, &n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to received data over USB");
            break;
        }

        // If no data was received, wait some time and try again
        if (n_bytes <= 0) {
            k_msleep(1000);
            continue;
        }

        // Start some pattern
        sim_sensor_start_pattern(PATTERN_CONST, 1, 5, 0);

        float sample = sim_sensor_read_sample();
        while (!isnan(sample)) {
            n_bytes = snprintf((char*) buffer, sizeof(buffer), "%.1f\n", sample);

            // Send the sample over USB (as text)
            ret = usb_comm_write(buffer, n_bytes);
            if (ret != 0) {
                LOG_ERR("Failed to send data over USB");
                break;
            }

            sample = sim_sensor_read_sample();
            k_msleep(500);
        }
    }

    // Turn off the board led (if an error took place)
    led_off();

    return ret;
}
/**
 * Created on Wed Nov 20 2024
 *
 * @brief Application entry point.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */

#include "command_parser.h"
#include "led.h"
#include "ring_buffer.h"
#include "sensor_thread.h"
#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <math.h>
#include <stdio.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define COMMAND_BUFFER_SIZE 64

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
    ring_buffer_t ring_buffer = {0};

    // Initialize the board
    int ret = init_board();
    if (ret != 0) {
        LOG_ERR("Failed to initialize the board");
        return ret;
    }

    // Start the sensor thread
    sensor_thread_start(&ring_buffer);

    // Turn on the board led (to indicate the board is ready)
    led_on();

    LOG_INF("Waiting for commands...");

    // Start the main loop
    while (true) {
        uint8_t buffer[COMMAND_BUFFER_SIZE + 1] = {0};
        command_t command                       = {0};
        size_t n_bytes                          = 0;

        // Check if any data was received over USB
        ret = usb_comm_read(buffer, COMMAND_BUFFER_SIZE, &n_bytes);
        if (ret != 0) {
            break;
        }

        // If no data was received, wait some time and try again
        if (n_bytes <= 0) {
            k_msleep(1000);
            continue;
        }

        // Parse the command received
        ret = command_parse(buffer, &command);
        if (ret != 0) {
            continue;
        }

        // Execute the command
        ret = command_execute(&command);
        if (ret != 0) {
            continue;
        }
    }

    // Turn off the board led (if an error took place)
    led_off();

    return ret;
}
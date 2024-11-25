/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to interact with the sensor thread.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "sensor_thread.h"

#include "sim_sensor.h"
#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <math.h>
#include <stdio.h>

LOG_MODULE_REGISTER(sensor_thread, LOG_LEVEL_INF);

#define DEFAULT_READ_RATE 1   // Hz

/* Static variables */
static uint16_t read_period  = 1000 / DEFAULT_READ_RATE;      // ms
static uint32_t read_wait_us = 1000000 / DEFAULT_READ_RATE;   // us

void sensor_thread_set_read_rate(uint16_t read_rate) {
    read_period  = (read_rate < 1000) ? 1000 / read_rate : 1;
    read_wait_us = (read_period > 1) ? read_period * 1000 : 100;
    LOG_INF("Read rate set to %d Hz (new read period: %d ms)", read_rate, read_period);
}

int sensor_thread_read_sensor_data(void) {
    uint8_t buffer[64]        = {0};
    size_t n_bytes            = 0;
    uint64_t next_sample_time = k_uptime_get();

    while (true) {
        // Read the next sample
        float sample = sim_sensor_read_sample();

        // Check if the sample is valid
        if (isnan(sample)) {
            break;
        }

        n_bytes = snprintf((char*) buffer, sizeof(buffer), "%.1f\n", sample);

        // Send the sample over USB (as text)
        int ret = usb_comm_write(buffer, n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to send data over USB");
            return ret;
        }

        // Wait for the next sample
        next_sample_time += read_period;
        while (k_uptime_get() < next_sample_time) {
            k_usleep(read_wait_us);
        }
    }

    return 0;
}
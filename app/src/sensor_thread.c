/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to interact with the sensor thread.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "sensor_thread.h"

#include "ring_buffer.h"
#include "sim_sensor.h"
#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <math.h>
#include <stdio.h>

LOG_MODULE_REGISTER(sensor_thread, LOG_LEVEL_INF);

/* Constants */
#define SENSOR_THREAD_STACK_SIZE 1000
#define SENSOR_THREAD_PRIO       5
#define DEFAULT_READ_RATE        1   // Hz

/* Static variables */
K_THREAD_STACK_DEFINE(sensor_thread_stack, SENSOR_THREAD_STACK_SIZE);
static struct k_thread sensor_thread = {0};

static uint16_t read_period  = 1000 / DEFAULT_READ_RATE;      // ms
static uint32_t read_wait_us = 1000000 / DEFAULT_READ_RATE;   // us

void sensor_thread_set_read_rate(uint16_t read_rate) {
    read_period  = (read_rate < 1000) ? 1000 / read_rate : 1;
    read_wait_us = (read_period > 1) ? read_period * 1000 : 100;
    LOG_INF("Read rate set to %d Hz (new read period: %d ms)", read_rate, read_period);
}

static void sensor_thread_loop(ring_buffer_t* ring_buffer) {
    uint8_t buffer[RING_BUFFER_ITEM_SIZE] = {0};
    uint8_t n_bytes                       = 0;
    uint64_t next_sample_time             = k_uptime_get();

    while (true) {
        // Wait for the next sample
        while (k_uptime_get() < next_sample_time) {
            k_usleep(read_wait_us);
        }
        next_sample_time += read_period;

        // Read the next sample
        float sample = sim_sensor_read_sample();

        // Check if the sample is valid
        if (isnan(sample)) {
            continue;
        }

        n_bytes = snprintf((char*) buffer, sizeof(buffer), "%.1f\n", sample);

        // Store the sample in the ring buffer
        int ret = ring_buffer_add(ring_buffer, buffer, n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to store sample in ring buffer");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));

        // Get the sample from the ring buffer
        ret = ring_buffer_get(ring_buffer, buffer, &n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to get sample from ring buffer");
            continue;
        }

        // Send the sample over USB (as text)
        ret = usb_comm_write(buffer, n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to send data over USB");
            continue;
        }
    }
}

void sensor_thread_start(ring_buffer_t* ring_buffer) {
    k_thread_create(&sensor_thread, sensor_thread_stack, SENSOR_THREAD_STACK_SIZE, (k_thread_entry_t) sensor_thread_loop, ring_buffer, NULL, NULL,
        SENSOR_THREAD_PRIO, 0, K_NO_WAIT);
}
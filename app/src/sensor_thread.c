/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to interact with the sensor thread.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "sensor_thread.h"

#include "sim_sensor.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <errno.h>
#include <math.h>

LOG_MODULE_REGISTER(sensor_thread, LOG_LEVEL_INF);

/* Constants */
#define SENSOR_THREAD_STACK_SIZE 1000
#define SENSOR_THREAD_PRIO       5
#define DEFAULT_READ_RATE        1   // Hz

/* Static variables */
K_THREAD_STACK_DEFINE(sensor_thread_stack, SENSOR_THREAD_STACK_SIZE);
static struct k_thread sensor_thread = {0};

static uint16_t read_period      = 1000 / DEFAULT_READ_RATE;                 // ms
static uint32_t read_wait_us     = (1000 / DEFAULT_READ_RATE) * 1000 / 10;   // us
static uint64_t next_sample_time = 0;

void sensor_thread_set_read_rate(uint16_t read_rate) {
    read_period      = (read_rate < 1000) ? 1000 / read_rate : 1;
    read_wait_us     = read_period * 1000 / 10;   // 1/10th of the period
    next_sample_time = k_uptime_get() + read_period;
    LOG_INF("Read rate set to %d Hz (new read period: %d ms)", read_rate, read_period);
}

// Actively check if a new sample is available - this is what is
// usually done (either through interrupts or by reading the sensor
// status) if we're trying to keep up with the sensor data rate.
static void sensor_thread_wait_for_new_data(void) {
    while (!sim_sensor_new_sample_ready()) {
        k_usleep(read_wait_us);
    }
}

// Alternatively, we can try to force a fixed data rate - just to
// demonstrate that reading the sensor at a different read rate than
// what data is produced will lead to duplicates or missed samples)
// (just like a normal sensor would)
static void sensor_thread_wait_fixed_rate(void) {
    while (k_uptime_get() < next_sample_time) {
        k_usleep(read_wait_us);
    }
    next_sample_time += read_period;
}

static bool sensor_thread_force_fixed_rate(void) {
    static bool force_fixed_rate = true;
    bool new_state               = (read_period != sim_sensor_get_sample_period());

    // When changing to fixed rate adjust the next sample time
    if (new_state != force_fixed_rate && new_state == true) {
        next_sample_time = k_uptime_get() + read_period;
    }

    // Update the current state
    force_fixed_rate = new_state;

    return force_fixed_rate;
}

static void sensor_thread_loop(ring_buffer_t* ring_buffer) {
    while (true) {
        // Wait for the next sample
        if (sensor_thread_force_fixed_rate()) {
            sensor_thread_wait_fixed_rate();
        } else {
            sensor_thread_wait_for_new_data();
        }

        // Read the next sample
        float sample = sim_sensor_read_sample();

        // Check if the sample is valid
        if (isnan(sample)) {
            continue;
        }

        // Store the sample in the ring buffer
        int ret = ring_buffer_add(ring_buffer, &sample, sizeof(sample));
        if (ret != 0) {
            LOG_ERR("Failed to store sample in the ring buffer (err: %d - %s)", ret, strerror(-ret));
            continue;
        }

        LOG_DBG("Stored: %.1f", sample);
    }
}

void sensor_thread_start(ring_buffer_t* ring_buffer) {
    k_thread_create(&sensor_thread, sensor_thread_stack, SENSOR_THREAD_STACK_SIZE, (k_thread_entry_t) sensor_thread_loop, ring_buffer, NULL, NULL,
        SENSOR_THREAD_PRIO, 0, K_NO_WAIT);
}
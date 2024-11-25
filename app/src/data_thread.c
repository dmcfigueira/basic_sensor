/**
 * Created on Mon Nov 25 2024
 *
 * @brief Provides methods to interact with the data thread, whose
 *        responsibility is to send data over USB at a fixed rate.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "data_thread.h"

#include "usb_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <errno.h>
#include <stdio.h>

LOG_MODULE_REGISTER(data_thread, LOG_LEVEL_INF);

/* Constants */
#define DATA_THREAD_STACK_SIZE 1000
#define DATA_THREAD_PRIO       5
#define DEFAULT_SEND_RATE      1   // Hz

/* Static variables */
K_THREAD_STACK_DEFINE(data_thread_stack, DATA_THREAD_STACK_SIZE);
static struct k_thread data_thread = {0};

static uint16_t send_period      = 1000 / DEFAULT_SEND_RATE;                 // ms
static uint32_t send_wait_us     = (1000 / DEFAULT_SEND_RATE) * 1000 / 10;   // us
static uint64_t next_sample_time = 0;

void data_thread_set_send_rate(uint16_t send_rate) {
    send_period      = (send_rate < 1000) ? 1000 / send_rate : 1;
    send_wait_us     = send_period * 1000 / 10;   // 1/10th of the period
    next_sample_time = k_uptime_get() + send_period;
    LOG_INF("Send rate set to %d Hz (new send period: %d ms)", send_rate, send_period);
}

static void data_thread_wait_fixed_rate(void) {
    while (k_uptime_get() < next_sample_time) {
        k_usleep(send_wait_us);
    }
    next_sample_time += send_period;
}

static void data_thread_loop(ring_buffer_t* ring_buffer) {
    uint8_t buffer[20] = {0};
    uint8_t n_bytes    = 0;
    float sample       = 0.0;

    next_sample_time = k_uptime_get() + send_period;

    while (true) {
        // Wait for the next sample
        data_thread_wait_fixed_rate();

        // Get the next sample from the ring buffer
        int ret = ring_buffer_get(ring_buffer, &sample, &n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to get sample from the ring buffer (err: %d - %s)", ret, strerror(-ret));
            continue;
        }

        // Check if a sample was retrieved
        if (n_bytes != sizeof(sample)) {
            continue;
        }

        // Convert the sample to text
        n_bytes = snprintf((char*) buffer, sizeof(buffer), "%.1f\n", sample);

        // Send the sample over USB
        ret = usb_comm_write(buffer, n_bytes);
        if (ret != 0) {
            LOG_ERR("Failed to send data over USB");
            continue;
        }

        LOG_DBG("Sent: %.1f", sample);
    }
}

void data_thread_start(ring_buffer_t* ring_buffer) {
    k_thread_create(&data_thread, data_thread_stack, DATA_THREAD_STACK_SIZE, (k_thread_entry_t) data_thread_loop, ring_buffer, NULL, NULL,
        DATA_THREAD_PRIO, 0, K_NO_WAIT);
}
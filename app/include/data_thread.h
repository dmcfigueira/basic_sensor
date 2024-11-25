/**
 * Created on Mon Nov 25 2024
 *
 * @brief Provides methods to interact with the data thread, whose
 *        responsibility is to send data over USB at a fixed rate.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include "ring_buffer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Set the data rate at which sensor data will be sent over USB.
 *
 * @param read_rate The send rate in Hz (max: 1000Hz).
 */
void data_thread_set_send_rate(uint16_t send_rate);

/**
 * @brief Start the data thread.
 *
 * @param ring_buffer The ring buffer used to store the sensor data.
 */
void data_thread_start(ring_buffer_t* ring_buffer);
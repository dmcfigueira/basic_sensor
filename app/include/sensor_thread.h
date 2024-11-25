/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to interact with the sensor thread, whose
 *        responsibility is to read sensor data at fixed data rate.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Set the data rate at which sensor data will be read
 *
 * @param read_rate The read rate in Hz (max: 1000Hz).
 */
void sensor_thread_set_read_rate(uint16_t read_rate);

// TODO: change this
int sensor_thread_read_sensor_data(void);
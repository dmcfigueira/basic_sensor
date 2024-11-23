/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to simulate sensor data.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    // The sensor will repeatedly return a the same value until
    // a certain number of samples is reached.
    // arg1: constant value
    // arg2: number of samples
    PATTERN_CONST = 0,

    // The sensor will repeatedly return a value that is increased
    // by a constant amount until a maximum value is reached.
    // arg1: initial value
    // arg2: increment amount
    // arg3: maximum value (inclusive)
    PATTERN_INCREASING = 1,

    // The sensor will repeatedly return a value that is decreased
    // by a constant amount until a minimum value is reached.
    // arg1: initial value
    // arg2: decrement amount
    // arg3: minimum value (inclusive)
    PATTERN_DECREASING = 2,

    // The sensor will repeatedly return a random value within a
    // given range until a certain number of samples is reached.
    // arg1: minimum value
    // arg2: maximum value
    // arg3: number of samples
    PATTERN_RANDOM = 3,
} sim_sensor_pattern_t;

/**
 * @brief Set the data rate at which simulated data will be produced.
 *
 * @param data_rate The data rate in Hz (max: 1000Hz).
 */
void sim_sensor_set_data_rate(uint16_t data_rate);

/**
 * @brief Start the simulation of a given data pattern. Once the pattern
 *        is started, data will be produced at the data rate set until a
 *        certain stop condition (pattern-specific) is reached, at which
 *        point no more data will be produced.
 *
 * @param pattern The pattern to be simulated.
 * @param arg1 The first argument for the pattern.
 * @param arg2 The second argument for the pattern.
 * @param arg3 The third argument for the pattern.
 *
 * @note There are several ways I could have passed the arguments of the
 *       pattern here (e.g. a union struct, void pointers, etc). I chose
 *       to use individual floats as arguments because it seemed like to
 *       be the simplest way to send the arguments over USB and directly
 *       pass them into this function.
 */
void sim_sensor_start_pattern(sim_sensor_pattern_t pattern, float arg1, float arg2, float arg3);

/**
 * @brief Retrieve a simulated sensor sample. Returns NaN if no simulation
 *        is currently ongoing. It will be up to the user to make sure this
 *        function is called exactly once per sample period. Calling this
 *        function more than once per sample period will result in duplicated
 *        data (just like a real sensor would).
 *
 * @return The simulated sensor sample (or NaN).
 */
float sim_sensor_read_sample(void);
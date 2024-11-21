/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to interact with the board leds.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initializes the board leds.
 *
 * @return 0 on success, negative errno code on fail.
 */
int led_init(void);

/**
 * @brief Toggles the state of the board first led (led0).
 *
 * @return 0 on success, negative errno on failure.
 */
int led_toggle(void);

/**
 * @brief Turns on the board first led (led0).
 *
 * @return 0 on success, negative errno on failure.
 */
int led_on(void);

/**
 * @brief Turns off the board first led (led0).
 *
 * @return 0 on success, negative errno on failure.
 */
int led_off(void);

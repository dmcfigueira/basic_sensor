/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to send and receive data over USB.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initializes the USB communication.
 *
 * @return 0 on success, negative errno code on fail.
 *
 * @note This function will block until a USB connection is established.
 */
int usb_comm_init(void);

/**
 * @brief Read data received over USB. This will read as many bytes as there
 *        are available, up to the size of the buffer. If no data is available,
 *        this function returns immediately.
 *
 * @param buffer Buffer to store the data read (output).
 * @param size The size of the buffer (or the max number of bytes you want to read).
 * @param n_bytes The number of bytes read (output).
 * @return 0 on success, negative errno on failure.
 */
int usb_comm_read(uint8_t* buffer, size_t buffer_len, size_t* n_bytes);

/**
 * @brief Write data over USB.
 *
 * @param buffer Buffer containing the data to be written.
 * @param n_bytes The number of bytes to be sent.
 * @return 0 on success, negative errno on failure.
 */
int usb_comm_write(uint8_t* buffer, size_t n_bytes);
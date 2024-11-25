/**
 * Created on Mon Nov 25 2024
 *
 * @brief Simple ring buffer implementation, which keeps a static number of items in a FIFO order.
 *        Once full, the oldest items are discarded.
 *
 * @note Zephyr RTOS provides it's own ring buffer implementation (zephyr/sys/ring_buffer.h),
 *       so I'm only adding this one for demonstration purposes.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Constants */
#define RING_BUFFER_MAX_ITEMS 10   // max: 65535
#define RING_BUFFER_ITEM_SIZE 10   // max: 255 bytes

/* Type definitions */

// Forward declaration the struct only
// (keeping the implementation details hidden)
typedef struct _ring_buffer_t ring_buffer_t;

/**
 * @brief Add a new item to the ring buffer. This will copy the item to the buffer,
 *        overwriting the oldest item if the buffer is full.
 *
 * @param buffer The ring buffer to add the item to.
 * @param item The item to add.
 * @param item_size The size of the item do add.
 * @return 0 on success, negative errno on failure.
 */
int ring_buffer_add(ring_buffer_t* buffer, void* item, uint8_t item_size);

/**
 * @brief Retrieve the oldest item from the ring buffer (and remove it).
 *
 * @param buffer The ring buffer to get the item from.
 * @param item The item retrieved (output).
 * @param item_size The size of the item retrieved (output).
 * @return 0 on success, negative errno on failure.
 */
int ring_buffer_get(ring_buffer_t* buffer, void* item, uint8_t* item_size);
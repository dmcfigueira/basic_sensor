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
#include "ring_buffer.h"

#include <zephyr/logging/log.h>

#include <errno.h>

LOG_MODULE_REGISTER(ring_buffer, LOG_LEVEL_INF);

/*
 * Note that no synchronization mechanisms are needed to manage access to the
 * ring buffer, even though we'll have multiple threads reading/writing to it.
 *
 * This is because while our application has multiple threads, these will be
 * run on a single core and therefore only one thread will be actually run
 * at any given time (see multi-threading vs parallelism).
 *
 * As such, it will be fine as long as we don't give-up the CPU to another thread
 * (by calling some OS function like 'k_sleep' or 'k_yield') mid-way through
 * accessing or modifying items from the ring buffer.
 */

int ring_buffer_add(ring_buffer_t* buffer, void* item, uint8_t item_size) {
    if (buffer == NULL || item == NULL) {
        return -EINVAL;
    }

    // Check if the item size is valid
    if (item_size > RING_BUFFER_ITEM_SIZE) {
        return -ENOMEM;
    }

    // If the buffer is full
    if (buffer->n_items >= RING_BUFFER_MAX_ITEMS) {
        // Advance the start index (overwriting the oldest item)
        LOG_WRN("Ring buffer is full. Item %d discarded.", buffer->start_idx);
        buffer->start_idx = (buffer->start_idx + 1) % RING_BUFFER_MAX_ITEMS;
    } else {
        // Otherwise increase the number of items
        buffer->n_items++;
    }

    // Then copy the item to the buffer
    memcpy(buffer->items[buffer->end_idx], item, item_size);
    buffer->sizes[buffer->end_idx] = item_size;

    // And update the end index
    buffer->end_idx = (buffer->end_idx + 1) % RING_BUFFER_MAX_ITEMS;

    return 0;
}

int ring_buffer_get(ring_buffer_t* buffer, void* item, uint8_t* item_size) {
    if (buffer == NULL || item == NULL || item_size == NULL) {
        return -EINVAL;
    }

    memset(item, 0, RING_BUFFER_ITEM_SIZE);
    *item_size = 0;

    // Check if the buffer is empty
    if (buffer->n_items == 0) {
        return 0;
    }

    // Copy the oldest item
    memcpy(item, buffer->items[buffer->start_idx], buffer->sizes[buffer->start_idx]);
    *item_size = buffer->sizes[buffer->start_idx];

    // Update the start index
    buffer->start_idx = (buffer->start_idx + 1) % RING_BUFFER_MAX_ITEMS;

    // Update the number of items
    buffer->n_items--;

    return 0;
}
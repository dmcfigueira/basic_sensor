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

#include <errno.h>

/* Type definitions */
struct _ring_buffer_t {
    uint8_t items[RING_BUFFER_MAX_ITEMS][RING_BUFFER_ITEM_SIZE];
    uint8_t sizes[RING_BUFFER_MAX_ITEMS];
    uint16_t start_idx;
    uint16_t end_idx;
    uint16_t n_items;
};

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

    // Check if the buffer is empty
    if (buffer->n_items == 0) {
        return 0;
    }

    // Update the end index
    memcpy(buffer->items[buffer->end_idx], item, item_size);
    buffer->end_idx = (buffer->end_idx > 0 ? buffer->end_idx : RING_BUFFER_MAX_ITEMS) - 1;

    // Copy the oldest item
    memcpy(item, buffer->items[buffer->end_idx], buffer->sizes[buffer->end_idx]);
    *item_size = buffer->sizes[buffer->end_idx];

    // Update the number of items
    buffer->n_items--;

    return 0;
}
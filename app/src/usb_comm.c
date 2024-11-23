/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to send and receive data over USB.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "usb_comm.h"

#include "led.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>

#include <errno.h>

LOG_MODULE_REGISTER(usb_comm, LOG_LEVEL_INF);

static const struct device* const uart_dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);

int usb_comm_init(void) {
    uint32_t baudrate = 0;
    uint32_t dtr      = 0;

    // Check if the UART device is ready
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART device not ready");
        return -1;
    }

    // Enable the USB susbystem
    int ret = usb_enable(NULL);
    if (ret != 0) {
        LOG_ERR("Failed to enable USB susbystem (err: %d - %s)", ret, strerror(-ret));
        return ret;
    }

    LOG_INF("Waiting for a USB connection...");

    // Wait for a USB connection to be present (blocking)
    while (dtr == 0) {
        uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_DTR, &dtr);

        // Here I'm adding some LED logic to help indicate the application state.
        // On a real application it makes no sense to couple the LED logic and the
        // USB logic together - but the LED is just for debug purposes and having it
        // here saves me from having to create a separate thread just to blink the led.
        led_toggle();

        k_msleep(500);
    }

    // Configure some optional settings
    ret = uart_line_ctrl_set(uart_dev, UART_LINE_CTRL_DCD, 1);
    if (ret != 0) {
        LOG_WRN("Failed to set USB DCD (err: %d - %s)", ret, strerror(-ret));
    }

    ret = uart_line_ctrl_set(uart_dev, UART_LINE_CTRL_DSR, 1);
    if (ret != 0) {
        LOG_WRN("Failed to set USB DSR (err: %d - %s)", ret, strerror(-ret));
    }

    // Wait for the host to adjust its settings
    k_msleep(100);

    // Get the current baudrate
    ret = uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
    if (ret != 0) {
        LOG_WRN("Failed to get USB baudrate (err: %d - %s)", ret, strerror(-ret));
    } else {
        LOG_DBG("USB baudrate: %d", baudrate);
    }

    LOG_INF("USB communications initialized.");

    return 0;
}

int usb_comm_read(uint8_t* buffer, size_t buffer_len, size_t* n_bytes) {
    int ret = 0;

    memset(buffer, 0, buffer_len);
    *n_bytes = 0;

    // While there is data present
    for (int i = 0; ret == 0; i++) {
        // Check if the buffer is full
        if (i >= buffer_len) {
            LOG_ERR("Failed to read USB data: buffer full");
            return -ENOBUFS;
        }

        // Read the next byte from the UART
        ret = uart_poll_in(uart_dev, &buffer[i]);
        if (ret == -1) {
            return 0;   // No data
        } else if (ret < 0) {
            LOG_ERR("Failed to read USB data (err: %d - %s)", ret, strerror(-ret));
            return ret;
        }

        (*n_bytes)++;
    }

    return 0;
}

int usb_comm_write(uint8_t* buffer, size_t n_bytes) {
    // Send bytes one by one
    for (int i = 0; i < n_bytes; i++) {
        uart_poll_out(uart_dev, buffer[i]);
    }
    return 0;
}
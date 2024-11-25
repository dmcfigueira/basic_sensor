/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to parse and execute commands received over USB.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_COMMAND_ARGS 4

/* Type definitions */
typedef enum {
    COMMAND_SET_DATA_RATE = 0,
    COMMAND_SET_READ_RATE = 1,
    COMMAND_SET_SEND_RATE = 2,
    COMMAND_START_PATTERN = 3,
    COMMAND_MAX_VALUE,
} command_type_t;

typedef struct {
    command_type_t type;
    float args[MAX_COMMAND_ARGS];
} command_t;

/**
 * @brief Parse a command received over USB.
 *
 * @param data A null-terminated string to be parsed.
 * @param command The command parsed (output).
 * @return 0 on success, negative errno on failure.
 */
int command_parse(char* data, command_t* command);

/**
 * @brief Execute a command.
 *
 * @param command The command to be executed.
 * @return 0 on success, negative errno on failure.
 */
int command_execute(command_t* command);
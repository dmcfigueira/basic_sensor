/**
 * Created on Sat Nov 23 2024
 *
 * @brief Provides methods to parse and execute commands received over USB.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "command_parser.h"

#include "sensor_thread.h"
#include "sim_sensor.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(command_parser, LOG_LEVEL_INF);

int command_parse(char* data, command_t* command) {
    LOG_DBG("Parsing command: %s", data);

    memset(command, 0, sizeof(command_t));

    // Parse the command type
    int ret = sscanf(data, "%d", (int*) &command->type);
    if (ret < 1) {
        LOG_ERR("Failed to parse the command.");
        return -EIO;
    }

    // Check if the command type is valid
    if (command->type < 0 || command->type >= COMMAND_MAX_VALUE) {
        LOG_ERR("Invalid command type: %d", command->type);
        return -EIO;
    }

    // Parse the command arguments
    data = strchr(data, ' ');
    for (int i = 0; data != NULL && i < MAX_COMMAND_ARGS; i++) {
        command->args[i] = strtof(data, &data);
    }

    LOG_DBG("Parsed command: type=%d, args=[%.1f, %.1f, %.1f, %.1f]", command->type, command->args[0], command->args[1], command->args[2],
        command->args[3]);

    return 0;
}

int command_execute(command_t* command) {
    switch (command->type) {
        case COMMAND_SET_DATA_RATE: sim_sensor_set_data_rate(command->args[0]); break;
        case COMMAND_SET_READ_RATE: sensor_thread_set_read_rate(command->args[0]); break;
        case COMMAND_SET_SEND_RATE: LOG_INF("Command not implemented yet."); break;   // TODO
        case COMMAND_START_PATTERN:
            sim_sensor_start_pattern((sim_sensor_pattern_t) command->args[0], command->args[1], command->args[2], command->args[3]);
            break;
        default: LOG_ERR("Invalid command type: %d", command->type); return -EINVAL;
    }
    return 0;
}
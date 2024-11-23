/**
 * Created on Thu Nov 21 2024
 *
 * @brief Provides methods to simulate sensor data.
 *
 * @author Daniel Figueira <daniel.castro.figueira@gmail.com>
 */
#include "sim_sensor.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <math.h>
#include <memory.h>

LOG_MODULE_REGISTER(sim_sensor, LOG_LEVEL_INF);

/* Type definitions */
typedef struct {
    int64_t last_sample_start_time;   // ms
    uint32_t sample_index;
    float arg1;
    float arg2;
    float arg3;
} simulation_ctx_t;

typedef float (*sim_sensor_pattern_fn)(simulation_ctx_t* ctx);

/* Static variables */
static uint16_t sample_period           = 1000;   // ms (1 sample per second by default)
static sim_sensor_pattern_fn pattern_fn = {0};
static simulation_ctx_t sim_ctx         = {0};

/* Pattern simulation functions */
static float sim_sensor_pattern_const(simulation_ctx_t* ctx) {
    int value     = ctx->arg1;
    int n_samples = ctx->arg2;
    return (ctx->sample_index < n_samples) ? value : NAN;
}

static float sim_sensor_pattern_increasing(simulation_ctx_t* ctx) {
    float start_value = ctx->arg1;
    float increment   = ctx->arg2;
    float max_value   = ctx->arg3;
    float value       = start_value + increment * ctx->sample_index;
    return (value <= max_value) ? value : NAN;
}

static float sim_sensor_pattern_decreasing(simulation_ctx_t* ctx) {
    float start_value = ctx->arg1;
    float decrement   = ctx->arg2;
    float min_value   = ctx->arg3;
    float value       = start_value - decrement * ctx->sample_index;
    return (value >= min_value) ? value : NAN;
}

/* Other functions */
void sim_sensor_set_data_rate(uint16_t data_rate) {
    sample_period = 1000 / data_rate;
    LOG_INF("Data rate set to %d Hz (new sample period: %d ms)", data_rate, sample_period);
};

void sim_sensor_start_pattern(sim_sensor_pattern_t pattern, float arg1, float arg2, float arg3) {
    // Clear the current simulation context
    memset(&sim_ctx, 0, sizeof(sim_ctx));

    // Set the new pattern function to be used
    switch (pattern) {
        case PATTERN_CONST: pattern_fn = sim_sensor_pattern_const; break;
        case PATTERN_INCREASING: pattern_fn = sim_sensor_pattern_increasing; break;
        case PATTERN_DECREASING: pattern_fn = sim_sensor_pattern_decreasing; break;
        default: pattern_fn = NULL; break;
    }

    // Set the new simulation context
    sim_ctx.last_sample_start_time = k_uptime_get();
    sim_ctx.arg1                   = arg1;
    sim_ctx.arg2                   = arg2;
    sim_ctx.arg3                   = arg3;

    LOG_INF("Simulation with pattern %d started (args: %.1f, %.1f, %.1f", pattern, arg1, arg2, arg3);
}

static uint32_t sim_sensor_compute_samples_elapsed(simulation_ctx_t* ctx) {
    int64_t delta_ms         = k_uptime_get() - ctx->last_sample_start_time;
    uint32_t samples_elapsed = (delta_ms > 0) ? delta_ms / sample_period : 0;
    LOG_DBG("Time elapsed: %d ms (%d samples) ", (int) delta_ms, samples_elapsed);
    return samples_elapsed;
}

float sim_sensor_read_sample(void) {
    // Check if there is a pattern ongoing
    if (pattern_fn == NULL) {
        return NAN;
    }

    // Update the simulation context based on the time elapsed
    uint32_t samples_elapsed = sim_sensor_compute_samples_elapsed(&sim_ctx);
    sim_ctx.last_sample_start_time += samples_elapsed * sample_period;
    sim_ctx.sample_index += samples_elapsed;

    // Generate the next sample
    float sample = pattern_fn(&sim_ctx);
    LOG_DBG("[%d]: %.1f", sim_ctx.sample_index, sample);

    // Stop the pattern if NaN was returned
    if (isnan(sample)) {
        pattern_fn = NULL;
        LOG_INF("Simulation ended.");
    }

    return sample;
};
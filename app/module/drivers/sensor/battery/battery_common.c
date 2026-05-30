/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <zephyr/drivers/sensor.h>

#include "battery_common.h"

int battery_channel_get(const struct battery_value *value, enum sensor_channel chan,
                        struct sensor_value *val_out) {
    switch (chan) {
    case SENSOR_CHAN_GAUGE_VOLTAGE:
        val_out->val1 = value->millivolts / 1000;
        val_out->val2 = (value->millivolts % 1000) * 1000U;
        break;

    case SENSOR_CHAN_GAUGE_STATE_OF_CHARGE:
        val_out->val1 = value->state_of_charge;
        val_out->val2 = 0;
        break;

    default:
        return -ENOTSUP;
    }

    return 0;
}

uint8_t lithium_ion_mv_to_pct(int16_t bat_mv) {
    // Simple linear approximation of a battery based off adafruit's discharge graph:
    // https://learn.adafruit.com/li-ion-and-lipoly-batteries/voltages

    if (bat_mv >= 4200) {
        return 100;
    } else if (bat_mv <= 3450) {
        return 0;
    }

    return bat_mv * 2 / 15 - 459;
}

struct battery_lookup_point {
    int16_t millivolts;
    int16_t percent;
};

static uint8_t lookup_mv_to_pct(int16_t bat_mv, const struct battery_lookup_point *table,
                                size_t table_len) {
    if (bat_mv > table[0].millivolts) {
        return table[0].percent;
    }

    for (size_t i = 1; i < table_len; i++) {
        struct battery_lookup_point one = table[i - 1];
        struct battery_lookup_point two = table[i];
        if (bat_mv >= two.millivolts) {
            const int t = bat_mv - one.millivolts;
            const int dx = two.millivolts - one.millivolts;
            const int dy = two.percent - one.percent;
            return one.percent + dy * t / dx;
        }
    }

    return table[table_len - 1].percent;
}

uint8_t alkaline_mv_to_pct(int16_t bat_mv) {
    // Discharge data from
    // https://lygte-info.dk/info/BatteriesLowCurrentDischarge2%20UK.html
    // millivolts values are for a 2-cell series stack.
    static const struct battery_lookup_point alkaline_lookup[] = {
        {.millivolts = (1600 * 2), .percent = 100}, {.millivolts = (1500 * 2), .percent = 94},
        {.millivolts = (1450 * 2), .percent = 88},  {.millivolts = (1300 * 2), .percent = 40},
        {.millivolts = (1260 * 2), .percent = 28},  {.millivolts = (1200 * 2), .percent = 16},
        {.millivolts = (1140 * 2), .percent = 10},  {.millivolts = (900 * 2), .percent = 0},
    };

    return lookup_mv_to_pct(bat_mv, alkaline_lookup, ARRAY_SIZE(alkaline_lookup));
}

uint8_t cr2032_mv_to_pct(int16_t bat_mv) {
    static const struct battery_lookup_point cr2032_lookup[] = {
        {.millivolts = 2900, .percent = 100}, {.millivolts = 2700, .percent = 30},
        {.millivolts = 2600, .percent = 20},  {.millivolts = 2500, .percent = 10},
        {.millivolts = 2200, .percent = 5},   {.millivolts = 2000, .percent = 0},
    };

    return lookup_mv_to_pct(bat_mv, cr2032_lookup, ARRAY_SIZE(cr2032_lookup));
}
#pragma once
#include <stdint.h>

void sensor_init(void);
float read_battery_voltage(void);
float read_soil_moisture(void);


uint8_t  batt_percent_from_v(float v); // 0–100
uint8_t  soil_percent_from_raw(int raw);// 0–100
int sensor_read_soil_raw_avg(void);

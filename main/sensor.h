#pragma once
#include <stdint.h>

void sensor_init(void);
float read_battery_voltage(void);
float read_soil_moisture(void);

int      read_soil_raw(void);          // opzionale, se vuoi il valore ADC
uint8_t  batt_percent_from_v(float v); // 0–100
uint8_t  soil_percent_from_raw(int raw);// 0–100
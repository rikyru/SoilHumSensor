#pragma once
#include <stdbool.h>
#include <stdint.h>

#define CONFIG_NAMESPACE "soilcfg"

typedef struct {
    char wifi_ssid[64];
    char wifi_pass[64];
    char mqtt_host[64];
    int mqtt_port;
    char mqtt_user[32];
    char mqtt_pass[32];
    int sleep_minutes;
    float batt_v_min;      // V
    float batt_v_max;      // V
    uint16_t soil_wet_raw; // ADC "bagnato"
    uint16_t soil_dry_raw; // ADC "asciutto"
} config_data_t;



// default sensati 
#define DEFAULT_BATT_V_MIN 3.80f
#define DEFAULT_BATT_V_MAX 4.90f
#define DEFAULT_SOIL_WET_RAW 1200
#define DEFAULT_SOIL_DRY_RAW 3200

void config_load(void);
bool config_is_valid(void);
void config_save(const config_data_t *data);

// comode setter (salvano subito)
bool config_set_batt_range(float vmin, float vmax);
bool config_set_soil_wet_raw(uint16_t raw);
bool config_set_soil_dry_raw(uint16_t raw);

config_data_t config_get(void);
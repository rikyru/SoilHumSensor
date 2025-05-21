#pragma once
#include <stdbool.h>

#define CONFIG_NAMESPACE "soilcfg"

typedef struct {
    char wifi_ssid[64];
    char wifi_pass[64];
    char mqtt_host[64];
    int mqtt_port;
    char mqtt_user[32];
    char mqtt_pass[32];
    int sleep_minutes;
} config_data_t;

void config_load(void);
bool config_is_valid(void);
void config_save(const config_data_t *data);
config_data_t config_get(void);
#include "config.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>

static config_data_t config;
static bool loaded = false;

// === NEW: default sensati ===
static void config_apply_defaults(config_data_t *c) 
{
    memset(c, 0, sizeof(*c));
    // metti qui eventuali default esistenti che già usavi (SSID, host, ecc) se vuoi
    c->mqtt_port = 1883;
    c->sleep_minutes = 5;

    // batteria tipica Li-Ion
    c->batt_v_min = 3.20f;
    c->batt_v_max = 4.20f;
    // ADC “tipici”: adatta ai tuoi (solo valori iniziali)
    c->soil_wet_raw = 1200;
    c->soil_dry_raw = 3200;
}



void config_load(void) {
    nvs_handle_t handle;
    if (nvs_open(CONFIG_NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        size_t len = sizeof(config);
        if (nvs_get_blob(handle, "data", &config, &len) == ESP_OK && len == sizeof(config)) {
            loaded = true;
        }
        nvs_close(handle);
    }
    if (!loaded) 
    {
        // === NEW: se non trovato → default + salva
        config_apply_defaults(&config);
        config_save(&config);
        loaded = true;
    }
}

bool config_is_valid(void) {
    return loaded;
}


void config_save(const config_data_t *data) {
    nvs_handle_t handle;
    if (nvs_open(CONFIG_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        nvs_set_blob(handle, "data", data, sizeof(*data));
        nvs_commit(handle);
        nvs_close(handle);
        config = *data;
        loaded = true;
    }
}

config_data_t config_get(void) {
    return config;
}

// === NEW: setter ===
bool config_set_batt_range(float vmin, float vmax) 
{
    if (vmax <= vmin || vmin < 2.5f || vmax > 5.5f) return false;
    config.batt_v_min = vmin;
    config.batt_v_max = vmax;
    config_save(&config);
    return true;
}

bool config_set_soil_wet_raw(uint16_t raw) 
{
    config.soil_wet_raw = raw;
    config_save(&config);
    return true;
}

bool config_set_soil_dry_raw(uint16_t raw) 
{
    config.soil_dry_raw = raw;
    config_save(&config);
    return true;
}

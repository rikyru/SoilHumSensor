#include "config.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>

static config_data_t config;
static bool loaded = false;

void config_load(void) {
    nvs_handle_t handle;
    if (nvs_open(CONFIG_NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        size_t len = sizeof(config);
        if (nvs_get_blob(handle, "data", &config, &len) == ESP_OK && len == sizeof(config)) {
            loaded = true;
        }
        nvs_close(handle);
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

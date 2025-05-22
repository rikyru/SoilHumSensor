#include "mqtt_wrapper.h"
#include "esp_log.h"
#include "config.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_system.h"
#include <stddef.h>        // Per NULL
#include "mqtt_client.h" 
#include <inttypes.h>  // Per PRIi32

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error");
            break;
        default:
            ESP_LOGI(TAG, "MQTT event ID: %" PRIi32, event_id);
            break;
    }
}


void start_mqtt(void) {
    config_data_t cfg = config_get();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .hostname = cfg.mqtt_host,
                .port = cfg.mqtt_port,
            }
        },
        .credentials = {
            .username = cfg.mqtt_user,
            .authentication = {
                .password = cfg.mqtt_pass,
            }
        },
        .session = {
            .keepalive = 30,
        },
        .network = {
            .disable_auto_reconnect = false,
        },
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if (!client) {
        ESP_LOGE(TAG, "Failed to init MQTT client");
        return;
    }

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}

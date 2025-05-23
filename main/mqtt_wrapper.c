/**
 * @file mqtt_wrapper.c
 * @brief MQTT client wrapper for soil humidity sensor
 * @details Handles MQTT connection, message publishing and HomeAssistant discovery
 */

#include "mqtt_wrapper.h"
#include "esp_log.h"
#include "config.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_system.h"
#include <stddef.h>        // Per NULL
#include "mqtt_client.h" 
#include <inttypes.h>  // Per PRIi32
#include "esp_mac.h"

/** @brief Tag for logging */
static const char *TAG = "MQTT";

/** @brief MQTT client handle */
static esp_mqtt_client_handle_t client = NULL;

/** @brief Device unique identifier derived from MAC address */
static char device_id[16] = {0};

/** @brief MQTT topic for publishing humidity readings */
static char topic_humidity[64] = {0};

/** @brief MQTT topic for publishing battery voltage readings */
static char topic_battery[64] = {0};

/** @brief MQTT topic for receiving sleep interval settings */
static char topic_set[64] = {0};

/** @brief MQTT topic for publishing current sleep interval */
static char topic_sleep[64] = {0};

/**
 * @brief MQTT event handler callback
 * @param handler_args User provided argument (unused)
 * @param base Event base
 * @param event_id Event ID received from MQTT client
 * @param event_data Event data containing MQTT event information
 */
static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            mqtt_publish_discovery();
            
            esp_mqtt_client_subscribe(client, topic_set, 1);
            ESP_LOGI(TAG, "Subscribed to %s", topic_set);
            char msg[16];
            snprintf(msg, sizeof(msg), "%d", config_get().sleep_minutes);
            esp_mqtt_client_publish(client, topic_sleep, msg, 0, 1, true); 
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error");
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Incoming on topic: %.*s", event->topic_len, event->topic);
            if (strncmp(event->topic, topic_set, event->topic_len) == 0) 
            {
                char val_str[16] = {0};
                memcpy(val_str, event->data, MIN(event->data_len, sizeof(val_str) - 1));
                int new_minutes = atoi(val_str);

                if (new_minutes >= 1 && new_minutes <= 1440) {
                    config_data_t cfg = config_get();
                    cfg.sleep_minutes = new_minutes;
                    config_save(&cfg);
                    ESP_LOGI(TAG, "Updated sleep interval to %d min", new_minutes);
                    // (Opzionale) pubblica subito il nuovo valore sul topic di stato
                    char state_topic[64], msg[16];
                    snprintf(state_topic, sizeof(state_topic), "soil_sensor/%s/sleep_interval", device_id);
                    snprintf(msg, sizeof(msg), "%d", new_minutes);
                    esp_mqtt_client_publish(client, state_topic, msg, 0, 1, true);
                }
            }
        break;
        default:
            ESP_LOGI(TAG, "MQTT event ID: %" PRIi32, event_id);
            break;
    }
}


/**
 * @brief Initialize and start MQTT client
 * @details Sets up MQTT client configuration using stored settings,
 *          initializes device ID and topic names, and starts connection
 */
void start_mqtt(void)
{
    if (device_id[0] == 0) 
    {
        /* read mac address and create topics */
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        snprintf(device_id, sizeof(device_id), "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
        snprintf(topic_humidity, sizeof(topic_humidity), "soil_sensor/%s/humidity", device_id);
        snprintf(topic_battery, sizeof(topic_battery), "soil_sensor/%s/battery", device_id);
        snprintf(topic_sleep, sizeof(topic_sleep), "soil_sensor/%s/sleep_interval", device_id);
        snprintf(topic_set, sizeof(topic_set), "soil_sensor/%s/sleep_interval/set", device_id);
    }
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

/**
 * @brief Publish HomeAssistant MQTT discovery messages
 * @details Sends device and sensor configuration for HomeAssistant auto-discovery:
 *          - Humidity sensor configuration
 *          - Battery voltage sensor configuration
 *          - Sleep interval control configuration
 * @note Messages are published with retain flag set to true
 */
void mqtt_publish_discovery(void)
{
    if (!client) {
        ESP_LOGW(TAG, "MQTT client not initialized");
        return;
    }

    // Usare lo stesso device_id già inizializzato in start_mqtt()
    char discovery_topic[128];
    char payload[512];

    // DEVICE section comune
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Soil Humidity\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"%%\","
            "\"device_class\":\"humidity\","
            "\"unique_id\":\"%s_humidity\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"],"
                "\"name\":\"SoilSensor %s\","
                "\"manufacturer\":\"Rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_humidity, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/sensor/soil_%s_humidity/config", device_id);

    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);  // retain = true

    // Battery sensor
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Battery Voltage\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"V\","
            "\"device_class\":\"voltage\","
            "\"unique_id\":\"%s_battery\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"],"
                "\"name\":\"SoilSensor %s\","
                "\"manufacturer\":\"Rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_battery, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/sensor/soil_%s_battery/config", device_id);

    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    snprintf(payload, sizeof(payload),
    "{"
        "\"name\":\"Sleep Interval\","
        "\"command_topic\":\"soil_sensor/%s/sleep_interval/set\","
        "\"state_topic\":\"soil_sensor/%s/sleep_interval\","
        "\"unit_of_measurement\":\"min\","
        "\"min\":1,\"max\":1440,\"step\":1,"
        "\"mode\":\"box\","
        "\"unique_id\":\"%s_sleep_interval\","
        "\"device\":{"
            "\"identifiers\":[\"%s\"],"
            "\"name\":\"SoilSensor %s\","
            "\"manufacturer\":\"Rikyru\","
            "\"model\":\"ESP32-C3 SoilSensor\""
        "}"
    "}", device_id, device_id, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_sleep_interval/config", device_id);

    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);
}

/**
 * @brief Publish sensor readings to MQTT broker
 * @param humidity Current soil humidity reading in percentage
 * @param battery_voltage Current battery voltage reading in volts
 * @details Publishes both humidity and battery readings to their respective topics.
 *          Values are formatted with appropriate precision (1 decimal for humidity,
 *          2 decimals for battery voltage)
 */
void mqtt_publish_sensor_data(float humidity, float battery_voltage)
{
    if (!client) {
        ESP_LOGW(TAG, "MQTT client not initialized");
        return;
    }

     // Crea payload
    char hum_str[16];
    snprintf(hum_str, sizeof(hum_str), "%.1f", humidity);

    char bat_str[16];
    snprintf(bat_str, sizeof(bat_str), "%.2f", battery_voltage);

    esp_mqtt_client_publish(client, topic_humidity, hum_str, 0, 1, false);
    esp_mqtt_client_publish(client, topic_battery, bat_str, 0, 1, false);
    ESP_LOGI(TAG, "Published humidity: %s to topic: %s", hum_str, topic_humidity);
    ESP_LOGI(TAG, "Published battery: %s to topic: %s", bat_str, topic_battery);
}

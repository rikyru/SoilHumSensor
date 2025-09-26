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
#include <stddef.h>        // NULL
#include "mqtt_client.h"
#include <inttypes.h>      // PRIi32
#include "esp_mac.h"
#include <stdio.h>         // snprintf
#include <string.h>        // memcpy, strcmp, strncmp
#include <stdlib.h>        // atoi, strtof
#include "sensor.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/** @brief Tag for logging */
static const char *TAG = "MQTT";

/** @brief MQTT client handle */
static esp_mqtt_client_handle_t client = NULL;

/** @brief Device unique identifier derived from MAC address */
static char device_id[16] = {0};

/** @brief Base topic "soil_sensor/<device_id>" */
static char base[64] = {0};

/** @brief MQTT topic for publishing humidity readings */
static char topic_humidity[128] = {0};

/** @brief MQTT topic for publishing battery voltage readings */
static char topic_battery[128] = {0};

/** @brief MQTT topic for publishing battery percentage (computed) */
static char topic_battery_pct[128] = {0};

/** @brief MQTT topic for receiving sleep interval settings */
static char topic_set[128] = {0};

/** @brief MQTT topic for publishing current sleep interval */
static char topic_sleep[128] = {0};

/** @brief MQTT topic for receiving calibration data for battery and humidity sensor */
static char topic_set_vmin[128], topic_set_vmax[128], topic_set_wet[128], topic_set_dry[128];

/** @brief MQTT topics for publishing current calibration parameters (retain) */
static char topic_batt_vmin_state[128], topic_batt_vmax_state[128], topic_soil_wet_state[128], topic_soil_dry_state[128];

/** @brief MQTT topic for receiving calibration command for humidity sensor (optional, not handled here) */
static char topic_cmd_mark_wet[128], topic_cmd_mark_dry[128];

/** @brief helper: compute battery % from voltage and current config (clamped 0..100) */
uint8_t batt_percent_from_v(float v)
{
    config_data_t c = config_get();
    float denom = (c.batt_v_max - c.batt_v_min);
    float p = (denom > 0.001f) ? (v - c.batt_v_min) / denom : 0.0f;
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    uint8_t pct = (unsigned)(p * 100.0f + 0.5f);
    if (pct > 100u) pct = 100u;
    return pct;
}

/**
 * @brief MQTT event handler callback
 * @param handler_args User provided argument (unused)
 * @param base Event base
 * @param event_id Event ID received from MQTT client
 * @param event_data Event data containing MQTT event information
 */
static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base_ev, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
        {
            ESP_LOGI(TAG, "MQTT connected");
            mqtt_publish_discovery();

            /* subscribe to control topics */
            esp_mqtt_client_subscribe(client, topic_set, 1);
            esp_mqtt_client_subscribe(client, topic_set_vmin, 1);
            esp_mqtt_client_subscribe(client, topic_set_vmax, 1);
            esp_mqtt_client_subscribe(client, topic_set_wet,  1);
            esp_mqtt_client_subscribe(client, topic_set_dry,  1);
            esp_mqtt_client_subscribe(client, topic_cmd_mark_wet, 1);
            esp_mqtt_client_subscribe(client, topic_cmd_mark_dry, 1);

            ESP_LOGI(TAG, "Subscribed to control topics.");

            /* publish current sleep interval (retain) */
            char msg[16];
            snprintf(msg, sizeof(msg), "%d", config_get().sleep_minutes);
            esp_mqtt_client_publish(client, topic_sleep, msg, 0, 1, true);

            /* publish current calibration values (retain) */
            {
                config_data_t c = config_get();
                char buf[16];

                snprintf(buf, sizeof(buf), "%.2f", c.batt_v_min);
                esp_mqtt_client_publish(client, topic_batt_vmin_state, buf, 0, 1, true);

                snprintf(buf, sizeof(buf), "%.2f", c.batt_v_max);
                esp_mqtt_client_publish(client, topic_batt_vmax_state, buf, 0, 1, true);

                snprintf(buf, sizeof(buf), "%u", c.soil_wet_raw);
                esp_mqtt_client_publish(client, topic_soil_wet_state, buf, 0, 1, true);

                snprintf(buf, sizeof(buf), "%u", c.soil_dry_raw);
                esp_mqtt_client_publish(client, topic_soil_dry_state, buf, 0, 1, true);
            }
            break;
        }
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error");
            break;

        case MQTT_EVENT_DATA:
        {
            ESP_LOGI(TAG, "Incoming on topic: %.*s", event->topic_len, event->topic);

            /* sleep_interval/set */
            if (strncmp(event->topic, topic_set, event->topic_len) == 0)
            {
                char val_str[16] = {0};
                memcpy(val_str, event->data, MIN(event->data_len, (int)sizeof(val_str) - 1));
                int new_minutes = atoi(val_str);

                if (new_minutes >= 0 && new_minutes <= 1440) {
                    config_data_t cfg = config_get();
                    cfg.sleep_minutes = new_minutes;
                    config_save(&cfg);
                    ESP_LOGI(TAG, "Updated sleep interval to %d min", new_minutes);
                    /* echo retained */
                    char state_topic[64], msg[16];
                    snprintf(state_topic, sizeof(state_topic), "soil_sensor/%s/sleep_interval", device_id);
                    snprintf(msg, sizeof(msg), "%d", new_minutes);
                    esp_mqtt_client_publish(client, state_topic, msg, 0, 1, true);
                }
            }
            /* batt_v_min */
            else if (strncmp(event->topic, topic_set_vmin, event->topic_len) == 0) {
                char s[16] = {0};
                memcpy(s, event->data, MIN(event->data_len, (int)sizeof(s)-1));
                float v = strtof(s, NULL);
                config_data_t c = config_get();
                if (v >= 2.50f && v < c.batt_v_max) {
                    c.batt_v_min = v;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%.2f", v);
                    esp_mqtt_client_publish(client, topic_batt_vmin_state, msg, 0, 1, true);
                    ESP_LOGI(TAG, "Updated batt_v_min -> %.2f V", v);
                } else {
                    ESP_LOGW(TAG, "Invalid batt_v_min %.2f", v);
                }
            }
            /* batt_v_max */
            else if (strncmp(event->topic, topic_set_vmax, event->topic_len) == 0) {
                char s[16] = {0};
                memcpy(s, event->data, MIN(event->data_len, (int)sizeof(s)-1));
                float v = strtof(s, NULL);
                config_data_t c = config_get();
                if (v <= 5.50f && v > c.batt_v_min) {
                    c.batt_v_max = v;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%.2f", v);
                    esp_mqtt_client_publish(client, topic_batt_vmax_state, msg, 0, 1, true);
                    ESP_LOGI(TAG, "Updated batt_v_max -> %.2f V", v);
                } else {
                    ESP_LOGW(TAG, "Invalid batt_v_max %.2f", v);
                }
            }
            /* soil_wet_raw */
            else if (strncmp(event->topic, topic_set_wet, event->topic_len) == 0) {
                char s[16] = {0};
                memcpy(s, event->data, MIN(event->data_len, (int)sizeof(s)-1));
                int r = atoi(s);
                if (r >= 0 && r <= 4095) {
                    config_data_t c = config_get();
                    c.soil_wet_raw = (uint16_t)r;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%u", c.soil_wet_raw);
                    esp_mqtt_client_publish(client, topic_soil_wet_state, msg, 0, 1, true);
                    ESP_LOGI(TAG, "Updated soil_wet_raw -> %u", c.soil_wet_raw);
                } else {
                    ESP_LOGW(TAG, "Invalid soil_wet_raw %d", r);
                }
            }
            /* soil_dry_raw */
            else if (strncmp(event->topic, topic_set_dry, event->topic_len) == 0) {
                char s[16] = {0};
                memcpy(s, event->data, MIN(event->data_len, (int)sizeof(s)-1));
                int r = atoi(s);
                if (r >= 0 && r <= 4095) {
                    config_data_t c = config_get();
                    c.soil_dry_raw = (uint16_t)r;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%u", c.soil_dry_raw);
                    esp_mqtt_client_publish(client, topic_soil_dry_state, msg, 0, 1, true);
                    ESP_LOGI(TAG, "Updated soil_dry_raw -> %u", c.soil_dry_raw);
                } else {
                    ESP_LOGW(TAG, "Invalid soil_dry_raw %d", r);
                }
            }
            /* commands for mark wet/dry */
            else if (strncmp(event->topic, topic_cmd_mark_wet, event->topic_len) == 0) 
            {
                int raw = sensor_read_soil_raw_avg();
                if (raw >= 0 && raw <= 4095) 
                {
                    config_data_t c = config_get();
                    c.soil_wet_raw = (uint16_t)raw;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%u", c.soil_wet_raw);
                    esp_mqtt_client_publish(client, topic_soil_wet_state, msg, 0, 1, true);
                    // rileggi umidità già con nuova calibrazione e ripubblica
                    float h = read_soil_moisture(); 
                    mqtt_publish_sensor_data(h, read_battery_voltage());
                }
            }
            else if (strncmp(event->topic, topic_cmd_mark_dry, event->topic_len) == 0) 
            {
                int raw = sensor_read_soil_raw_avg();
                if (raw >= 0 && raw <= 4095) {
                    config_data_t c = config_get();
                    c.soil_dry_raw = (uint16_t)raw;
                    config_save(&c);
                    char msg[16]; snprintf(msg, sizeof(msg), "%u", c.soil_dry_raw);
                    esp_mqtt_client_publish(client, topic_soil_dry_state, msg, 0, 1, true);
                    // rileggi umidità già con nuova calibrazione e ripubblica
                    float h = read_soil_moisture(); 
                    mqtt_publish_sensor_data(h, read_battery_voltage());
                }
            }


            break;
        }

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

        /* base topic */
        snprintf(base, sizeof(base), "soil_sensor/%s", device_id);

        /* measurement topics */
        snprintf(topic_humidity, sizeof(topic_humidity), "%s/humidity", base);
        snprintf(topic_battery,  sizeof(topic_battery),  "%s/battery",  base);
        snprintf(topic_battery_pct, sizeof(topic_battery_pct), "%s/battery_pct", base);

        /* sleep control topics */
        snprintf(topic_sleep, sizeof(topic_sleep), "%s/sleep_interval", base);
        snprintf(topic_set,   sizeof(topic_set),   "%s/sleep_interval/set", base);

        /* calibration set topics */
        snprintf(topic_set_vmin, sizeof(topic_set_vmin), "%s/set/batt_v_min", base);
        snprintf(topic_set_vmax, sizeof(topic_set_vmax), "%s/set/batt_v_max", base);
        snprintf(topic_set_wet,  sizeof(topic_set_wet),  "%s/set/soil_wet_raw", base);
        snprintf(topic_set_dry,  sizeof(topic_set_dry),  "%s/set/soil_dry_raw", base);

        /* calibration state topics (retain) */
        snprintf(topic_batt_vmin_state, sizeof(topic_batt_vmin_state), "%s/batt_v_min", base);
        snprintf(topic_batt_vmax_state, sizeof(topic_batt_vmax_state), "%s/batt_v_max", base);
        snprintf(topic_soil_wet_state,  sizeof(topic_soil_wet_state),  "%s/soil_wet_raw", base);
        snprintf(topic_soil_dry_state,  sizeof(topic_soil_dry_state),  "%s/soil_dry_raw", base);

        /* optional command topics (not handled in this file) */
        snprintf(topic_cmd_mark_wet, sizeof(topic_cmd_mark_wet), "%s/cmd/soil_mark_wet", base);
        snprintf(topic_cmd_mark_dry, sizeof(topic_cmd_mark_dry), "%s/cmd/soil_mark_dry", base);
    }

    config_data_t cfg = config_get();

    char broker_uri[128] = {0};

    /* Check host validity */
    if (cfg.mqtt_host[0] == '\0')
    {
        ESP_LOGE(TAG, "MQTT host is empty");
        return;
    }

    /* User must NOT include mqtt:// */
    if (strstr(cfg.mqtt_host, "mqtt://"))
    {
        ESP_LOGE(TAG, "ERROR: mqtt_host should not include 'mqtt://'. Please enter only the hostname.");
        return;
    }

    snprintf(broker_uri, sizeof(broker_uri), "mqtt://%s", cfg.mqtt_host);

    esp_mqtt_client_config_t mqtt_cfg =
    {
        .broker = {
            .address.uri = broker_uri
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
    ESP_LOGI(TAG, "MQTT config:");
    ESP_LOGI(TAG, "  URI: %s", mqtt_cfg.broker.address.uri);
    ESP_LOGI(TAG, "  Username: %s", mqtt_cfg.credentials.username ? mqtt_cfg.credentials.username : "(none)");
    ESP_LOGI(TAG, "  Password: %s", mqtt_cfg.credentials.authentication.password ? mqtt_cfg.credentials.authentication.password : "(none)");
    ESP_LOGI(TAG, "  Keepalive: %d", mqtt_cfg.session.keepalive);
    ESP_LOGI(TAG, "  Disable auto reconnect: %s", mqtt_cfg.network.disable_auto_reconnect ? "true" : "false");
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
 *          - Battery percentage sensor configuration
 *          - Sleep interval control configuration
 *          - Number entities for calibration (batt_v_min/max, soil wet/dry raw)
 * @note Messages are published with retain flag set to true
 */
void mqtt_publish_discovery(void)
{
    if (!client) {
        ESP_LOGW(TAG, "MQTT client not initialized");
        return;
    }

    char discovery_topic[160];
    char payload[600];

    /* HUMIDITY (%) */
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
                "\"manufacturer\":\"rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_humidity, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/sensor/soil_%s_humidity/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* BATTERY VOLTAGE (V) */
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
                "\"manufacturer\":\"rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_battery, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/sensor/soil_%s_battery/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* BATTERY % */
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Battery %%\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"%%\","
            "\"device_class\":\"battery\","
            "\"unique_id\":\"%s_battery_pct\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"],"
                "\"name\":\"SoilSensor %s\","
                "\"manufacturer\":\"rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_battery_pct, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/sensor/soil_%s_battery_pct/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* NUMBER: Sleep Interval */
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Sleep Interval\","
            "\"command_topic\":\"%s\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"min\","
            "\"min\":0,\"max\":1440,\"step\":1,"
            "\"mode\":\"box\","
            "\"retain\":true,"
            "\"unique_id\":\"%s_sleep_interval\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"],"
                "\"name\":\"SoilSensor %s\","
                "\"manufacturer\":\"rikyru\","
                "\"model\":\"ESP32-C3 SoilSensor\""
            "}"
        "}", topic_set, topic_sleep, device_id, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_sleep_interval/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* NUMBER: Batt Vmin */
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Batt Vmin\","
            "\"command_topic\":\"%s\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"V\","
            "\"min\":2.5,\"max\":5.5,\"step\":0.01,"
            "\"mode\":\"box\","
            "\"retain\":true,"
            "\"unique_id\":\"%s_batt_v_min\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"]"
            "}"
        "}", topic_set_vmin, topic_batt_vmin_state, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_batt_vmin/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* NUMBER: Batt Vmax */
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Batt Vmax\","
            "\"command_topic\":\"%s\","
            "\"state_topic\":\"%s\","
            "\"unit_of_measurement\":\"V\","
            "\"min\":2.5,\"max\":5.5,\"step\":0.01,"
            "\"mode\":\"box\","
            "\"retain\":true,"
            "\"unique_id\":\"%s_batt_v_max\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"]"
            "}"
        "}", topic_set_vmax, topic_batt_vmax_state, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_batt_vmax/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    /* NUMBER: Soil wet/dry RAW */
    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Soil Wet RAW\","
            "\"command_topic\":\"%s\","
            "\"state_topic\":\"%s\","
            "\"min\":0,\"max\":4095,\"step\":1,"
            "\"mode\":\"box\","
            "\"retain\":true,"
            "\"unique_id\":\"%s_soil_wet_raw\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"]"
            "}"
        "}", topic_set_wet, topic_soil_wet_state, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_wet_raw/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    snprintf(payload, sizeof(payload),
        "{"
            "\"name\":\"Soil Dry RAW\","
            "\"command_topic\":\"%s\","
            "\"state_topic\":\"%s\","
            "\"min\":0,\"max\":4095,\"step\":1,"
            "\"mode\":\"box\","
            "\"retain\":true,"
            "\"unique_id\":\"%s_soil_dry_raw\","
            "\"device\":{"
                "\"identifiers\":[\"%s\"]"
            "}"
        "}", topic_set_dry, topic_soil_dry_state, device_id, device_id);

    snprintf(discovery_topic, sizeof(discovery_topic),
        "homeassistant/number/soil_%s_dry_raw/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    // Button: Segna Bagnato
    snprintf(payload, sizeof(payload),
    "{"
        "\"name\":\"Segna Bagnato\","
        "\"command_topic\":\"%s\","
        "\"payload_press\":\"1\","
        "\"unique_id\":\"%s_soil_mark_wet\","
        "\"device\":{\"identifiers\":[\"%s\"]}"
    "}", topic_cmd_mark_wet, device_id, device_id);
    snprintf(discovery_topic, sizeof(discovery_topic),
    "homeassistant/button/soil_%s_mark_wet/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

    // Button: Segna Asciutto
    snprintf(payload, sizeof(payload),
    "{"
        "\"name\":\"Segna Asciutto\","
        "\"command_topic\":\"%s\","
        "\"payload_press\":\"1\","
        "\"unique_id\":\"%s_soil_mark_dry\","
        "\"device\":{\"identifiers\":[\"%s\"]}"
    "}", topic_cmd_mark_dry, device_id, device_id);
    snprintf(discovery_topic, sizeof(discovery_topic),
    "homeassistant/button/soil_%s_mark_dry/config", device_id);
    esp_mqtt_client_publish(client, discovery_topic, payload, 0, 1, true);

}

/**
 * @brief Publish sensor readings to MQTT broker
 * @param humidity Current soil humidity reading in percentage
 * @param battery_voltage Current battery voltage reading in volts
 * @details Publishes humidity, battery voltage, and battery percentage
 */
void mqtt_publish_sensor_data(float humidity, float battery_voltage)
{
    if (!client) {
        ESP_LOGW(TAG, "MQTT client not initialized");
        return;
    }

    char hum_str[16];
    snprintf(hum_str, sizeof(hum_str), "%.1f", humidity);

    char bat_str[16];
    snprintf(bat_str, sizeof(bat_str), "%.2f", battery_voltage);

    char bpct_str[8];
    snprintf(bpct_str, sizeof(bpct_str), "%u", batt_percent_from_v(battery_voltage));

    esp_mqtt_client_publish(client, topic_humidity, hum_str, 0, 1, false);
    esp_mqtt_client_publish(client, topic_battery,  bat_str, 0, 1, false);
    esp_mqtt_client_publish(client, topic_battery_pct, bpct_str, 0, 1, false);

    ESP_LOGI(TAG, "Published humidity: %s to topic: %s", hum_str, topic_humidity);
    ESP_LOGI(TAG, "Published battery: %s to topic: %s", bat_str, topic_battery);
    ESP_LOGI(TAG, "Published battery %%: %s to topic: %s", bpct_str, topic_battery_pct);
}

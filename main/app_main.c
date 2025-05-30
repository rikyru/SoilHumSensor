// main/app_main.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "wifi_provisioning.h"
#include "mqtt_wrapper.h"
#include "sensor.h"
#include "sleep_control.h"
#include "config.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"

static const char *TAG = "MAIN";
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_DISCONNECTED_BIT BIT1

void battery_task(void *param) {
    while (1) {
        float vbat = read_battery_voltage();
        float humidity = 0; // forced

        if (vbat > 0) {
            char msg[16];
            snprintf(msg, sizeof(msg), "%.2f", vbat);
            mqtt_publish_sensor_data(humidity, vbat);
            vTaskDelay(pdMS_TO_TICKS(2000));  // aspetta 2s per sicurezza che parta MQTT
        }

        enter_deep_sleep(30);  // dormi 30 secondi
    }
}


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Wi-Fi disconnected, trying to reconnect...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, WIFI_DISCONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP, starting MQTT...");

        start_mqtt();
        xTaskCreate(battery_task, "battery_task", 2048, NULL, 5, NULL);

        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupClearBits(wifi_event_group, WIFI_DISCONNECTED_BIT);
    }
}


void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    config_load();
    sensor_init();  // Inizializza i sensori

    if (!config_is_valid()) {
        ESP_LOGI(TAG, "No valid config found, starting provisioning.");
        start_wifi_provisioning();
        return;
    }
    

    // Event loop + handler per connessione Wi-Fi
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_connect_from_config();  // si connette, poi chiama la callback
}

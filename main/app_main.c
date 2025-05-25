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

static const char *TAG = "MAIN";

static void on_wifi_connected(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ESP_LOGI(TAG, "Got IP, starting MQTT...");

        start_mqtt();

    //     float humidity = read_soil_moisture();
    //     float battery = read_battery_voltage();
    //     mqtt_publish_sensor_data(humidity, battery);

    //     mqtt_listen_for_sleep_update();
    //     sleep_control_enter();
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    config_load();

    if (!config_is_valid()) {
        ESP_LOGI(TAG, "No valid config found, starting provisioning.");
        start_wifi_provisioning();
        return;
    }

    // Event loop + handler per connessione Wi-Fi
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_wifi_connected, NULL));

    wifi_connect_from_config();  // si connette, poi chiama la callback
}

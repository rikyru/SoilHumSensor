// main/app_main.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_provisioning.h"
#include "mqtt_wrapper.h"
#include "sensor.h"
#include "sleep_control.h"
#include "config.h"
#include "mqtt_client.h"



void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    config_load();

    if (!config_is_valid()) {
        ESP_LOGI("MAIN", "No valid config found, starting provisioning.");
        start_wifi_provisioning();
        return;
    }

    wifi_connect_from_config();
    start_mqtt();
    // TBD
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // if (!esp_mqtt_client_is_connected()) 
    // {
    //     // TBD
    //     //handle_mqtt_failure();
    //     return;
    // }

    // float humidity = read_soil_moisture();
    // float battery = read_battery_voltage();
    // mqtt_publish_sensor_data(humidity, battery);

    // mqtt_listen_for_sleep_update();
    // sleep_control_enter();
}

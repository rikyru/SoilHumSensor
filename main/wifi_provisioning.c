#include "wifi_provisioning.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "config.h"
#include <string.h>

#define TAG "PROVISIONING"
static httpd_handle_t server = NULL;

static esp_err_t handle_get(httpd_req_t *req) {
    const char *form_html =
        "<html><body><h2>Soil Sensor Setup</h2>"
        "<form method='POST'>"
        "SSID: <input name='ssid'><br>"
        "Password: <input name='password' type='password'><br>"
        "MQTT Host: <input name='mqtt_host'><br>"
        "MQTT Port: <input name='mqtt_port' type='number'><br>"
        "MQTT User: <input name='mqtt_user'><br>"
        "MQTT Pass: <input name='mqtt_pass' type='password'><br>"
        "Sleep (min): <input name='sleep_interval' type='number'><br>"
        "<input type='submit' value='Save'></form></body></html>";

    httpd_resp_send(req, form_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t handle_post(httpd_req_t *req) {
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    config_data_t cfg = {0};
    sscanf(buf, "ssid=%[^&]&password=%[^&]&mqtt_host=%[^&]&mqtt_port=%d&mqtt_user=%[^&]&mqtt_pass=%[^&]&sleep_interval=%d",
           cfg.wifi_ssid, cfg.wifi_pass, cfg.mqtt_host, &cfg.mqtt_port, cfg.mqtt_user, cfg.mqtt_pass, &cfg.sleep_minutes);

    config_save(&cfg);
    httpd_resp_sendstr(req, "Saved. Rebooting...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}

static httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = handle_get
};

static httpd_uri_t uri_post = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = handle_post
};

static void start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);
    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_post);
}

void start_wifi_provisioning(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "SoilSensor",
            .ssid_len = 0,
            .channel = 1,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Access point started. Connect to 'SoilSensor' and go to 192.168.4.1");
    start_http_server();
}

void wifi_connect_from_config(void) {
    config_data_t config = config_get();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t sta_cfg = {0};
    strcpy((char *)sta_cfg.sta.ssid, config.wifi_ssid);
    strcpy((char *)sta_cfg.sta.password, config.wifi_pass);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_cfg);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
}

#include "wifi_provisioning.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "config.h"
#include "form_html.h"
#include <string.h>
#include "esp_mac.h"

#define TAG "PROVISIONING"
static httpd_handle_t server = NULL;



static esp_err_t handle_get(httpd_req_t *req) {
    httpd_resp_send(req, form_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t handle_post(httpd_req_t *req) {
    char buf[1024];
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



static esp_err_t handle_scan(httpd_req_t *req) {
    ESP_LOGI(TAG, ">> handle_scan() called");

    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };

    esp_err_t err = esp_wifi_scan_start(&scan_config, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Scan start failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Scan failed");
        return err;
    }

    uint16_t ap_num = 0;
    wifi_ap_record_t ap_records[20];
    esp_wifi_scan_get_ap_num(&ap_num);
    if (ap_num > 20) ap_num = 20;
    esp_wifi_scan_get_ap_records(&ap_num, ap_records);

    ESP_LOGI(TAG, "Found %d networks", ap_num);

    for (int i = 0; i < ap_num; i++) {
        ESP_LOGI(TAG, "SSID[%d]: %s, RSSI: %d", i, ap_records[i].ssid, ap_records[i].rssi);
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");

    for (int i = 0; i < ap_num; i++) {
        httpd_resp_sendstr_chunk(req, "\"");
        httpd_resp_sendstr_chunk(req, (const char *)ap_records[i].ssid);
        httpd_resp_sendstr_chunk(req, i < ap_num - 1 ? "\"," : "\"");
    }

    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, NULL);

    ESP_LOGI(TAG, "<< handle_scan() done");
    return ESP_OK;
}

static void start_http_server(void) 
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;  // Aumenta da 4096 a 8192
    config.max_open_sockets = 4;
    config.recv_wait_timeout = 10;

    httpd_start(&server, &config);
    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_post);

    httpd_uri_t uri_scan = {
        .uri = "/scan",
        .method = HTTP_GET,
        .handler = handle_scan
    };
    httpd_register_uri_handler(server, &uri_scan);
}

void start_wifi_provisioning(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);  // oppure ESP_MAC_WIFI_STA

    char ssid[32];
    snprintf(ssid, sizeof(ssid), "SoilSensor%02X%02X", mac[4], mac[5]);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_APSTA);

    wifi_config_t ap_config = {
        .ap = {
            .ssid_len = strlen(ssid),
            .channel = 1,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };
    strcpy((char *)ap_config.ap.ssid, ssid);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Access point started. Connect to '%s' and go to 192.168.4.1", ssid);
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





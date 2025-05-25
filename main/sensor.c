// sensor.c

#include "sensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#define TAG "SENSOR"

// Pin A0 della XIAO ESP32-C3 = GPIO0 = ADC_CHANNEL_2 (ADC1)
#define VBAT_ADC_CHANNEL ADC_CHANNEL_2
#define VBAT_DIVIDER_RATIO 2.0  // per partitore 220k/220k

static adc_oneshot_unit_handle_t adc_handle;

void sensor_init(void) {
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // 12 bit
        .atten = ADC_ATTEN_DB_11           // fino a ~3.3V
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, VBAT_ADC_CHANNEL, &chan_cfg));
}

float read_battery_voltage(void) {
    int raw = 0;
    esp_err_t err = adc_oneshot_read(adc_handle, VBAT_ADC_CHANNEL, &raw);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(err));
        return -1.0f;
    }

    float voltage = (raw / 4095.0f) * 3.3f * VBAT_DIVIDER_RATIO;
    ESP_LOGI(TAG, "Battery raw: %d -> %.2f V", raw, voltage);
    return voltage;
}

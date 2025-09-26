// sensor.c

#include "sensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include <math.h>


#define TAG "SENSOR"

// Pin A0 della XIAO ESP32-C3 = GPIO0 = ADC_CHANNEL_2 (ADC1)
#define VBAT_ADC_CHANNEL ADC_CHANNEL_2
#define VBAT_DIVIDER_RATIO 2.0  // per partitore 220k/220k
#define SOIL_ADC_CHANNEL ADC_CHANNEL_3  // GPIO3
#define SOIL_POWER_GPIO  GPIO_NUM_10    // D10

static float clamp01(float x){ if (x<0) return 0; if (x>1) return 1; return x; }
static float map01(float x, float a, float b)
{
    if (fabsf(b-a) < 1e-6f) return 0.f;
    return (x - a) / (b - a);
}

static adc_oneshot_unit_handle_t adc_handle;

void sensor_init(void) {
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t vbat_chan_cfg  = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // 12 bit
        .atten = ADC_ATTEN_DB_11           // fino a ~3.3V
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, VBAT_ADC_CHANNEL, &vbat_chan_cfg));

    // Alimentazione sensore su GPIO10
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SOIL_POWER_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(SOIL_POWER_GPIO, 0);  // spegnilo di default

    // ADC configurazione già esistente per GPIO1
    adc_oneshot_chan_cfg_t soil_chan_cfg  = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, SOIL_ADC_CHANNEL, &soil_chan_cfg ));
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


// float read_soil_moisture(void) {
//     // Accendi il sensore
//     gpio_set_level(SOIL_POWER_GPIO, 1);
//     vTaskDelay(pdMS_TO_TICKS(1500));  // attesa stabilizzazione

//     int raw = 0;
//     esp_err_t err;// adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &raw);
//     float sum = 0;
//     for (int i=0; i<10; i++) 
//     {
//         err  = adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &raw);
//         sum += raw;
//         vTaskDelay(pdMS_TO_TICKS(80));  // piccolo ritardo tra letture
//     }
//     float average = sum / 10.0f;

//     // Spegni il sensore
//     gpio_set_level(SOIL_POWER_GPIO, 0);

//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "ADC read failed for moisture: %s", esp_err_to_name(err));
//         return -1.0f;
//     }

//     // Mappatura: più bagnato = valore più basso
//     float moisture_percent = 100.0f - ((average / 4095.0f) * 100.0f);
//     ESP_LOGI(TAG, "Moisture raw: %f -> %.1f %%", average, moisture_percent);
//     return moisture_percent;
// }

float read_soil_moisture(void) {
    // Accendi il sensore e attendi stabilizzazione
    gpio_set_level(SOIL_POWER_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1500));

    int raw = 0;
    esp_err_t err = ESP_OK;
    int sum = 0;

    // Stesso schema di campionamento che avevi (10 campioni con 80 ms)
    for (int i = 0; i < 10; i++) {
        err = adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &raw);
        if (err != ESP_OK) break;
        sum += raw;
        vTaskDelay(pdMS_TO_TICKS(80));
    }

    // Spegni il sensore
    gpio_set_level(SOIL_POWER_GPIO, 0);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed for moisture: %s", esp_err_to_name(err));
        return -1.0f;
    }

    // Media arrotondata
    int avg = (sum + 5) / 10;

    // Mappatura con calibrazione da NVS:
    // 0% = asciutto (soil_dry_raw), 100% = bagnato (soil_wet_raw),
    // funziona anche se wet < dry (verso invertito)
    config_data_t c = config_get();
    float p;

    if (c.soil_wet_raw != c.soil_dry_raw) {
        p = ((float)avg - (float)c.soil_dry_raw) /
            ((float)c.soil_wet_raw - (float)c.soil_dry_raw);
    } else {
        // Fallback legacy se non calibrato
        p = 1.0f - ((float)avg / 4095.0f);
    }

    // Clamp 0..1
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;

    float moisture_percent = p * 100.0f;
    ESP_LOGI(TAG, "Moisture raw:%d -> %.1f%% (dry:%u wet:%u)",
             avg, moisture_percent, c.soil_dry_raw, c.soil_wet_raw);

    return moisture_percent;
}


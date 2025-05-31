#include "esp_sleep.h"
#include "esp_log.h"
#include <inttypes.h>
#include "config.h"

#define TAG "SLEEP"

void enter_deep_sleep()
{

    int mins = config_get().sleep_minutes;
    if (mins > 0) {
        ESP_LOGI("SLEEP", "Going to deep sleep for %d min", mins);
        esp_sleep_enable_timer_wakeup((uint64_t)mins * 60 * 1000000ULL);
        esp_deep_sleep_start();
    } else {
        ESP_LOGI("SLEEP", "Sleep disabled, staying awake");
    }
}

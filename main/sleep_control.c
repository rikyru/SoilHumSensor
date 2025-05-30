#include "esp_sleep.h"
#include "esp_log.h"
#include <inttypes.h>

#define TAG "SLEEP"

void enter_deep_sleep(uint32_t seconds)
{
    ESP_LOGI(TAG, "Going to deep sleep for %" PRIu32 " seconds...", seconds);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

# ESP32-C3 Soil Humidity Sensor (Low Power)

A battery-powered soil humidity sensor using an ESP32-C3, designed for ultra-low power operation, with:
- Configurable sleep interval
- MQTT publishing
- Home Assistant MQTT Discovery integration
- Captive portal provisioning via web interface

## 🚀 Features

- 🪫 Deep sleep between readings (minutes, configurable via MQTT)
- 🌱 Capacitive soil moisture sensor (analog), calibration can be made on HA
- 🔋 Battery voltage monitoring (ADC), calibration of battery readings on HA
- 📡 MQTT client for Home Assistant with buttons for calibration (set wet/dry soil)
- 📲 Wi-Fi + MQTT provisioning via captive portal (no hardcoded creds)
- 📦 Lightweight code using ESP-IDF and FreeRTOS

## 🛠️ Setup

### Flash firmware

```bash
idf.py set-target esp32c3
idf.py build flash monitor
```

## 🟢 First Boot

1. Device starts in **Access Point mode** (`SoilSensor`).
2. Connect via Wi-Fi and open [http://192.168.4.1](http://192.168.4.1).
3. Enter Wi-Fi & MQTT credentials and set the sleep interval.

---

## 🧪 Hardware Requirements

- **ESP32-C3** (Used Xiao ESP32C3, it integrates battery charge circuit)
- **Capacitive soil humidity sensor**
- **Voltage divider** for battery monitoring (see https://forum.seeedstudio.com/t/battery-voltage-monitor-and-ad-conversion-for-xiao-esp32c/267535)
- *(Optional)* Reset button and status LED

---

## 📡 MQTT Topics

| Topic                             | Payload      | Descrizione                 | Retain |
| --------------------------------- | ------------ | --------------------------- | :----: |
| `soil_sensor/<id>/humidity`       | `float` (%)  | Umidità suolo (%)           |    ❌   |
| `soil_sensor/<id>/battery`        | `float` (V)  | Tensione batteria           |    ❌   |
| `soil_sensor/<id>/battery_pct`    | `int` (%)    | % batteria (da Vmin/Vmax)   |    ❌   |
| `soil_sensor/<id>/sleep_interval` | `int` (min)  | Intervallo sleep corrente   |    ✅   |
| `soil_sensor/<id>/batt_v_min`     | `float` (V)  | Echo Vmin salvato in NVS    |    ✅   |
| `soil_sensor/<id>/batt_v_max`     | `float` (V)  | Echo Vmax salvato in NVS    |    ✅   |
| `soil_sensor/<id>/soil_wet_raw`   | `int` 0–4095 | Echo RAW “bagnato” salvato  |    ✅   |
| `soil_sensor/<id>/soil_dry_raw`   | `int` 0–4095 | Echo RAW “asciutto” salvato |    ✅   |

| Topic                                 | Payload      | Effetto                                          | Retain |
| ------------------------------------- | ------------ | ------------------------------------------------ | :----: |
| `soil_sensor/<id>/sleep_interval/set` | `int` (min)  | Imposta intervallo sleep                         |    ❌   |
| `soil_sensor/<id>/set/batt_v_min`     | `float` (V)  | Salva **Vmin** in NVS + pubblica echo            |    ❌   |
| `soil_sensor/<id>/set/batt_v_max`     | `float` (V)  | Salva **Vmax** in NVS + pubblica echo            |    ❌   |
| `soil_sensor/<id>/set/soil_wet_raw`   | `int` 0–4095 | Salva RAW **bagnato** + pubblica echo            |    ❌   |
| `soil_sensor/<id>/set/soil_dry_raw`   | `int` 0–4095 | Salva RAW **asciutto** + pubblica echo           |    ❌   |
| `soil_sensor/<id>/cmd/soil_mark_wet`* | qualsiasi    | Legge RAW ora → salva come **bagnato** (+ echo)  |    ❌   |
| `soil_sensor/<id>/cmd/soil_mark_dry`* | qualsiasi    | Legge RAW ora → salva come **asciutto** (+ echo) |    ❌   |


- Supports MQTT discovery via Home Assistant.

---

## 📁 Project Structure

```
main/
├── app_main.c
├── wifi_provisioning.c/.h
├── config.c/.h
├── mqtt_client.c/.h     (coming soon)
├── sensor.c/.h          (coming soon)
├── sleep_control.c/.h   (coming soon)
```

---

## 🤝 License

MIT License 


##  Author

[rikyru] – Feel free to fork and contribute!

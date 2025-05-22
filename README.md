# ESP32-C3 Soil Humidity Sensor (Low Power)

A battery-powered soil humidity sensor using an ESP32-C3, designed for ultra-low power operation, with:
- Configurable sleep interval
- MQTT publishing
- Home Assistant MQTT Discovery integration
- Captive portal provisioning via web interface

## 🚀 Features

- 🪫 Deep sleep between readings (minutes, configurable via MQTT)
- 🌱 Capacitive soil moisture sensor (analog)
- 🔋 Battery voltage monitoring (ADC)
- 📡 MQTT client for Home Assistant
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

| Topic                        | Description                    |
|------------------------------|--------------------------------|
| `soil_sensor/humidity`       | Soil moisture reading (%)      |
| `soil_sensor/battery`        | Battery voltage (V)            |
| `soil_sensor/sleep_interval/set` | Set sleep interval (minutes) |

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

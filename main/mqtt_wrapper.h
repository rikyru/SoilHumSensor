#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

void start_mqtt(void);
void mqtt_publish_sensor_data(float humidity, float battery_voltage);


#endif
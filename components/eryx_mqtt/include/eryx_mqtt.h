#ifndef ERYX_MQTT_H
#define ERYX_MQTT_H

void eryx_mqtt_initialize(void);
void eryx_mqtt_wait_for_connection(void);
void eryx_mqtt_publish(const char*, const char*);

#endif // ERYX_MQTT_H
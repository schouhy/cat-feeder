#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include "esp_event.h"


void init_servo(void);

void lid_handler_open(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);
void lid_handler_close(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);

#endif // SERVO_DRIVER_H
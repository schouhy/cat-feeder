#include "feeder_state.h"
#include "esp_log.h"
#include "eryx_mqtt.h"
#include "stdio.h"


static const char *TAG = "SYSTEM_STATUS";
static char * mqtt_topic_ptr;

static SystemState current_state = BOOTING;

SystemState get_current_state(void)
{
    return current_state;
}

void system_next_state(void)
{
    
    switch (current_state) {
        case BOOTING:
            current_state = WAITING_FOR_SCHEDULE;
            break;
        case WAITING_FOR_SCHEDULE:
            current_state = SCHEDULED;
            break;
        case SCHEDULED:
            current_state = OPENING_LID;
            break;
        case OPENING_LID:
            current_state = LID_OPEN;
            break;
        case LID_OPEN:
            current_state = CLOSING_LID;
            break;
        case CLOSING_LID:
            current_state = END;
            break;
        case END:
            free(mqtt_topic_ptr);
            return;
    }
    ESP_LOGI(TAG, "New state: %i", current_state);

    if(mqtt_topic_ptr == NULL) {
        size_t topic_bufsize = snprintf(NULL, 0, "iot/%s/data", CONFIG_MQTT_USERNAME) + 1;
        mqtt_topic_ptr = malloc(topic_bufsize);
        snprintf(mqtt_topic_ptr, topic_bufsize, "iot/%s/data", CONFIG_MQTT_USERNAME);
    } 

    char state_message[15];
    snprintf(state_message, 15, "[{\"state\": %i}]", current_state);
    eryx_mqtt_publish(mqtt_topic_ptr, state_message);
}

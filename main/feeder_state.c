#include "feeder_state.h"
#include "esp_log.h"

static const char *TAG = "SYSTEM_STATUS";

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
            break;
    }
    ESP_LOGI(TAG, "New state: %i", current_state);
}

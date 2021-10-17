
#ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "esp_event.h"
#include "esp_timer.h"

// Declare an event base
ESP_EVENT_DECLARE_BASE(TIMER_EVENTS);        // declaration of the timer events family

enum {                                       // declaration of the specific events under the timer event family
    TIMER_EVENT_STARTED,                     // raised when the timer is first started
    TIMER_EVENT_EXPIRY,                      // raised when a period of the timer has elapsed
    TIMER_EVENT_STOPPED                      // raised when the timer has been stopped
};

// Declare an event base
ESP_EVENT_DECLARE_BASE(LID_EVENTS);          // declaration of the lid events family

enum {                                       // declaration of the specific events under the lid event family
    LID_EVENT_OPEN,                          // raised when the lid has to been open
    LID_EVENT_CLOSE                          // raised when the lid has to been closed
};

// Declare an event base
ESP_EVENT_DECLARE_BASE(WEBSERVER_EVENTS);    // declaration of the webserver events family

enum {                                       // declartation of theh specific events under the webserver event family
    WEBSERVER_EVENT_START,                   // raised when webserver is first started 
    WEBSERVER_EVENT_TIME_CONFIGURED,         // raised when user successfully configured the opening lid time
    WEBSERVER_EVENT_STOP                     // raised when the webserver has been stopped
};



#endif // #ifndef EVENT_SOURCE_H_

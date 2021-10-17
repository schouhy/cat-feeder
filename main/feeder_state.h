#ifndef STATE_H
#define STATE_H

typedef enum {
    BOOTING,
    WAITING_FOR_SCHEDULE,
    SCHEDULED, 
    OPENING_LID,
    LID_OPEN,
    CLOSING_LID,
    END
} SystemState;

SystemState get_current_state(void);
void system_next_state(void);

#endif

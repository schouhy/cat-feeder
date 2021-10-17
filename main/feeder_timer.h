#ifndef FEEDER_TIMER_H
#define FEEDER_TIMER_H

#define LID_OPENING_TIME_IN_MINUTES 20

void set_lid_open_time_t(time_t open_time_t);
size_t get_lid_open_time_str(char * buffer);
esp_err_t get_lid_open_time_t(time_t *time);

void set_timer(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);
void sync_buenos_aires_sntp_datetime(void);

#endif // FEEDER_TIMER_H
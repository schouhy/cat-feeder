#include "esp_log.h"
#include "esp_event.h"
#include "driver/mcpwm.h"

#include "feeder_servo_driver.h"
#include "feeder_state.h"
#include "feeder_sound.h"


#define SERVO_MIN_PULSEWIDTH_US (1000) // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US (2000) // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE        (76)   // Maximum angle in degree upto which servo can rotate

#define SERVO_PULSE_GPIO        (18)   // GPIO connects to the PWM signal line


static const char *TAG = "SERVO_DRIVER";

static inline uint32_t example_convert_servo_angle_to_duty_us(int angle)
{
    return (angle + SERVO_MAX_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (2 * SERVO_MAX_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void init_servo(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PULSE_GPIO); // To drive a RC servo, one MCPWM generator is enough

    mcpwm_config_t pwm_config = {
        .frequency = 50, // frequency = 50Hz, i.e. for every servo motor time period should be 20ms
        .cmpr_a = 0,     // duty cycle of PWMxA = 0
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, example_convert_servo_angle_to_duty_us(-SERVO_MAX_DEGREE)));
}

void lid_handler_open(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    run_sound();
    if(get_current_state() == OPENING_LID) {
        ESP_LOGI(TAG, "lid handler open called");
        for (int angle = -SERVO_MAX_DEGREE; angle < SERVO_MAX_DEGREE; angle += 2) {
            ESP_LOGI(TAG, "Angle of rotation: %d", angle);
            ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, example_convert_servo_angle_to_duty_us(angle)));
            vTaskDelay(pdMS_TO_TICKS(25)); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation under 5V power supply
        }
        system_next_state();
    }
}

void lid_handler_close(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    if(get_current_state() == CLOSING_LID) {
        ESP_LOGI(TAG, "lid handler closed called");
        for (int angle = SERVO_MAX_DEGREE; angle > -SERVO_MAX_DEGREE; angle -= 2) {
            ESP_LOGI(TAG, "Angle of rotation: %d", angle);
            ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, example_convert_servo_angle_to_duty_us(angle)));
            vTaskDelay(pdMS_TO_TICKS(25)); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation under 5V power supply
        }
        system_next_state();
    }
}

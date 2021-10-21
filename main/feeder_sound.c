/* LEDC (LED Controller) basic example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO          (16) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (0x7F) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (660) // Frequency in Hertz. Set frequency at 5 kHz

ledc_timer_config_t ledc_timer;
ledc_channel_config_t ledc_channel;

void tone(int gpio_num,uint32_t freq,uint32_t duration)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer.speed_mode       = LEDC_MODE;
    ledc_timer.bit_num          = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num        = LEDC_TIMER;
    ledc_timer.freq_hz          = freq;  // Set output frequency at 5 kHz
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel.gpio_num       = LEDC_OUTPUT_IO;
    ledc_channel.speed_mode     = LEDC_MODE;
    ledc_channel.channel        = LEDC_CHANNEL;
    ledc_channel.intr_type      = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel      = LEDC_TIMER;
    ledc_channel.duty           = 0x0; // Set duty to 0%
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(duration/portTICK_PERIOD_MS);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
}

void delay(int n)
{
	vTaskDelay(n/portTICK_PERIOD_MS);
}

void run_sound(void)
{
    tone(LEDC_OUTPUT_IO,660,100);
    delay(150);
    tone(LEDC_OUTPUT_IO,660,100);
    delay(300);
    tone(LEDC_OUTPUT_IO,660,100);
    delay(300);
    tone(LEDC_OUTPUT_IO,510,100);
    delay(100);
    tone(LEDC_OUTPUT_IO,660,100);
    delay(300);
    tone(LEDC_OUTPUT_IO,770,100);
    delay(550);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
}
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "usb_serial.h"
#include "pio.h"
#include "sys.h"
#include "delay.h"
#include "pwm.h"
#include "panic.h"
#include "target.h"
#include "pacer.h"
#include "nrf24.h"
#include "pwm_control.h"
#include "radio_control.h"
#include "usb_read.h"


static pwm_t pwm1, pwm2;

static const pwm_cfg_t pwm1_cfg = {
    .pio = H_BRIDGE_AIN1,
    .period = PWM_PERIOD_DIVISOR(PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm2_cfg = {
    .pio = H_BRIDGE_BIN1,
    .period = PWM_PERIOD_DIVISOR(PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

void pwm_control_init(void)
{

    pwm1 = pwm_init(&pwm1_cfg);
    pwm2 = pwm_init(&pwm2_cfg);
    
    pio_config_set(H_BRIDGE_AIN1, PIO_OUTPUT_LOW);
    pio_config_set(H_BRIDGE_BIN2, PIO_OUTPUT_LOW);
    
    pwm_channels_start(pwm_channel_mask(pwm1) | pwm_channel_mask(pwm2));

}

// delays in this function might need to be removed for the pacer loop
// Update the specified PWM channel
void update_pwm_duty(char channel, char direction, int duty_cycle)
{
    if (duty_cycle < 0) duty_cycle = 0;
    if (duty_cycle > 100) duty_cycle = 100;

    if (channel == 'a') {
        if (direction == 'f') {
            delay_ms(10);  // Allow H-bridge to settle
            pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        } else if (direction == 'r') {
            delay_ms(10);
            pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        }
        // printf("Motor A set to %d%% %s\n", duty_cycle, direction == 'r' ? "reverse" : "forward");
    } else if (channel == 'b') {
        if (direction == 'f') {
            delay_ms(10);
            pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        } else if (direction == 'r') {
            delay_ms(10);
            pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        }
        // printf("Motor B set to %d%% %s\n", duty_cycle, direction == 'r' ? "reverse" : "forward");
    }
}

// Prompt the user for input
void prompt_command(void)
{
    printf("Enter duty cycle (0-100): ");
    fflush(stdout);
}

// Process the user's command

void pwm_set(char channel, char direction, int duty)
{
    // Clamp duty cycle
    if (duty < 0) duty = 0;
    if (duty > 100) duty = 100;

    // Set direction and update motor configs
    if (channel == 'a') {
        if (direction == 'f') {
            pio_config_set(H_BRIDGE_AIN1, PIO_PERIPH_B);
            pio_config_set(H_BRIDGE_AIN2, PIO_OUTPUT_LOW);
        } else if (direction == 'r') {
            pio_config_set(H_BRIDGE_AIN1, PIO_OUTPUT_LOW);
            pio_config_set(H_BRIDGE_AIN2, PIO_PERIPH_A);
        }
    } else if (channel == 'b') {
        if (direction == 'f') {
            pio_config_set(H_BRIDGE_BIN1, PIO_PERIPH_B);
            pio_config_set(H_BRIDGE_BIN2, PIO_OUTPUT_LOW);
        } else if (direction == 'r') {
            pio_config_set(H_BRIDGE_BIN1, PIO_OUTPUT_LOW);
            pio_config_set(H_BRIDGE_BIN2, PIO_PERIPH_A);
        }
    }

    // Update PWM duty and return the values
    update_pwm_duty(channel, direction, duty);
}

void parse_and_set_motor(const char* input)
{
    char channel, direction;
    int duty;

    if (sscanf(input, " %c %c %d", &channel, &direction, &duty) == 3) {
        channel = tolower(channel);
        direction = tolower(direction);
        if ((channel == 'a' || channel == 'b') &&
            (direction == 'f' || direction == 'r') &&
            abs(duty) >= 0 && abs(duty) <= 100) {

            if (channel == 'a') {
                motor_direction_a = direction;
                motor_duty_a = duty;
            } 
            if (channel == 'b') {
                motor_direction_b = direction;
                motor_duty_b = duty;
            }
        }
    }
}
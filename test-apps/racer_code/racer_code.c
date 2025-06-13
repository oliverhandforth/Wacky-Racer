#include <stdio.h>
#include "usb_serial.h"
#include "pio.h"
#include "sys.h"
#include "delay.h"
#include "pwm.h"
#include "panic.h"
#include "target.h"
#include "pacer.h"

#define PACER_RATE 50  // Controls how often we check serial input
#define PWM_FREQ_HZ 30000

pwm_t pwm1, pwm2;


pwm_cfg_t pwm1_cfg = {
    .pio = H_BRIDGE_AIN1,
    .period = PWM_PERIOD_DIVISOR(PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

pwm_cfg_t pwm2_cfg = {
    .pio = H_BRIDGE_BIN1,
    .period = PWM_PERIOD_DIVISOR(PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};




// Update the specified PWM channel
void update_pwm_duty(char channel, int duty_cycle, char direction)
{
    if (duty_cycle < 0) duty_cycle = 0;
    if (duty_cycle > 100) duty_cycle = 100;

    if (channel == 'a') {
        if (direction == 'f') {
            // pwm_duty_set(pwm3, 0);  // turn off reverse first
            delay_ms(10);           // allow H-bridge to settle
            pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        } else if (direction == 'r') {
            // pwm_duty_set(pwm1, 0);  // turn off forward first
            delay_ms(10);
            pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        }
        printf("Motor A set to %d%% %s\n", duty_cycle, direction == 'r' ? "reverse" : "forward");
    } else if (channel == 'b') {
        if (direction == 'f') {
            // pwm_duty_set(pwm4, 0);
            delay_ms(10);
            pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        } else if (direction == 'r') {
            // pwm_duty_set(pwm2, 0);
            delay_ms(10);
            pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        }
        printf("Motor B set to %d%% %s\n", duty_cycle, direction == 'r' ? "reverse" : "forward");
    } else {
        printf("Invalid channel '%c'. Use 'a' or 'b'.\n", channel);
    }

    fflush(stdout);
}



// Prompt the user for input
void prompt_command(void)
{
    printf("Enter duty cycle (0-100): ");
    fflush(stdout);
}

// Process the user's command
void process_command(void)
{
    char buffer[80];
    char *str;

    str = fgets(buffer, sizeof(buffer), stdin);
    if (!str) return;

    char channel;
    int duty;
    char direction;

    if (sscanf(str, "%c %d %c", &channel, &duty, &direction) == 3)
    {
        if (duty < 0) duty = 0;
        if (duty > 100) duty = 100;

        // Set the direction for the specified channel
        if (channel == 'a') {
            if (direction == 'f') {
                pio_config_set(H_BRIDGE_AIN1, PIO_PERIPH_B);
                pio_config_set(H_BRIDGE_AIN2, PIO_OUTPUT_LOW);
                
            } else if (direction == 'r') {
                pio_config_set(H_BRIDGE_AIN1, PIO_OUTPUT_LOW);
                pio_config_set(H_BRIDGE_AIN2, PIO_PERIPH_A);
            }
            printf("Set A to %d%% %s\n", duty, direction == 'r' ? "reverse" : "forward");
        }
        else if (channel == 'b') {
            if (direction == 'r') {
                pio_config_set(H_BRIDGE_BIN1, PIO_OUTPUT_LOW);
                pio_config_set(H_BRIDGE_BIN2, PIO_PERIPH_A);
            } else if (direction == 'f') {
                pio_config_set(H_BRIDGE_BIN1, PIO_PERIPH_B);
                pio_config_set(H_BRIDGE_BIN2, PIO_OUTPUT_LOW);
            }
            printf("Set B to %d%% %s\n", duty, direction == 'r' ? "reverse" : "forward");
        }
        update_pwm_duty(channel, duty, direction);
    }
    else
    {
        printf("Invalid input. Format: a 60 f or b 40 r\n");
    }

    fflush(stdout);
    prompt_command();
}

int main(void)
{
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    mcu_jtag_disable();

    usb_serial_stdio_init();
    pacer_init(PACER_RATE);

    // Motor pin config
    pio_config_set(nSLEEP, PIO_OUTPUT_HIGH);

    pwm1 = pwm_init(&pwm1_cfg);
    if (!pwm1) panic(LED_ERROR_PIO, 1);
    
    pwm2 = pwm_init(&pwm2_cfg);
    if (!pwm2) panic(LED_ERROR_PIO, 2);
    
    
    // pio_output_set(H_BRIDGE_AIN2, PIO_PERIPH_B);
    pio_config_set(H_BRIDGE_AIN1, PIO_OUTPUT_LOW);
    pio_config_set(H_BRIDGE_BIN2, PIO_OUTPUT_LOW);

    // pio_output_set(H_BRIDGE_AIN1, PIO_OUTPUT_LOW);
    // pio_output_set(H_BRIDGE_AIN2, PIO_PERIPH_A);
    
    // pio_output_set(H_BRIDGE_BIN1, PIO_PERIPH_A);
    // pio_output_set(H_BRIDGE_BIN2, PIO_OUTPUT_LOW);
    
    pwm_channels_start(pwm_channel_mask(pwm1) | pwm_channel_mask(pwm2));

    prompt_command();

    while (1)
    {
        pacer_wait();
        process_command();
    }

    return 0;
}



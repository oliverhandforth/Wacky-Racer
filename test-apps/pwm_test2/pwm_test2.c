/* File:   pwm_test2.c
   Author: M. P. Hayes, UCECE
   Date:   15 April 2013
   Descr:  This example starts two channels simultaneously; one inverted
           with respect to the other.
*/
#include "pwm.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"
#include "target.h"



// If you are using PWM to drive a motor you will need
// to choose a lower frequency!
#define PWM_FREQ_HZ 10000

static const pwm_cfg_t pwm1_cfg =
{
    .pio = H_BRIDGE_AIN1,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm2_cfg =
{
    .pio = H_BRIDGE_BIN1,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};


int
main (void)
{
    pwm_t pwm1;
    pwm_t pwm2;

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);

    mcu_jtag_disable ();

    pwm1 = pwm_init (&pwm1_cfg);
    if (! pwm1)
        panic (LED_ERROR_PIO, 1);

    pwm2 = pwm_init (&pwm2_cfg);
    if (! pwm2)
        panic (LED_ERROR_PIO, 2);

    pwm_channels_start (pwm_channel_mask (pwm1) | pwm_channel_mask (pwm2));
    pio_config_set (nSLEEP, PIO_OUTPUT_HIGH);
    pio_config_set (H_BRIDGE_AIN2, PIO_OUTPUT_HIGH);
    pio_config_set (H_BRIDGE_BIN2, PIO_OUTPUT_HIGH);

    while (1)
    {
        delay_ms (500);
        pio_output_toggle (LED_STATUS_PIO);
    }

    return 0;
}

#include "radio.h"
#include "nrf24.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

extern nrf24_t *nrf;

void transmit_string(const char *msg)
{
    nrf24_write(nrf, msg, strlen(msg));
}

void transmit_duty_left(int duty_left, char dir_left)
{
    char msg_left[6];
    snprintf(msg_left, sizeof(msg_left), "a%c%d", dir_left, duty_left);
    transmit_string(msg_left);
}

void transmit_duty_right(int duty_right, char dir_right)
{
    char msg_right[6];
    snprintf(msg_right, sizeof(msg_right), "b%c%d", dir_right, duty_right);
    transmit_string(msg_right);
}

void transmit_duty(int duty_left, int duty_right, char dir_left, char dir_right)
{
    char msg_left[6];
    char msg_right[6];

    snprintf(msg_left, sizeof(msg_left), "a%c%d", dir_left, duty_left);
    snprintf(msg_right, sizeof(msg_right), "b%c%d", dir_right, duty_right);

    transmit_string(msg_left);
    delay_ms(10);  // Small gap to ensure receiver gets both
    transmit_string(msg_right);
}
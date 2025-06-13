#ifndef RADIO_H
#define RADIO_H

#include "nrf24.h"

// Declare nrf as extern so it can be used in other files
extern nrf24_t *nrf;

/**
 * @brief Transmits a string message via NRF24.
 * 
 * @param msg The message to transmit.
 */
void transmit_string(const char *msg);

/**
 * @brief Transmits the left motor duty cycle and direction.
 * 
 * @param duty_left Duty cycle for the left motor.
 * @param dir_left Direction for the left motor ('f' or 'r').
 */
void transmit_duty_left(int duty_left, char dir_left);

/**
 * @brief Transmits the right motor duty cycle and direction.
 * 
 * @param duty_right Duty cycle for the right motor.
 * @param dir_right Direction for the right motor ('f' or 'r').
 */
void transmit_duty_right(int duty_right, char dir_right);

/**
 * @brief Transmits both motor duty cycles and directions.
 * 
 * @param duty_left Duty cycle for the left motor.
 * @param duty_right Duty cycle for the right motor.
 * @param dir_left Direction for the left motor ('f' or 'r').
 * @param dir_right Direction for the right motor ('f' or 'r').
 */
void transmit_duty(int duty_left, int duty_right, char dir_left, char dir_right);

#endif // RADIO_H
// filepath: p:\ENCE461\Software\wacky-racers-g-31-1\src\common\led_tape.h
#ifndef LED_TAPE_H
#define LED_TAPE_H

#include "pio.h"

/**
 * @brief Initializes the LED tape.
 * 
 * @param pin The pin connected to the LED tape.
 */
void led_tape_init(pio_t pin);

/**
 * @brief Updates the LED tape (to be called in the paced loop).
 */
void led_tape_update(void);

#endif // LED_TAPE_H
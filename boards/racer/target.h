/** @file   target.h
    @author Max Kirkwood
    @date   29 April 2025
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a racer!  */

/* System clocks  */
#define F_XTAL 12.00e6
#define MCU_PLL_MUL 16
#define MCU_PLL_DIV 1

#define MCU_USB_DIV 2
/* 192 MHz  */
#define F_PLL (F_XTAL / MCU_PLL_DIV * MCU_PLL_MUL)
/* 96 MHz  */
#define F_CPU (F_PLL / 2)

/* TWI  */
#define TWI_TIMEOUT_US_DEFAULT 10000

/* USB  */
#define USB_VBUS_PIO PA31_PIO
#define USB_CURRENT_MA 500

/* LEDs  */
#define LED_ERROR_PIO PB2_PIO
#define LED_STATUS_PIO PB1_PIO
#define LED_ACTIVE 0

/* General  */  /*what are these*/
#define APPENDAGE_PIO PA1_PIO
#define SERVO_PWM_PIO PA2_PIO  

/* Buttons  */
#define BUTTON_PIO PA9_PIO


/* H-bridges   */
#define H_BRIDGE_AIN1 PB4_PIO
#define H_BRIDGE_AIN2 PA2_PIO
#define H_BRIDGE_BIN1 PA11_PIO
#define H_BRIDGE_BIN2 PA0_PIO

/* Radio  */
#define RADIO_CS_PIO PA15_PIO
#define RADIO_CE_PIO PA27_PIO
#define RADIO_IRQ_PIO PA16_PIO

/* LED tape  */
// #define LEDTAPE_PIO PA6_PIO
#define LEDTAPE_PIO PA28_PIO

/* Bumper */
#define BUMPER_PIO PA10_PIO
#define BUMPER_ACTIVE 0

#define SWCLK PB7_PIO
#define SWDIO PB6_PIO

#define nSLEEP PA4_PIO

/* Battery */
#define BATTERY_VOLTAGE_PIO PB3_PIO
#define R1 27
#define R2 18

/* Channel select header (H6) */

#define EXT1 PB0_PIO
#define EXT2 PA21_PIO
#define EXT3 PA18_PIO
#define EXT4 PB14_PIO

#endif /* TARGET_H  */

/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a hat!  */

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
#define MPU_ADDRESS 0x68

/* USB  */
#define USB_VBUS_PIO PA31_PIO
#define USB_CURRENT_MA 500

/* ADC dunno */
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_7

/* IMU  dunno */
#define IMU_INT1_PIO PB5_PIO
#define IMU_INT2_PIO PA30_PIO

/* LEDs  */
#define LED_ERROR_PIO PA0_PIO
#define LED_STATUS_PIO PA1_PIO
#define LED_ACTIVE 1

/* General  dunno */
#define APPENDAGE_PIO PA16_PIO
#define SERVO_PWM_PIO PA2_PIO

/* Button  */
#define BUTTON_PIO PA29_PIO

/* Radio  */
#define RADIO_CS_PIO PA11_PIO
#define RADIO_CE_PIO PA27_PIO
#define RADIO_IRQ_PIO PA15_PIO

/* LED tape  */
// #define LEDTAPE_PIO PA8_PIO
#define LEDTAPE_PIO PA28_PIO

/* Accelerometer */
#define ADXL345_ADDRESS 0x53

/*Battery*/
#define BATT_DETECT PB4_PIO

#define SWCLK PB7_PIO
#define SWDIO PB6_PIO

/* Buzzer */
#define PIEZO_PIO PA2_PIO

/* Battery */
#define BATTERY_VOLTAGE_PIO PB4_PIO
#define R1 27
#define R2 18

/* Channel select header (H2/H3) */

#define R_CHANNEL_1 PA20_PIO
#define R_CHANNEL_2 PA23_PIO
#define R_CHANNEL_3 PA22_PIO
#define R_CHANNEL_4 PA19_PIO

#endif /* TARGET_H  */

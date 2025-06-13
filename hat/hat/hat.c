/* File:   adxl345_test1.c
   Author: M. P. Hayes, UCECE
   Date:   3 December 2022
   Descr:  Read from an ADXL345 accelerometer and write its output to the USB serial.
*/

// latest update: working transmission and receiving, but scale is wrong.
// imu flat is 50 speed when it should read 0, so -100 to 100 is what we want to be calculated, but currently only does 0 to 100. 


#include "pio.h"
#include "delay.h"
#include "target.h"
#include "pacer.h"
#include "usb_serial.h"
#include "adxl345.h"
#include "panic.h"
#include "nrf24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piezo.h"
#include "piezo_beep.h"
#include "imu_control.h"
#include "radio.h"
#include "ledbuffer.h"
#include "adc.h"
#include "channel_select.h"
#include "sound.h"


#define PACER_RATE 200

// Task execution rates (in Hz)
#define ACCEL_POLL_RATE 20
#define RADIO_TRANSMIT_RATE 20
#define STATUS_LED_TOGGLE_RATE 4
#define RADIO_CHECK_RATE 100
#define ERROR_LED_TOGGLE_RATE 25
#define LED_TAPE_UPDATE_RATE 10
#define BATTERY_READ_RATE 1
#define CHANNEL_DETECT_RATE 10

// Derived task intervals (in ticks)
#define ACCEL_POLL_INTERVAL (PACER_RATE / ACCEL_POLL_RATE)
#define RADIO_TRANSMIT_INTERVAL (PACER_RATE / RADIO_TRANSMIT_RATE)
#define STATUS_LED_TOGGLE_INTERVAL (PACER_RATE / STATUS_LED_TOGGLE_RATE)
#define RADIO_CHECK_INTERVAL (PACER_RATE / RADIO_CHECK_RATE)
#define ERROR_LED_TOGGLE_INTERVAL (PACER_RATE / ERROR_LED_TOGGLE_RATE)
#define LED_TAPE_UPDATE_INTERVAL (PACER_RATE / LED_TAPE_UPDATE_RATE)
#define BATTERY_READ_INTERVAL (PACER_RATE / BATTERY_READ_RATE)
#define CHANNEL_DETECT_INTERVAL (PACER_RATE / CHANNEL_DETECT_RATE)

#define IDLE_DUTY_LEFT 50
#define IDLE_DUTY_RIGHT 50
#define MAX_DUTY 100
#define MIN_DUTY 0
#define MAX_ABS_IMU_VAL 260
#define DUTY_TURNING_OFFSET 50

#define RADIO_CHANNEL 15
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 32

#define NUM_LEDS 21 // 20 or 22 depending on the tape

nrf24_t *nrf;

static twi_cfg_t adxl345_twi_cfg = {
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR(100000),
    .slave_addr = 0
};

static const adc_cfg_t battery_adc_cfg = {
    .channel = ADC_CHANNEL_7,  // REPLACE WITH CORRECT CHANNEL - double check since idk if its a 
    .trigger = ADC_TRIGGER_SW,
    .clock_speed_kHz = 1000
};

int main(void)
{
    twi_t adxl345_twi;
    adxl345_t *adxl345;
    int accel_poll_ticks = 0;
    int radio_transmit_ticks = 0;
    int status_led_toggle_ticks = 0;
    int radio_check_ticks = 0;
    int error_led_toggle_ticks = 0;
    int led_tape_update_ticks = 0;
    int battery_read_ticks = 0;
    int error_LED_status = 0;
    int channel_select_ticks = 0;

    int duty_left = IDLE_DUTY_LEFT;
    int duty_right = IDLE_DUTY_RIGHT;
    char dir_left = 'f';
    char dir_right = 'f';

    piezo_cfg_t piezo_cfg = {
        .pio = PIEZO_PIO
    };

    piezo_t buzzer = piezo_init(&piezo_cfg);

    mcu_jtag_disable();

    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_LOW);
    pio_output_set(LED_ERROR_PIO, !LED_ACTIVE);
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_LOW);
    pio_output_set(LED_STATUS_PIO, !LED_ACTIVE);

    pio_config_set(R_CHANNEL_1, PIO_PULLUP);
    pio_config_set(R_CHANNEL_2, PIO_PULLUP);
    pio_config_set(R_CHANNEL_3, PIO_PULLUP);
    pio_config_set(R_CHANNEL_4, PIO_PULLUP);
    
    // Set up TWI (I2C) and accelerometer
    adxl345_twi = twi_init(&adxl345_twi_cfg);
    if (!adxl345_twi) panic(LED_ERROR_PIO, 1);

    adxl345 = adxl345_init(adxl345_twi, ADXL345_ADDRESS);
    if (!adxl345) panic(LED_ERROR_PIO, 2);

    // Set up NRF24
    static const spi_cfg_t spi_cfg = {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0,
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8
    };

    static nrf24_cfg_t nrf24_cfg = {
            .channel = RADIO_CHANNEL,
            .address = RADIO_ADDRESS,
            .payload_size = RADIO_PAYLOAD_SIZE,
            .ce_pio = RADIO_CE_PIO,
            .irq_pio = RADIO_IRQ_PIO,
            .spi = spi_cfg
        };

#ifdef RADIO_POWER_ENABLE_PIO
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    delay_ms(10);
#endif

    pacer_init(PACER_RATE);
    usb_serial_stdio_init();
    // led_tape_init(LEDTAPE_PIO); // Initialize LED tape

    printf("Starting Hat...\n");
    fflush(stdout);

    adc_t battery_adc = adc_init(&battery_adc_cfg);
    if (!battery_adc)
    {
        printf("Failed to initialize ADC for battery voltage\n");
        return 1;
    }

    bool blue = false;
    int count = 0;

    ledbuffer_t *leds = ledbuffer_init(LEDTAPE_PIO, NUM_LEDS);

    int channel = 0;
    sound_init();
    channel = determine_channel();

    while (channel == 0) {
        pacer_wait();
        channel_select_ticks++;
        if (channel_select_ticks >= CHANNEL_DETECT_INTERVAL) {
            channel = determine_channel();
            printf("Channel selected: %d\n", channel);
            fflush(stdout); // ?
            channel_select_ticks = 0;
        }
    }

    nrf24_cfg.channel = channel;
    nrf = nrf24_init(&nrf24_cfg);
    if (!nrf) panic(LED_ERROR_PIO, 3);

    while (1)
    {
        pacer_wait();
        sound_update();
        
        // Increment task counters
        accel_poll_ticks++;
        radio_transmit_ticks++;
        status_led_toggle_ticks++;
        radio_check_ticks++;
        error_led_toggle_ticks++;
        led_tape_update_ticks++;
        battery_read_ticks++;

        // Task: Toggle status LED
        if (status_led_toggle_ticks >= STATUS_LED_TOGGLE_INTERVAL)
        {
            status_led_toggle_ticks = 0;
            pio_output_toggle(LED_STATUS_PIO);
        }

        // Task: Toggle error LED
        if (error_led_toggle_ticks >= ERROR_LED_TOGGLE_INTERVAL) 
        {
            error_led_toggle_ticks = 0;

            if (error_LED_status)
            {
                pio_output_set(LED_ERROR_PIO, 1);  // Turn on error LED
            }
            else
            {
                pio_output_set(LED_ERROR_PIO, 0);  // Turn off error LED
            }
        }

        // Task: Poll accelerometer
        if (accel_poll_ticks >= ACCEL_POLL_INTERVAL)
        {
            accel_poll_ticks = 0;

            if (!adxl345_is_ready(adxl345))
            {
                // Handle accelerometer not ready
            }
            else
            {
                int16_t accel[3];
                if (adxl345_accel_read(adxl345, accel))
                {
                    convert_accel_to_duty(accel, &duty_left, &duty_right, &dir_left, &dir_right);
                }
                else
                {
                    error_LED_status = 1;  // Set error state
                }
            }
        }

        // Task: Transmit radio data
        if (radio_transmit_ticks >= RADIO_TRANSMIT_INTERVAL)
        {
            radio_transmit_ticks = 0;

            int16_t accel[3];
            if (adxl345_accel_read(adxl345, accel))
            {
                convert_accel_to_duty(accel, &duty_left, &duty_right, &dir_left, &dir_right);
                transmit_duty(duty_left, duty_right, dir_left, dir_right);
            }
        }
        // nrf24_listen();

        // Task: Check for incoming radio data
        if (radio_check_ticks >= RADIO_CHECK_INTERVAL) // need to check that there is a time delay between this and when we send data. same for racer
        {
            radio_check_ticks = 0;

            char buffer[RADIO_PAYLOAD_SIZE + 1];
            uint8_t bytes = nrf24_read(nrf, buffer, RADIO_PAYLOAD_SIZE);
            if (bytes > 0)
            {
                buffer[bytes] = '\0';
                printf("Received: %s\n", buffer);
                pio_output_toggle(LED_STATUS_PIO);
                // piezo_beep(buzzer, 2000);
                sound_play_mcdonalds();
            }
        }

        // Task: Update LED tape
        if (led_tape_update_ticks >= LED_TAPE_UPDATE_INTERVAL)
        {
            led_tape_update_ticks = 0;
            // printf("Running...\n");
            if (count++ == NUM_LEDS)
            {
                ledbuffer_clear(leds);
                if (blue)
                {
                    ledbuffer_set(leds, 0, 0, 0, 255);
                    ledbuffer_set(leds, NUM_LEDS / 2, 0, 0, 255);
                }
                else
                {
                    ledbuffer_set(leds, 0, 255, 0, 0);
                    ledbuffer_set(leds, NUM_LEDS / 2, 255, 0, 0);
                }
                blue = !blue;
                count = 0;
            }

            ledbuffer_write(leds);
            ledbuffer_advance(leds, 1);
        }

        // Task: Read battery voltage - DONT USE UNTIL PIN IS CORRECTLY CONFIGURED, REFER TO SCHEMATIC


        // if (battery_read_ticks >= BATTERY_READ_INTERVAL)
        // {
        //     battery_read_ticks = 0;

        //     uint16_t adc_value;
        //     adc_read(battery_adc, &adc_value, sizeof(adc_value));
        //     float battery_voltage = (adc_value / 4095.0) * 3.3;
        //     battery_voltage = battery_voltage * (R1 + R2) / R2;  // Adjust for voltage divider
        //     // printf("Battery Voltage: %.2f V\n", battery_voltage);

        //     if (battery_voltage < 6.6)  // Example threshold
        //     {
        //         error_LED_status = 1;  // Set error LED status
        //     }
        //     else
        //     {
        //         error_LED_status = 0;  // Clear error LED status
        //     }
        // }
    }
}

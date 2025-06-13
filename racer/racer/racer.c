#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
#include "ledbuffer.h"
#include "adc.h"
#include "sam4s.h"
#include "radio_select.h"

#define PACER_RATE 200

#define USB_POLL_RATE 5
#define RADIO_CHECK_RATE 50 //450
#define STATUS_LED_TOGGLE_RATE 2
#define BUMPER_CHECK_RATE 10 // 450
#define DISPLAY_UPDATE_RATE 20
#define ERROR_LED_TOGGLE_RATE 25
#define PWM_UPDATE_RATE 25
#define LED_TAPE_UPDATE_RATE 10
#define BATTERY_READ_RATE 1
#define BUMPER_DELAY_RATE 0.2 //     1/5
#define SLEEP_RATE 1
#define MESSAGE_RESEND_RATE 100
#define CHANNEL_DETECT_RATE 10

#define RACER_BATTERY_THRESHOLD_VOLTAGE 3.0

#define NUM_LEDS 21 // 20 or 22 depending on the tape

// Derived intervals
#define USB_POLL_INTERVAL (PACER_RATE / USB_POLL_RATE)
#define RADIO_CHECK_INTERVAL (PACER_RATE / RADIO_CHECK_RATE)
#define STATUS_LED_TOGGLE_INTERVAL (PACER_RATE / STATUS_LED_TOGGLE_RATE)
#define BUMPER_CHECK_INTERVAL (PACER_RATE / BUMPER_CHECK_RATE)
#define DISPLAY_UPDATE_INTERVAL (PACER_RATE / DISPLAY_UPDATE_RATE)
#define ERROR_LED_TOGGLE_INTERVAL (PACER_RATE / ERROR_LED_TOGGLE_RATE)
#define PWM_UPDATE_INTERVAL (PACER_RATE / PWM_UPDATE_RATE)
#define LED_TAPE_UPDATE_INTERVAL (PACER_RATE / LED_TAPE_UPDATE_RATE)
#define BATTERY_READ_INTERVAL (PACER_RATE / BATTERY_READ_RATE)
#define BUMPER_DELAY_INTERVAL (PACER_RATE / BUMPER_DELAY_RATE)
#define SLEEP_INTERVAL (PACER_RATE / SLEEP_RATE)
#define RESEND_MESSAGE_DELAY_INTERVAL (PACER_RATE / MESSAGE_RESEND_RATE)
#define CHANNEL_DETECT_INTERVAL (PACER_RATE / CHANNEL_DETECT_RATE)

char motor_direction_a = 'f';
int motor_duty_a = 0;
char motor_direction_b = 'f';
int motor_duty_b = 0;
int BUMPER_DELAY_MODE = 0;

nrf24_t *nrf;

// board currently makes high pitched noise when plugged in

// ADC configuration for the battery voltage pin
static const adc_cfg_t battery_adc_cfg = {
    .channel = ADC_CHANNEL_7,  // Replace with the correct channel for your pin
    .trigger = ADC_TRIGGER_SW,
    .clock_speed_kHz = 1000
};

int main(void)
{
    mcu_jtag_disable();
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_LOW);

    // radio_control_init();
    usb_read_init();
    pwm_control_init();
    pio_config_set(BUTTON_PIO, PIO_INPUT);
    pio_config_set(nSLEEP, PIO_OUTPUT_HIGH);
    // led_tape_init(LEDTAPE_PIO);  // Replace `LEDTAPE_PIO` with the actual pin used for the LED tape

    pio_config_set(EXT1, PIO_PULLUP);
    pio_config_set(EXT2, PIO_PULLUP);
    pio_config_set(EXT3, PIO_PULLUP);
    pio_config_set(EXT4, PIO_PULLUP);

    printf("Starting Racer...\n");

    pacer_init(PACER_RATE);

    // Initialize ADC for battery voltage
    adc_t battery_adc = adc_init(&battery_adc_cfg);
    if (!battery_adc)
    {
        printf("Failed to initialize ADC for battery voltage\n");
        return 1;
    }

    int usb_poll_ticks = 0;
    int radio_check_ticks = 0;
    int status_led_toggle_ticks = 0;
    int bumper_check_ticks = 0;
    int display_update_ticks = 0;
    int error_led_toggle_ticks = 0;
    int pwm_update_ticks = 0;
    int led_tape_update_ticks = 0;
    int battery_read_ticks = 0;
    int bumper_delay_ticks = 0;
    int sleep_ticks = 0;
    int resend_message_delay_ticks = 0;
    int channel_select_ticks = 0;

    int motor_a_duty = 0, motor_b_duty = 0;
    char motor_a_direction = 'f', motor_b_direction = 'f';

    int error_LED_status = 0;
    int MESSAGE_RECEIVED = 1;  // Flag to indicate if a message has been received by the hat


    bool usb_command_mode = false;  // Set to true if in USB command mode
    bool button_last_state = false;  // Last sampled state
    


    bool blue = false;
    int count = 0;

    ledbuffer_t *leds = ledbuffer_init(LEDTAPE_PIO, NUM_LEDS);

    int channel = 0;
    //sound_init();
    channel = determine_channel_1();

    while (channel == 0) {
        pacer_wait();
        channel_select_ticks++;
        if (channel_select_ticks >= CHANNEL_DETECT_INTERVAL) {
            int channel = determine_channel_1();
            printf("Channel selected: %d\n", channel);
            fflush(stdout); // ?
            channel_select_ticks = 0;
        }
    } 

    radio_control_init(channel);

    while (1)
    {
        pacer_wait();

        // Increment task counters
        usb_poll_ticks++;
        radio_check_ticks++;
        status_led_toggle_ticks++;
        bumper_check_ticks++;
        display_update_ticks++;
        error_led_toggle_ticks++;
        pwm_update_ticks++;
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

        // Task: Check bumper - probably need to keep entering this loop as long as we dont receive confirmation that the message has been received by the hat.
        // Additionally, need to incoporate a delay between the two messages so that we dont spam the radio with messages
        // Also need to create debouncing logic AND ensure that the message gets sent went the bumber is pressed (rising edge), rather than when it is released (falling edge)
        if (bumper_check_ticks >= BUMPER_CHECK_INTERVAL)
        {
            bumper_check_ticks = 0;

            if ((pio_input_get(BUMPER_PIO) == BUMPER_ACTIVE) && !BUMPER_DELAY_MODE) // debouncing logic needed
            {
                MESSAGE_RECEIVED = send_buzzer_signal();  // Update error_LED_status based on bumper signal
                if (!MESSAGE_RECEIVED)
                {
                    // Set the error LED status to indicate a message was sent
                    error_LED_status = 1;
                }
                BUMPER_DELAY_MODE = 1;
                printf("Delay started\n");
            }
        }

        // Task: Poll USB commands or check radio commands
        if (usb_command_mode)
        {
            if (usb_poll_ticks >= USB_POLL_INTERVAL)
            {
                usb_poll_ticks = 0;
                char *usb_cmd = usb_loop();
                if (usb_cmd)
                {
                    parse_and_set_motor(usb_cmd);
                    pwm_set('a', motor_direction_a, motor_duty_a);
                    pwm_set('b', motor_direction_b, motor_duty_b);
                    free(usb_cmd);
                }
            }
        }
        else
        {
            if (radio_check_ticks >= RADIO_CHECK_INTERVAL)
            {
                radio_check_ticks = 0;
                char *radio_data = radio_loop();
                if (radio_data)
                {
                    process_radio_buffer(radio_data); // need to remove the call to pwm_set so that the pwm update task actually does its job
                    free(radio_data);
                }
            }
        }

        // Task: Update PWM
        if (pwm_update_ticks >= PWM_UPDATE_INTERVAL)
        {
            if (BUMPER_DELAY_MODE)
            {
                motor_duty_a = 0;
                motor_duty_b = 0;
            }
            pwm_update_ticks = 0;

            // if ((motor_direction_a == 'f') && (motor_direction_b == 'r'))
            // {
            //     motor_direction_a = 'r';  // Reverse motor A
            //     motor_direction_b = 'f';  // Forward motor B
            // }
            // else if ((motor_direction_a == 'r') && (motor_direction_b == 'f'))
            // {
            //     motor_direction_a = 'f';  // Reverse motor A
            //     motor_direction_b = 'r';  // Forward motor B
            // }

            pwm_set('a', motor_direction_a, motor_duty_a);
            pwm_set('b', motor_direction_b, motor_duty_b);
        }

        // Task: Update display
        if (display_update_ticks >= DISPLAY_UPDATE_INTERVAL)
        {
            display_update_ticks = 0;
            printf("Motor A: %d%% %s\n", motor_duty_a, motor_direction_a == 'r' ? "reverse" : "forward");
            printf("Motor B: %d%% %s\n", motor_duty_b, motor_direction_b == 'r' ? "reverse" : "forward");
            // printf("Battery Voltage: %.2f V\n", battery_voltage);
        }

        // Task: Update LED tape
        if (led_tape_update_ticks >= LED_TAPE_UPDATE_INTERVAL)
        {
            led_tape_update_ticks = 0;

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

        // Task: Read battery voltage
        // if (battery_read_ticks >= BATTERY_READ_INTERVAL)
        // {
        //     battery_read_ticks = 0;

        //     uint16_t adc_value;
        //     adc_read(battery_adc, &adc_value, sizeof(adc_value));
        //     float battery_voltage = (adc_value / 4095.0) * 3.3;
        //     battery_voltage = battery_voltage * (R1 + R2) / R2;  // Adjust for voltage divider
        //     // printf("Battery Voltage: %.2f V\n", battery_voltage);
        //     if (battery_voltage < RACER_BATTERY_THRESHOLD_VOLTAGE)  // Example threshold
        //     {
        //         error_LED_status = 1;  // Set error LED status
        //     }
        //     else
        //     {
        //         error_LED_status = 0;  // Clear error LED status
        //     }
        // }

        // Task: Delay the motors if the bumper is pressed - holding the bumber means that we never get to 
        if (BUMPER_DELAY_MODE)
        {
            if (bumper_delay_ticks >= BUMPER_DELAY_INTERVAL){
                BUMPER_DELAY_MODE = 0;
                bumper_delay_ticks = 0;
                printf("Delay finished\n");
            } 
            bumper_delay_ticks ++;
        }

        if (!MESSAGE_RECEIVED)
        {
            if (resend_message_delay_ticks >= RESEND_MESSAGE_DELAY_INTERVAL){
                printf("Resending message\n");
                MESSAGE_RECEIVED = send_buzzer_signal();
                if (MESSAGE_RECEIVED)
                {
                    resend_message_delay_ticks = 0;
                    printf("Message received\n");
                    error_LED_status = 0;
                }
            } 
            resend_message_delay_ticks ++;
        }
    }

    return 0;
}



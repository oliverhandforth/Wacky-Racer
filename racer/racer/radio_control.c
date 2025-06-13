#include <stdio.h>
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
#include <stdlib.h>
#include "usb_read.h"
#include <string.h>
#include <ctype.h>


static const spi_cfg_t spi_cfg =
            {
                .channel = 0,
                .clock_speed_kHz = 1000,
                .cs = RADIO_CS_PIO,
                .mode = SPI_MODE_0,
                .cs_mode = SPI_CS_MODE_FRAME,
                .bits = 8
            };
static nrf24_cfg_t nrf24_cfg = // const removed
            {
                .channel = RADIO_CHANNEL,
                .address = RADIO_ADDRESS,
                .payload_size = RADIO_PAYLOAD_SIZE,
                .ce_pio = RADIO_CE_PIO,
                // If the IRQ pin is not connected, use a value of 0.
                // .irq_pio = 0,
                .irq_pio = RADIO_IRQ_PIO,
                .spi = spi_cfg,
            };

nrf24_t *nrf;

void radio_control_init(int channel)
{
    // Initialize NRF24 radio with the specified channel
    nrf24_cfg.channel = channel;
    nrf = nrf24_init (&nrf24_cfg);
    if (! nrf)
        panic (LED_ERROR_PIO, 2);
}

char* radio_loop(void)
{
    char temp_buffer[RADIO_PAYLOAD_SIZE + 1];  // Temporary stack buffer
    uint8_t bytes;

    bytes = nrf24_read(nrf, temp_buffer, RADIO_PAYLOAD_SIZE);

    if (bytes == 0) {
        return NULL;  // No data
    }

    temp_buffer[bytes] = '\0';  // Null-terminate

    char *buffer = malloc(bytes + 1);
    if (!buffer) {
        return NULL;
    }

    memcpy(buffer, temp_buffer, bytes + 1);  // Copy including '\0'

    //printf("%s\n", buffer);
    // pio_output_toggle(LED_STATUS_PIO);

    return buffer;
}

void process_radio_buffer(const char* buffer)
{
    const char* ptr = buffer;
    char cmdA[6] = {0}, cmdB[6] = {0};

    while (*ptr) {
        if ((ptr[0] == 'a' || ptr[0] == 'b') &&
            (ptr[1] == 'f' || ptr[1] == 'r') &&
            isdigit((unsigned char)ptr[2])) {

            char cmd[6] = {0};
            int i = 0;
            cmd[i++] = ptr[0];
            cmd[i++] = ptr[1];
            int j = 2;
            while (isdigit((unsigned char)ptr[j]) && i < 5) {
                cmd[i++] = ptr[j++];
            }
            cmd[i] = '\0';

            if (cmd[0] == 'a') {
                strcpy(cmdA, cmd);
            } else if (cmd[0] == 'b') {
                strcpy(cmdB, cmd);
            }

            ptr += j;
        } else {
            ptr++;
        }
    }

    if (cmdA[0]) parse_and_set_motor(cmdA);
    if (cmdB[0]) parse_and_set_motor(cmdB);
}

int send_buzzer_signal(void)
{
    char buffer[RADIO_PAYLOAD_SIZE + 1];
    const char *msg = "Bumper Hit!\n";
    // delay_ms(10);
    // if (!nrf24_write(nrf, msg, strlen(msg))) {
    //     printf("Sent buzzer");
    //     return 1;
    // } else {
    //     return 0;
    // }
    if (nrf24_write(nrf, msg, strlen(msg))) {
        printf("Sent buzzer\n");
        return 1;
    } else {
        printf("Failed to send buzzer\n");
        return 0;
    }
}
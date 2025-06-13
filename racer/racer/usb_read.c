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
#include "usb_read.h"

void usb_read_init(void)
{
    usb_serial_stdio_init();
    printf("USB Serial Initialized\n");
    fflush(stdout);
}

char* usb_loop(void)
{
    pacer_wait(); // remove this
    char *buffer = malloc(80);
    if (!buffer) return NULL;
    
    if (fgets(buffer, 80, stdin) == NULL) {
        free(buffer);
        return NULL;
    }

    return buffer;
}
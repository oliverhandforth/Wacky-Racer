#include "led_tape.h"
#include "ledbuffer.h"
#include "pacer.h"

#define NUM_LEDS 22

static ledbuffer_t *leds = NULL;
static bool blue = false;
static int count = 0;

void led_tape_init(pio_t pin)
{
    leds = ledbuffer_init(pin, NUM_LEDS);
    if (!leds)
    {
        // Handle initialization failure
    }
}

void led_tape_update(void)
{
    if (!leds)
        return;

    if (count++ == NUM_LEDS)
    {
        // Wait for a revolution
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
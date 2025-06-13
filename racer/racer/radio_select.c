#include "radio_select.h"
#include "target.h"
#include "pio.h"

__attribute__((optimize (0)))
int determine_channel_1(void) {
    int channel_number = 0;

    if (!pio_input_get(EXT1)) {
        channel_number = 77;
    }
    else if (!pio_input_get(EXT2)) {
        channel_number = 27;
    }
    else if (!pio_input_get(EXT3)) {
        channel_number = 35;
    }
    else if (!pio_input_get(EXT4)) {
        channel_number = 16;
    }

    return channel_number;
}
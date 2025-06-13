#include "channel_select.h"
#include "target.h"
#include "pio.h"


int determine_channel(void) {
    int channel_number = 0;

    if (!pio_input_get(R_CHANNEL_1)) {
        channel_number = 77;
    }
    else if (!pio_input_get(R_CHANNEL_2)) {
        channel_number = 27;
    }
    else if (!pio_input_get(R_CHANNEL_3)) {
        channel_number = 35 ;
    }
    else if (!pio_input_get(R_CHANNEL_4)) {
        channel_number = 16;
    }

    return channel_number;
}
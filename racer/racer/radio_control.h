#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include <stdint.h>

#define RADIO_CHANNEL 15
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 32

// #define RADIO_CHANNEL 5
// #define RADIO_ADDRESS 0x0123456789LL
// #define RADIO_PAYLOAD_SIZE 32

// Initializes the radio module
void radio_control_init(int channel);

// Main loop for receiving radio messages
char* radio_loop(void);

void process_radio_buffer(const char* buffer);
int send_buzzer_signal(void);

#endif // RADIO_CONTROL_H

#ifndef USB_READ_H
#define USB_READ_H

#include <stdlib.h>

/**
 * @brief Initializes the USB serial interface.
 */
void usb_read_init(void);

/**
 * @brief Waits for and reads a line of input from USB serial.
 * 
 * @return Pointer to a dynamically allocated string containing the input.
 *         The caller is responsible for freeing the memory.
 */
char* usb_loop(void);

#endif // USB_READ_H

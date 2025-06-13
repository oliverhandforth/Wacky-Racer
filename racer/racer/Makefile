# filepath: p:\ENCE461\Software\wacky-racers-g-31-1\src\apps\racer\Makefile

PERIPHERALS = adc pwm
DRIVERS = pacer usb_serial panic nrf24 ledtape
SRC = racer.c \
	  pwm_control.c \
	  usb_read.c \
	  radio_control.c \
	  radio_select.c

# Use low level of optimisation to simplify debugging
OPT = -Os

BOARD = racer

# Add the include path for ledbuffer.h
CFLAGS += -Ip:\ENCE461\Software\wacky-racers-g-31-1\src\mmculib\ledbuffer

include ../../boards/board.mk

PERIPHERALS = pit twi pwm adc
DRIVERS = pacer usb_serial adxl345 panic nrf24 piezo ring ledtape
SRC = hat.c \
      radio.c \
      imu_control.c \
      channel_select.c \
      mmelody.c \
      sound.c
OPT = -O1

BOARD = hat

# Add the include path for ledbuffer.h
CFLAGS += -Ip:\ENCE461\Software\wacky-racers-g-31-1\src\mmculib\ledbuffer

include ../../boards/board.mk


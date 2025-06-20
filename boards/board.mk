# Note, the default board can be specified on the fly using an
# environment variable.  For example:
# BOARD=tank make

TARGET ?= app.elf
BUILD_DIR ?= build

RUN_MODE = ROM

ifndef OPT
OPT = -O0
endif

MMCULIB_DIR = ../../mmculib

ifndef BOARD
$(error BOARD not defined.  This needs to be explicitly set in the Makefile or defined as an environment variable.  For example, BOARD=motor make.  The value needs to be defined as one of the sub-directories in the boards directory.  In these directories are a file (target.h) for defining the crystal frequency, PLL parameters, and PIO pins, and (board.mk) for defining the MCU type and compilation options)
endif

include ../../boards/$(BOARD)/board.mk
include $(MMCULIB_DIR)/mmculib.mk

# # Add the new LED tape source file
# SRC += src/common/led_tape.c

# # Add the ledbuffer module if not already included
# SRC += src/mmculib/ledbuffer/ledbuffer.c

# CFLAGS += -Isrc/common
# CFLAGS += -Isrc/mmculib

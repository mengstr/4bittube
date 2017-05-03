#
# Makefile for the blink demo project by mats.engstrom@gmail.com
#

# Change these to match your setup
ESPPORT     ?= /dev/ttyUSB0
ESP8266SDK  ?= /opt/esp-open-sdk
ESPTOOL     ?= /opt/esptool/esptool.py

# Probably no need to change this
FLASHPARAM  =  --flash_freq 80m --flash_mode dout
FLASHBAUD   =  921600

# Name of the main source and also the name of firmwares
TARGET      =  4bittube

FW0         =  $(TARGET).elf-0x00000.bin
FW1         =  $(TARGET).elf-0x10000.bin

BINS        =  $(ESP8266SDK)/xtensa-lx106-elf/bin
GCC	        =  $(BINS)/xtensa-lx106-elf-gcc
ESPFW       =  $(ESP8266SDK)/sdk/bin

CFLAGS      =  -Wall -DICACHE_FLASH -Iinclude -I./ -mlongcalls
LDLIBS      =  -nostdlib -Wl,--start-group -lmain -lupgrade -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lcirom -lgcc
LDFLAGS     =  -Teagle.app.v6.ld

.PHONY: all flash flashinit clean

# Remove "flash" from here is you don't want to auto-flash
# after a successful compilation. Then you need to do a
# manual "make flash" to upload
all : $(TARGET).elf $(FW1) $(FW2) flash

# List of all required object modules. (I.E all your sources
# with .c replaced with .o)
OBJS = main.o 4bittube.o

# Specify which files each object depends on so it can be
# automatically recompiled if any dependency changes.
main.o: main.c 4bittube.h
4bittube.o: 4bittube.c

# The lines below don't need to be changed, but the @ at the
# start of the lines can be removed to show the commands
# being executed.  You can also remove the > /dev/null to
# show additional output from the running command.

$(TARGET).elf: $(OBJS)
	@echo "[Linking]"
	@$(GCC) $(OBJS) -o $@ $(LDLIBS) $(LDFLAGS)

$(OBJS):
	@echo "[Compiling $<]"
	@$(GCC) -c $< -o $@ $(CFLAGS)

$(FW1) $(FW2): $(TARGET).elf
	@echo "[Converting .elf to .bin]"
	@$(ESPTOOL) elf2image $^ > /dev/null

flash : $(FW0) $(FW1)
	@echo "[Flashing firmware]"
	@$(ESPTOOL) -p $(ESPPORT) -b $(FLASHBAUD) write_flash \
		0x00000 $(FW0) \
		0x10000 $(FW1) \
		$(FLASHPARAM) > /dev/null

# This might be required to be run once on a brand new
# ESP8266 unit. "make flashinit"
flashinit:
	@echo "[Erasing & initializing ESP flash]"
	@$(ESPTOOL) -p $(ESPPORT) -b $(FLASHBAUD) erase_flash
	@$(ESPTOOL) -p $(ESPPORT) -b $(FLASHBAUD) write_flash \
		0x3fc000 $(ESPFW)/esp_init_data_default.bin \
		0x3fe000 $(ESPFW)/blank.bin \
		$(FLASHPARAM) > /dev/null

# Do a "make clean" to remove anyting created by the makefile
# and compilers so the folder is clean and nice again.
clean :
	@echo "[Cleaning up folder]"
	@rm -f $(FW0) $(FW1) $(TARGET).elf *.o *.bak *.log *~

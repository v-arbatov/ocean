
ifndef ESP_OPEN_SDK_DIR
    $(error ESP_OPEN_SDK_DIR is undefined. This variable should point at esp-open-sdk directory)
endif

#
# CONFIGURATION START
#

ADDR0=0x00000
ADDR1=0x10000
FLASH_PORT=/dev/ttyUSB0

#
# CONFIGURATION END
#

CC = xtensa-lx106-elf-gcc

CFLAGS = -O2 -I. -mlongcalls -I$(ESP_OPEN_SDK_DIR)/sdk/include -I$(ESP_OPEN_SDK_DIR)/sdk/driver_lib/include/driver

LDLIBS = -nostdlib -Wl,--start-group -ldriver -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lgcc -lc

LDFLAGS = -L$(ESP_OPEN_SDK_DIR)/sdk/lib -L$(ESP_OPEN_SDK_DIR)/sdk/ld -Teagle.app.v6.ld

.PHONY: flash clean

ocean-$(ADDR0).bin: ocean
	esptool.py elf2image $^

ocean: main.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

main.o: main.c

flash: ocean-$(ADDR0).bin
	esptool.py --port $(FLASH_PORT) write_flash $(ADDR0) ocean-$(ADDR0).bin $(ADDR1) ocean-$(ADDR1).bin

clean:
	rm -f ocean main.o ocean-$(ADDR0).bin ocean-$(ADDR1).bin

prog=main
chip=attiny13
cpu_freq=
programmer=buspirate
port=/dev/buspirate
# programmer=usbtiny
# port=usb
CC=avr-gcc

CFLAGS  = -funsigned-char -funsigned-bitfields 
CFLAGS += -fpack-struct -fshort-enums 
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fwhole-program
CFLAGS += -Wall -Wstrict-prototypes

LDFLAGS  = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections

# DEPS = mcp23x08.h
DEPS = 
OBJ = $(prog).o $(DEPS:.h=.o)

all: $(prog).eep $(prog).lss $(prog).sym 

%.o: %.c $(DEPS)
	@echo "== Compiling object file: $@" && \
	$(CC) -c -mdeb -mmcu=$(chip) -I. -gdwarf-2 $(cpu_freq) -Os $(CFLAGS) -Wa,-adhlns=./$@.lst -std=gnu99 $< -o $@ || true

$(prog).eep: $(prog).elf
	@echo "== Creating load file for EEPROM" && \
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@

$(prog).lss: $(prog).elf
	@echo "== Creating Extended Listing" && \
	avr-objdump -h -S -z $< > $@

$(prog).sym: $(prog).elf
	@echo "== Creating Symbol Table" && \
	avr-nm -n $< > $@

$(prog).elf: $(prog).hex
	@echo "== Creating load file for Flash" && \
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $@ $^

$(prog).hex: $(OBJ)
	@echo "== Creating hex" && \
	$(CC) -mmcu=$(chip) -I. -mdeb -gdwarf-2 $(cpu_freq) -Os $(CFLAGS) -Wa,-adhlns=$<  -std=gnu99 -MMD -MP -MF .dep/$(prog).elf.d $^ --output $(prog).elf $(LDFLAGS) -lm

upload: all
	@echo "== Uploading to chip"
	sudo avrdude -P $(port) -c $(programmer) -p $(chip) -U flash:w:$(prog).hex

fuses:
	@echo "== Setting fuses"
	sudo avrdude -P $(port) -c $(programmer) -p $(chip) -U hfuse:w:0x1f:m -U lfuse:w:0x49:m
	# Set for 4.8 Mhz clock and have watchdog timer on at all times

fuses-default:
	@echo "== Setting default fuses"
	sudo avrdude -P $(port) -c $(programmer) -p $(chip) -U hfuse:w:0x1f:m -U lfuse:w:0x6a:m

size:
	@echo "Size of binary: "
	avr-size $(prog).hex

clean:
	rm -rf *.{eep,elf,hex,lss,lst,map,o,sym}

.PHONY: all upload clean fuses fuses-default size

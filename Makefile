prog=main
chip=attiny13a
cpu_freq=
programmer=usbtiny
port=usb
CC=avr-gcc
CFLAGS=-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref

#CFLAGS += -s
#CFLAGS += -mcall-prologues
#LDFLAGS += -Wl,--relax
#LDFLAGS += -Wl,--gc-sections
#CFLAGS += --param inline-call-cost=2
#CFLAGS += --combine -fwhole-program
#CFLAGS += -fno-inline-small-functions
#CFLAGS += -ffunction-sections
#CFLAGS += -fdata-sections

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

clean:
	rm -rf *.{eep,elf,hex,lss,lst,map,o,sym}

.PHONY: all upload clean

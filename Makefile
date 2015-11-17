prog=main
chip=attiny13
cpu_freq=
programmer=usbtiny
port=usb
CC=avr-gcc

DEPS = 
OBJ = $(prog).o $(DEPS:.h=.o)

all: $(prog).eep $(prog).lss $(prog).sym 

%.o: %.c $(DEPS)
	@echo "== Compiling object file: $@" && \
	$(CC) -c -mdeb -mmcu=$(chip) -I. -gdwarf-2 $(cpu_freq) -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=./$@.lst -std=gnu99 $< -o $@ || true

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
	$(CC) -mmcu=$(chip) -I. -mdeb -gdwarf-2 $(cpu_freq) -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=$<  -std=gnu99 -MMD -MP -MF .dep/$(prog).elf.d $^ --output $(prog).elf -Wl,-Map=$(prog).map,--cref -lm

upload: all
	@echo "== Uploading to chip"
	sudo avrdude -p $(chip) -P $(port) -c $(programmer) -U flash:w:$(prog).hex

clean:
	rm -rf *.{eep,elf,hex,lss,lst,map,o,sym}

.PHONY: all upload clean

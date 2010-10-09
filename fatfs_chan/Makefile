TARGET         = avr_mmc
CSRC           = main.c uart.c ff.c mmc.c rtc.c cc932_avr.c
ASRC           = xitoa.S
MCU_TARGET     = atmega644p
OPTIMIZE       = -Os -mcall-prologues
DEFS           =
LIBS           =
DEBUG          = dwarf-2
PORT           = COM13
UPLOAD_RATE    = 57600

CC             = avr-gcc
ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS)
CFLAGS         = -Wall -g$(DEBUG) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
LDFLAGS        = -Wl,-Map,$(TARGET).map
OBJ            = $(CSRC:.c=.o) $(ASRC:.S=.o)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size

# Programming support using avrdude. Settings and variables.
AVRDUDE        = avrdude
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT   = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS  = -e \
-p $(MCU_TARGET) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
-b $(UPLOAD_RATE)

all: $(TARGET).elf lst text size

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Program the device.  
upload: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
  
clean:
	rm -rf *.o $(TARGET).elf *.eps *.bak *.a $(CSRC:.c=.d)
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf $(TARGET).hex

size: $(TARGET).elf
	$(SIZE) -C --mcu=$(MCU_TARGET) $(TARGET).elf

lst:  $(TARGET).lst
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



text: hex
hex:  $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# Automatic dependencies
%.d: %.c
	$(CC) -M $(CFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@
  
include $(CSRC:.c=.d)

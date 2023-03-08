# Makefile for programming the Arduino UNO with Win-AVR 
# Eric Hansen and Petra Bonfert-Taylor, Engs 28
# Based on demo makefile at www.obdev.at/products/crosspack/index.html, with 
# help from Matt Dailey of Thayer School Computing Services

# MAIN is the name of the top-level program in your project
#MAIN	   = blinky
MAIN	   = blinky_timer


# SOURCES is the name of all the source files in your project (including $(MAIN))
SOURCES    = $(MAIN).c

# For MacOS & Linux:
# Update this line with the correct number for /dev/tty.usbmodem* for your setup
# (type ls /dev/tty.usbmodem*).
# This will likely change every time you plug the Arduino into your computer.
PROGRAMMER = -c arduino -b 115200 -P com3

# For Windows:
# Use the Device Manager to identify the correct COM port for your Arduino.
# This might change every time you plug the Arduino into your computer.
# PROGRAMMER = -c arduino -b 115200 -P COM3

# Shouldn't have to change these unless you decide to target a different Arduino
# than the UNO.
DEVICE     = atmega328p
CLOCK      = 16000000
OBJECTS    = $(MAIN).o
FUSES      = -U hfuse:w:0xde:m -U lfuse:w:0xff:m -U efuse:w:0x05:m

# Tune the lines below only if you know what you are doing:
# -std=c99 imposes C99 language standard, as recommended by Barr and JPL for embedded; 
#  default is -std=gnu11 (gnu C 2011)
# -Wall enables all warnings about constructions that some users consider questionable
#  and are easily avoided 
# -O optimizes for code size and execution time without increasing compilation time; 
# -Os optimizes only for code size
# -DF_CPU passes the clock speed down to units that need it, e.g., delay functions
CFLAGS = -std=c99 -Wall -O -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
COMPILE = avr-gcc $(CFLAGS)
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

# symbolic targets:
all:	$(MAIN).hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(MAIN).hex:i

fuse:
	$(AVRDUDE) $(FUSES)

## Xcode uses the Makefile targets "", "clean" and "install"
#install: flash fuse

## if you use a bootloader, change the command below appropriately:
#load: all
#	bootloadHID main.hex

clean:
	rm -f $(MAIN).hex $(MAIN).elf $(OBJECTS)

# file targets:
$(MAIN).o: $(SOURCES)
	$(COMPILE) -c $(SOURCES) -o $(MAIN).o
$(MAIN).elf: $(OBJECTS)
	$(COMPILE) -o $(MAIN).elf $(OBJECTS)

$(MAIN).hex: $(MAIN).elf
	rm -f $(MAIN).hex
	avr-objcopy -j .text -j .data -O ihex $(MAIN).elf $(MAIN).hex
	avr-size --format=avr --mcu=$(DEVICE) $(MAIN).elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	$(MAIN).elf
	avr-objdump -d $(MAIN).elf

cpp:
	$(COMPILE) -E $(SOURCES)

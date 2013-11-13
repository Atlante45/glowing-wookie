CC=avr-gcc -mmcu=atmega8 -DF_CPU=16000000
OC=avr-objcopy -O binary
OD=avr-objdump -d

all: twinkle.bin 

twinkle.bin: twinkle.out
	$(OC) $< $@

twinkle.out: twinkle.c
	$(CC) $^ -o $@ 

dump: twinkle.out
	$(OD) $<
send: twinkle.bin
	 avrdude -p m8 -c arduino -P /dev/ttyUSB0 -b 19200 -U flash:w:$<

clean:
	rm *.out *.bin

.PHONY: clean dump

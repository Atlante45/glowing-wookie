CC=avr-gcc -mmcu=atmega8 -DF_CPU=16000000
OC=avr-objcopy -O binary
OD=avr-objdump -d

all: test.bin 

test.bin: test.out
	$(OC) $< $@

test.out: test.c
	$(CC) $^ -o $@ 

dump: test.out
	$(OD) $<
send: test.bin
	 avrdude -p m8 -c arduino -P /dev/ttyUSB0 -b 19200 -U flash:w:$<

clean:
	rm *.out *.bin

.PHONY: clean dump

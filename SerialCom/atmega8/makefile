CC=avr-gcc -mmcu=atmega8 -DF_CPU=16000000
OC=avr-objcopy -O binary
OD=avr-objdump -d

all: serialCom.bin 

serialCom.bin: serialCom.out
	$(OC) $< $@

serialCom.out: serialCom.c protocol.o communication.o protocol.h communication.h pinAction.h

	$(CC) $^ -o $@ 

protocol.o: protocol.c protocol.h
	$(CC) -o $@ -c $<

communication.o: communication.c communication.h
	$(CC) -o $@ -c $<

pinAction.o: pinAction.c pinAction.h
	$(CC) -o $@ -c $<

dump: serialCom.out
	$(OD) $<
send: serialCom.bin
	 avrdude -p m8 -c arduino -P /dev/ttyUSB0 -b 19200 -U flash:w:$<

clean:
	rm *.o *.out *.bin

.PHONY: clean dump

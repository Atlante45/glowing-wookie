CFLAGS=
CC=

CC+=@echo -n " Compiling " $@ "..." ; 
TRACE=@echo " Done."

ifeq ($(DEBUG),1)
CC+=gcc
MOCK=_mock
CFLAGS+= -D DEBUG
else
CC+=avr-gcc -mmcu=atmega8 -DF_CPU=16000000 -Wno-cpp
MOCK=
CFLAGS+=-D DISABLE_EXTRA_FUNCTIONS
endif
CC+=$(CFLAGS)

OC=avr-objcopy -O binary
OD=avr-objdump -d


OBJ=build
COMMON=../common

ifeq ($(DEBUG),1)
all:	$(OBJ)/serialCom.out
else
all:	$(OBJ)/serialCom.bin
endif

ifeq ($(DEBUG),1)
run:	all
	$(OBJ)/serialCom.out
endif

$(OBJ)/bits.o: $(COMMON)/bits.c $(COMMON)/bits.h
	$(CC) -c $< -o $@
	$(TRACE)

$(OBJ)/checksum.o: $(COMMON)/checksum.c $(COMMON)/checksum.h
	$(CC) -c $< -o $@
	$(TRACE)

$(OBJ)/mask.o: $(COMMON)/mask.c $(COMMON)/mask.h
	$(CC) -c $< -o $@
	$(TRACE)

$(OBJ)/protocol_util.o: $(COMMON)/protocol_util.c $(COMMON)/protocol_util.h
	$(CC) -c $< -o $@
	$(TRACE)


$(OBJ)/serialCom.bin: $(OBJ)/serialCom.out
	$(OC) $< $@
	$(TRACE)


$(OBJ)/protocol.o: protocol.c protocol.h
	$(CC) -o $@ -c $<
	$(TRACE)

$(OBJ)/communication.o: communication$(MOCK).c communication.h
	$(CC) -o $@ -c $<
	$(TRACE)

$(OBJ)/pinAction.o: pinAction$(MOCK).c pinAction.h
	$(CC) -o $@ -c $<
	$(TRACE)


$(OBJ)/serialCom.out: serialCom.c protocol.h communication.h pinAction.h $(OBJ)/bits.o $(OBJ)/mask.o $(OBJ)/protocol.o $(OBJ)/communication.o  $(OBJ)/pinAction.o $(OBJ)/protocol_util.o $(OBJ)/checksum.o
	$(CC) $^ -o $@ 
	$(TRACE)

dump: $(OBJ)/serialCom.out
	$(OD) $<


send: $(OBJ)/serialCom.bin
	 avrdude -p m8 -c arduino -P /dev/ttyUSB0 -b 19200 -U flash:w:$<


clean:
	@rm -f $(OBJ)/*.o $(OBJ)/*.out $(OBJ)/*.bin
	@echo "Cleaned."

.PHONY: clean dump

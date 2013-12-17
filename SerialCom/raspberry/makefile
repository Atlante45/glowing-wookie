CC=
CC=@echo -n " Compiling " $@ "..." ;
CC+=g++
TRACE=@echo " Done."
CFLAGS=-g

ifeq ($(DEBUG),1)
MOCK=_mock
else
MOCK=
endif


OBJ=build
COMMON=../common

BIN=main

all:	$(OBJ)/$(BIN)

run:	$(OBJ)/$(BIN)
	$(OBJ)/$(BIN)

$(OBJ)/bits.o: $(COMMON)/bits.c $(COMMON)/bits.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/mask.o: $(COMMON)/mask.c $(COMMON)/mask.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/serialib.o: serialib$(MOCK).cpp serialib.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/protocol.o: protocol.cpp protocol.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/checksum.o: $(COMMON)/checksum.c $(COMMON)/checksum.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/protocol_util.o: $(COMMON)/protocol_util.c $(COMMON)/protocol_util.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(TRACE)

$(OBJ)/$(BIN):	  $(OBJ)/protocol.o $(OBJ)/serialib.o $(OBJ)/mask.o $(OBJ)/bits.o $(OBJ)/checksum.o $(OBJ)/protocol_util.o
	$(CC) $(CFLAGS) $^ -o $@
	$(TRACE)

clean:
	@rm -f $(OBJ)/*.o $(OBJ)/*.out $(OBJ)/*.bin $(OBJ)/$(BIN)
	@echo "Cleaned."

.PHONY: clean run

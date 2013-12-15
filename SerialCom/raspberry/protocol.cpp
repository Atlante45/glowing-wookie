#include "protocol.h"
#include "../common/protocol_command.h"
#include "../common/bits.h"

void sendCommand(char header, char *payload, int payload_length){
    char *buffer = new char[HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length + CHECKSUM_LENGTH];
    unsigned int offset = 0;
    buffer[0] = header;
    binary_write(buffer, HEADER_SIZE, DATA_SIZE_SIZE, payload_length);
    for (int i = 0; i < payload_length; i++)
        buffer[HEADER_LENGTH + DATA_SIZE_LENGTH + i] = payload[i];
    binary_write(buffer,
                 HEADER_SIZE + DATA_SIZE_SIZE + payload_length * 8,
                 CHECKSUM_SIZE,
                 checksum(buffer, HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length));
    port.Write(buffer, HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length + CHECKSUM_LENGTH);
}

void getCaps() {
    char header=0;
    binary_write(&(header[0]), 0, COMMAND_SIZE, GET_CAPS);
    sendCommand(header, NULL, 0);
}

void reset() {
    char header=0;
    binary_write(&(header[0]), 0, COMMAND_SIZE, RESET);
    sendCommand(header, NULL, 0);
}

void ping() {
    char header=0;
    binary_write(&(header[0]), 0, COMMAND_SIZE, PING);
    char version=HOST_PROTOCOL_VERSION;
    sendCommand(header, &(version[0]), 1);
}

void read(mask_t *pins) {
    char *buffer = NULL;
    int pin = mask__single_value_index(pins);

    if(pin != -1) {
        buffer = new char[2];

        buffer[0] = READ;
        buffer[1] = pin & 0b00011111;
    } else {
        buffer = new char[4];

        buffer[0] = READ + 0b00001000;
        mask__to_string(pins, buffer + 1, 3);
    }
}

void write(enum types type, mask_t *pins, mask_t *values) {
    int pin = mask__single_value_index(pins);

    char header = 0;
    char *data = NULL;
    binary_write(&(header[0]), 0, COMMAND_SIZE, WRITE);
    binary_write(&(header[0]), TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE, type);
    if(pin != -1) {
        binary_write(&(header[0]), MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, 0);
        char data_length = PIN_ID_LENGTH + TYPE_LENGTH[type];
        char *data = new char[data_length];
        binary_write(&(data[0]),0, PIN_ID_SIZE, pin);
        binary_write(data, PIN_ID_SIZE, TYPE_SIZE[type], values->values[pin]);
        sendCommand(header, &(data[0]), data_length);
    } else {
        binary_write(&(header[0]), MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, 1);
        char data_length = PIN_ID_LENGTH + TYPE_LENGTH[type];
        //TODO
    }

    if (data != NULL)
        delete data;
}

void setType(mask_t *pins, mask_t *states) {
    char header = 0;
    int pin = mask__single_value_index(pins);


    if(pin != -1) {
        buffer[0] = SET_TYPE;
        buffer[1] =  (states->values[pin] << 5) & 0b01100000 + pin & 0b00011111;
    } else {
        buffer = new char[10];

        buffer[0] = SET_TYPE + 0b00001000;
        mask__to_string(pins  , buffer + 1, 3);
        mask__to_string(states, buffer + 4, 6);
    }
}

void getType(mask_t *pins) {
    char *buffer;
    int pin = mask__single_value_index(pins);

    if(pin != -1) {
        buffer = new char[2];

        buffer[0] = GET_TYPE;
        buffer[1] = pin & 0b00011111;
    } else {
        buffer = new char[4];

        buffer[0] = GET_TYPE + 0b00001000;
        mask__to_string(pins, buffer + 1, 3);
    }
}

void getFailSafe() {
    // TODO
}

void setFailSafe() {
    // TODO
}


#include "serialib.h"
#include <iostream>

#define SERIAL_PORT     "/dev/ttyUSB0"
#define SERIAL_BAUDRATE 2400

int main () {
    serialib port;
    port.Open(SERIAL_PORT, SERIAL_BAUDRATE);

    char buffer[255] = "ping";

    do {
        std::cout << buffer << std::endl;
        port.Write(buffer, strlen(buffer));
    } while(port.Read(buffer,255, 1000) != 0);

    return 0;
}














#include "protocol.h"

#include "../common/bits.h"
#include "../common/checksum.h"

#include <iostream>


Protocol::Protocol(serialib *serialPort){
    port = serialPort;
}

void Protocol::sendCommand( char header, char *payload, int payload_length){
    unsigned int packet_length = HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length + CHECKSUM_LENGTH;
    char *buffer = new char[packet_length];
    buffer[0] = header;
    binary_write(buffer, HEADER_SIZE, DATA_SIZE_SIZE, payload_length);
    for (int i = 0; i < payload_length; i++)
        buffer[HEADER_LENGTH + DATA_SIZE_LENGTH + i] = payload[i];
    binary_write(buffer,
                 HEADER_SIZE + DATA_SIZE_SIZE + payload_length * 8,
                 CHECKSUM_SIZE,
                 checksum(buffer, HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length));

    //DEBUG
    for (int i=0; i < packet_length; i++)
        binary_print(8, buffer[i]);
    std::cout << std::endl;

    port->Write(buffer, packet_length);
    delete buffer;
}

void Protocol::getCaps() {
    char header=0;
    binary_write(&header, 0, COMMAND_SIZE, GET_CAPS);
    sendCommand(header, NULL, 0);
}

void Protocol::reset() {
    char header=0;
    binary_write(&header, 0, COMMAND_SIZE, RESET);
    sendCommand(header, NULL, 0);
}

void Protocol::ping() {
    char header=0;
    binary_write(&header, 0, COMMAND_SIZE, PING);
    char version=HOST_PROTOCOL_VERSION;
    sendCommand(header, &version, 1);
}

void Protocol::read(mask_t *pins) {
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

void Protocol::write(enum types type, mask_t *pins, mask_t *values) {
    int pin = mask__single_value_index(pins);

    char header = 0;
    char *data = NULL;
    int data_length = 0;

    binary_write(&header, 0, COMMAND_SIZE, WRITE);
    binary_write(&header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE, type);
    if(pin != -1) {
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, 0);
        data_length = PIN_ID_LENGTH + TYPE_LENGTH[type];
        char *data = new char[data_length];
        binary_write(&(data[0]),0, PIN_ID_SIZE, pin);
        binary_write(data, PIN_ID_SIZE, TYPE_SIZE[type], values->values[pin]);

    } else {
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, 1);
        data_length = mask__get_length(pins) + mask__get_length(values);
        char *data = new char[data_length];

        mask__to_string(pins, data, 0);
        mask__to_string(values, data, mask__get_size(pins));

        sendCommand(header, data, data_length);
    }
    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::setType(mask_t *pins, mask_t *states) {
    char header = 0;
    int pin = mask__single_value_index(pins);


//    if(pin != -1) {
//        buffer[0] = SET_TYPE;
//        buffer[1] =  (states->values[pin] << 5) & 0b01100000 + pin & 0b00011111;
//    } else {
//        buffer = new char[10];

//        buffer[0] = SET_TYPE + 0b00001000;
//        mask__to_string(pins  , buffer + 1, 3);
//        mask__to_string(states, buffer + 4, 6);
//    }
}

void Protocol::getType(mask_t *pins) {
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

void Protocol::getFailSafe() {
    // TODO
}

void Protocol::setFailSafe() {
    // TODO
}


#define SERIAL_PORT     "/dev/ttyUSB0"
#define SERIAL_BAUDRATE 2400

int main () {
    serialib port;
    port.Open( SERIAL_PORT, SERIAL_BAUDRATE);

    Protocol p (&port);
    p.ping();

//    char buffer[255] = "ping";
//    do {
//        std::cout << buffer << std::endl;
//        port.Write(buffer, strlen(buffer));
//    } while(port.Read(buffer,255, 1000) != 0);

    return 0;
}

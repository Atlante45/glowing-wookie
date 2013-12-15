#include "protocol.h"

#include "../common/bits.h"
#include "../common/checksum.h"
#include "../common/protocol_util.h"

#include <iostream>


Protocol::Protocol(serialib *serialPort){
    port = serialPort;
}

void Protocol::parse(){
    char header[HEADER_LENGTH];
    port->Read(&(header[0]), HEADER_LENGTH, 1000);
    int command = binary_read(header, COMMAND_INDEX, COMMAND_SIZE);
    int replay_code = binary_read(header, REPLY_CODE_INDEX, REPLY_CODE_SIZE);

    char size_buffer[DATA_SIZE_LENGTH];
    port->Read(&(size_buffer[0]), DATA_SIZE_LENGTH, 1000);
    int size = binary_read(&(size_buffer[0]), DATA_SIZE_INDEX, DATA_SIZE_SIZE);

    int reply_id = 0;
    port->Read((char*) &reply_id, REPLY_ID_LENGTH, 1000);

    int payload_size = size - HEADER_LENGTH - DATA_SIZE_LENGTH - REPLY_ID_LENGTH - CHECKSUM_LENGTH;
    char *payload = new char[payload_size];
    port->Read(payload, payload_size, 1000);

    int checksum = 0;
    port->Read((char*) &checksum, CHECKSUM_LENGTH, 1000);

    //DEBUG
    std::cout << "Received command " << command << std::endl;

    switch(command){
    case GET_CAPS:
        break;
    case RESET:
        break;
    case PING:
        break;
    case READ:
        break;
    case WRITE:
        break;
    case GET_TYPE:
        break;
    case SET_TYPE:
        break;
    case GET_FAIL_SAFE:
        break;
    case SET_FAIL_SAFE:
        break;
    case RESERVED_COMMAND:
        break;

    }
}

void Protocol::sendCommand( char header, char *payload, int payload_length){
    int packet_length = 0;
    char *buffer = protocol__make_packet(&packet_length, header, payload, payload_length);

    //DEBUG
    for (int i=0; i < packet_length; i++)
        binary_print(8, buffer[i]);
    std::cout << std::endl;

    port->Write(buffer, packet_length);
    if (buffer != NULL)
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
    char version=PROTOCOL_VERSION;
    sendCommand(header, &version, 1);
}

void Protocol::read(enum types type, mask_t *pins) {
    int pin = mask__single_value_index(pins);

    char *data = NULL;
    int data_length = 0;

    char header = 0;
    binary_write(&header, 0, COMMAND_SIZE, WRITE);
    binary_write(&header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE, type);

    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = PIN_ID_LENGTH;
        data = new char[data_length];
        binary_write(data,0, PIN_ID_SIZE, pin);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = mask__get_length(pins);
        data = new char[data_length];
        mask__to_string(pins, data, 0);
    }

    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::write(enum types type, mask_t *pins, mask_t *values) {
    int pin = mask__single_value_index(pins);

    char header = 0;
    char *data = NULL;
    int data_length = 0;

    binary_write(&header, 0, COMMAND_SIZE, WRITE);
    binary_write(&header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE, type);
    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = PIN_ID_LENGTH + TYPE_LENGTH[type];
        data = new char[data_length];
        binary_write(data,0, PIN_ID_SIZE, pin);
        binary_write(data, PIN_ID_SIZE, TYPE_SIZE[type], values->values[pin]);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = mask__get_length(pins) + mask__get_length(values);
        data = new char[data_length];
        mask__to_string(pins, data, 0);
        mask__to_string(values, data, mask__get_size(pins));
    }
    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::setType(mask_t *pins, mask_t *states) {
    int pin = mask__single_value_index(pins);

    char header = 0;
    char *data = NULL;
    int data_length = 0;

    binary_write(&header, 0, COMMAND_SIZE, SET_TYPE);
    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = PIN_ID_LENGTH + TYPE_DATA_LENGTH;
        data = new char[data_length];
        binary_write(data,0, PIN_ID_SIZE, pin);
        binary_write(data+1, TYPE_DATA_INDEX, TYPE_DATA_SIZE, states->values[pin]);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = mask__get_length(pins) + mask__get_length(states);
        data = new char[data_length];
        mask__to_string(pins, data, 0);
        mask__to_string(states, data, mask__get_size(pins));
    }
    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::getType(mask_t *pins) {
    int pin = mask__single_value_index(pins);

    char *data = NULL;
    int data_length = 0;

    char header = 0;
    binary_write(&header, 0, COMMAND_SIZE, GET_TYPE);

    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = PIN_ID_LENGTH;
        data = new char[data_length];
        binary_write(data,0, PIN_ID_SIZE, pin);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = mask__get_length(pins);
        data = new char[data_length];
        mask__to_string(pins, data, 0);
    }

    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::getFailSafe(mask_t *pins) {
    int pin = mask__single_value_index(pins);

    char *data = NULL;
    int data_length = 0;

    char header = 0;
    binary_write(&header, 0, COMMAND_SIZE, GET_FAIL_SAFE);

    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = PIN_ID_LENGTH;
        data = new char[data_length];
        binary_write(data,0, PIN_ID_SIZE, pin);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = mask__get_length(pins);
        data = new char[data_length];
        mask__to_string(pins, data, 0);
    }

    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}

void Protocol::setFailSafe(int timeout, enum types type, mask_t *pins, mask_t *values) {
    int pin = mask__single_value_index(pins);

    char header = 0;
    char *data = NULL;
    int data_length = 0;

    binary_write(&header, 0, COMMAND_SIZE, WRITE);
    binary_write(&header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE, type);
    if(pin != -1) {
        // single pin
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_DISABLED);
        data_length = TIMEOUT_LENGTH + PIN_ID_LENGTH + TYPE_LENGTH[type];
        data = new char[data_length];
        binary_write(data, 0, TIMEOUT_SIZE, timeout);
        binary_write(data, TIMEOUT_SIZE, PIN_ID_SIZE, pin);
        binary_write(data, TIMEOUT_SIZE+PIN_ID_SIZE, TYPE_SIZE[type], values->values[pin]);
    } else {
        // multiple pins
        binary_write(&header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE, MASKP_PARAMETER_ENABLED);
        data_length = TIMEOUT_LENGTH + mask__get_length(pins) + mask__get_length(values);
        data = new char[data_length];
        binary_write(data, 0, TIMEOUT_SIZE, timeout);
        mask__to_string(pins, data, TIMEOUT_SIZE);
        mask__to_string(values, data, TIMEOUT_SIZE + mask__get_size(pins));
    }
    sendCommand(header, data, data_length);

    if (data != NULL)
        delete data;
}


#define SERIAL_PORT     "echo" //"/dev/ttyUSB0"
#define SERIAL_BAUDRATE 2400

int main () {
    serialib port;
    port.Open( SERIAL_PORT, SERIAL_BAUDRATE);

    Protocol p (&port);
    p.ping();
    p.parse();

    //    char buffer[255] = "ping";
    //    do {
    //        std::cout << buffer << std::endl;
    //        port.Write(buffer, strlen(buffer));
    //    } while(port.Read(buffer,255, 1000) != 0);

    return 0;
}

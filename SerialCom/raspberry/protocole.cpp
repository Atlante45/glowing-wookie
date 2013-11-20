#include "protocole.h"

#define CHAR_SIZE 8

#define PROTOCOLE_VERSION_MIN 0
#define PROTOCOLE_VERSION     0

#define SET_PROTOCOLE_VERSION_COMMAND 0b00000000
#define RESET_COMMAND                 0b00010000
#define SET_MODE_COMMAND              0b00110000
#define SET_HEART_BEAT_COMMAND        0b01000000
#define HEART_BEAT_COMMAND            0b01010000
#define SET_STATE_COMMAND             0b10000000
#define GET_STATE_COMMAND             0b10010000
#define READ_COMMAND                  0b10100000
#define WRITE_COMMAND                 0b10110000
#define MONITOR_READ_COMMAND          0b11000000

void getProtocolVersion() {
    char *buffer = new char[5];

    buffer[0] = SET_PROTOCOLE_VERSION_COMMAND;
    buffer[1] = PROTOCOLE_VERSION_MIN >> CHAR_SIZE;
    buffer[2] = PROTOCOLE_VERSION_MIN % (1 << CHAR_SIZE);
    buffer[3] = PROTOCOLE_VERSION     >> CHAR_SIZE;
    buffer[4] = PROTOCOLE_VERSION     % (1 << CHAR_SIZE);
}

void reset(char resetType) {
    char *buffer = new char[1];

    buffer[0] = RESET_COMMAND + ((resetType << 1) & 0b00001110);
}

void failSafe() {
    // TODO
}

void setMode(Mode mode) {
    char *buffer = new char[1];

    buffer[0] = SET_MODE_COMMAND + ((mode << 3)  & 0b00001000);
}

void setHeartBeat(char freq) {
    char *buffer = new char[1];

    buffer[0] = SET_HEART_BEAT_COMMAND + (freq  & 0b00001111);
}

void heartBeat() {
    char *buffer = new char[2];
    buffer[1] = PROTOCOLE_VERSION >> CHAR_SIZE;
    buffer[2] = PROTOCOLE_VERSION % (1 << CHAR_SIZE);
}

void setState(mask_t *pins, mask_t *states) {
    char *buffer;

    if(1) { // TODO
        int pin; // TODO
        buffer = new char[2];

        buffer[0] = SET_STATE_COMMAND;
        buffer[1] =  (states->values[pin] << 5) & 0b01100000 + pin & 0b00011111;
    } else {
        buffer = new char[10];

        buffer[0] = SET_STATE_COMMAND + 0b00001000;
        mask__to_string(pins  , buffer + 1, 3);
        mask__to_string(states, buffer + 4, 6);
    }
}

void getState(mask_t *pins) {
    char *buffer;

    if(1) { // TODO
        int pin; // TODO
        buffer = new char[2];

        buffer[0] = GET_STATE_COMMAND;
        buffer[1] = pin & 0b00011111;
    } else {
        buffer = new char[4];

        buffer[0] = GET_STATE_COMMAND + 0b00001000;
        mask__to_string(pins, buffer + 1, 3);
    }
}



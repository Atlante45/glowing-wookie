#ifndef PROTOCOL_COMMANDS_H_
#define PROTOCOL_COMMANDS_H_

/* #### HEADER #### */
#define HEADER_LENGTH 1 // bytes
#define HEADER_SIZE   8 // bits

//COMMAND
#define COMMAND_SIZE 4
#define COMMAND_INDEX 0

//PARAMETER
#define PARAMETER_SIZE 4
#define TYPE_PARAMETER_SIZE 3
#define TYPE_PARAMETER_INDEX 4
#define MASKP_PARAMETER_SIZE 1
#define MASKP_PARAMETER_INDEX 7

/* #### SIZE #### */
#define DATA_SIZE_LENGTH 2 
#define DATA_SIZE_SIZE 16 
#define DATA_SIZE_INDEX 0

/* #### DATA #### */
//PING
#define VERSION_LENGTH 1
#define VERSION_SIZE 8
#define VERSION_INDEX 0

#define PIN_ID_LENGTH 1
#define PIN_ID_SIZE 8

// CHECKSUM
#define CHECKSUM_LENGTH 1
#define CHECKSUM_SIZE 8

enum command {
    GET_CAPS      = 0,
    RESET         = 1,
    PING          = 2,
    READ          = 3,
    WRITE         = 4,
    GET_TYPE      = 5,
    SET_TYPE      = 6,
    GET_FAIL_SAFE = 7,
    SET_FAIL_SAFE = 8,

    RESERVED      = 15
};

enum types {
    ANALOG_8 = 0,
    ANALOG_16 = 1,
    BOOLEAN = 2,
    PWM_8 = 3,
    PWM_16 = 4,

    UNSPECIFIED = 7
};

#endif

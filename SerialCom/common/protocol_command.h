#ifndef PROTOCOL_COMMANDS_H_
#define PROTOCOL_COMMANDS_H_

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

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

/*COMMAND*/
#define COMMAND_SIZE 4
#define COMMAND_INDEX 0

/*SIZE*/
#define SIZE_SIZE 16
#define SIZE_INDEX 8

/*PING*/
#define VERSION_SIZE 8
#define VERSION_INDEX 24

#endif

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../common/protocol_command.h"

#define NB_PINS 23

#define MASK_LENGTH 3

typedef
struct state{
    enum types type[NB_PINS];
    unsigned int reply_id;
} state;

state current_state;

/* Function to analyze the protocol*/
void parseProtocol(state *current);

#endif

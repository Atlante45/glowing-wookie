#ifndef PROTOCOL_DEBUG_
#define PROTOCOL_DEBUG_



static const char *COMMAND_NAME_[] = {
  "GET_CAPS",
  "RESET",
  "PING",
  "READ",
  "WRITE",
  "GET_TYPE",
  "SET_TYPE",
  "GET_FAIL_SAFE",
  "SET_FAIL_SAFE",
  "UNDEFINED"
};
#define NB_COMMANDS_ 9

static const char *COMMAND_NAME(int command){
  if (command >= 0 && command < NB_COMMANDS_){
    return COMMAND_NAME_[command];
  }
  return COMMAND_NAME_[NB_COMMANDS_];
}

#endif

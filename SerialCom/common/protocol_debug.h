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

static const char *REPLY_CODE_NAME_[] = {
    "SUCCESS",
    "PARTIAL_SUCCESS",
    "FAILURE",
    "RESERVED_REPLY"
};
#define NB_REPLY_CODES_ 3

static const char *TYPE_NAME_[] = {
    "ANALOG_8",
    "ANALOG_16",
    "BOOLEAN",
    "PWM_8",
    "PWM_16",
    "UNSPECIFIED"
};  
#define NB_TYPES_ 5

static const char *COMMAND_NAME(int command){
  if (command >= 0 && command < NB_COMMANDS_){
    return COMMAND_NAME_[command];
  }
  return COMMAND_NAME_[NB_COMMANDS_];
}
static const char *REPLY_CODE_NAME(int code){
  if (code >= 0 && code < NB_REPLY_CODES_){
    return REPLY_CODE_NAME_[code];
  }
  return REPLY_CODE_NAME_[NB_REPLY_CODES_];
}
static const char *TYPE_NAME(int type){
  if (type >= 0 && type < NB_TYPES_){
    return TYPE_NAME_[type];
  }
  return TYPE_NAME_[NB_TYPES_];
}



#endif

#include "protocol.h"

#include "../common/bits.h"
#include "../common/checksum.h"
#include "../common/protocol_util.h"

#include <iostream>


Protocol::Protocol(serialib *serialPort){
  port = serialPort;
  timeout=1000;
}

int Protocol::parse(int &command, int &reply_code, int &payload_size, char *payload){
  int read;

  // HEADER
  char header[HEADER_LENGTH];
  read=port->Read(&(header[0]), HEADER_LENGTH, timeout);
  if (read!=1)
    return read;
  command = binary_read(header, COMMAND_INDEX, COMMAND_SIZE);
  reply_code = binary_read(header, REPLY_CODE_INDEX, REPLY_CODE_SIZE);

  std::cout << "header " << (int)header[0] << " r=" << read << std::endl;

  // SIZE
  char size_buffer[DATA_SIZE_LENGTH];
  read=port->Read(&(size_buffer[0]), DATA_SIZE_LENGTH, timeout);
  if (read!=1)
    return read;
  int size = binary_read(&(size_buffer[0]), DATA_SIZE_INDEX, DATA_SIZE_SIZE);
  std::cout << "size " << size << std::endl;

  // REPLYID
  int reply_id = 0;
  read=port->Read((char*) &reply_id, REPLY_ID_LENGTH, timeout);
  if (read!=1)
    return read;
  // TODO check replyid
  std::cout << "reply_id " << reply_id << std::endl;


  // REPLY SPECIFIC PAYLOAD
  payload_size = size - HEADER_LENGTH - DATA_SIZE_LENGTH - REPLY_ID_LENGTH - CHECKSUM_LENGTH;
  std::cout << "payload_size " << payload_size << std::endl;
  if (payload_size < 0)
    return -3;
  payload = new char[payload_size];
  read=port->Read(payload, payload_size, timeout);
  if (read!=1){
    delete[] payload;
    return read;
  }

  // CHECKSUM
  int checksum = 0;
  read=port->Read((char*) &checksum, CHECKSUM_LENGTH, timeout);
  if (read!=1){ // TODO checksum
    delete[] payload;
    return read;
  }

  std::cout << "checksum " << checksum << std::endl;

  std::cout << "Received command " << command << std::endl;

}


void Protocol::receiveCommand(int &command, int &reply_code, int &payload_size, char *payload){
  //TODO timeout
  while(parse(command, reply_code, payload_size, payload)!=0);
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
    delete[] buffer;
}

//////////////////////////////// SYNC /////////////////////////////////////

int Protocol::getCaps(int &output__nb_pins){
  sendGetCaps();

  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);

  if (payload == NULL)
    return -1;
  if (command != GET_CAPS || reply_code != SUCCESS){
    delete[] payload;
    return -2;
  }

  output__nb_pins = binary_read(payload, 0, NB_PINS_SIZE);
  //TODO read type_mask

  delete[] payload;

  return 0;
}

int Protocol::reset(){
  sendReset();

  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != RESET || reply_code != SUCCESS){
    delete payload;
    return -2;
  }

  delete payload;

  return 0;

}

int Protocol::ping(int &output__protocol_version){
  sendPing();

  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != PING || reply_code != SUCCESS){
    delete payload;
    return -2;
  }

  output__protocol_version = binary_read(payload, 0, 1);
  
  delete[] payload;

  return 0;
}

int Protocol::read(enum types type, mask_t *pins, mask_t *output__values){ 
  sendRead(type, pins);
  
  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != READ || (reply_code != SUCCESS && reply_code != PARTIAL_SUCCESS)){
    delete payload;
    return -2;
  }

  int nb_values = payload_size / TYPE_SIZE[type];
  output__values = mask__from_string(payload, nb_values, TYPE_SIZE[type], 0);
  
  delete[] payload;

  return 0;
}

int Protocol::write(enum types type, mask_t *pins, mask_t *values){ 
  sendWrite(type, pins, values);

  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != WRITE || reply_code != SUCCESS){
    delete payload;
    return -2;
  }

  delete payload;

  return 0;
}

int Protocol::setType(mask_t *pins, mask_t *states){ 
  sendSetType(pins, states);

  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != SET_TYPE || reply_code != SUCCESS){
    delete payload;
    return -2;
  }

  delete payload;

  return 0;
}

int Protocol::getType(mask_t *pins, mask_t *output__type_mask){ 
  sendGetType(pins);
  
  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != GET_TYPE || (reply_code != SUCCESS && reply_code != PARTIAL_SUCCESS)){
    delete payload;
    return -2;
  }

  int nb_values = payload_size / TYPE_DATA_SIZE;
  output__type_mask = mask__from_string(payload, nb_values, TYPE_DATA_SIZE, 0);
  
  delete[] payload;

  return 0;

}

void Protocol::getFailSafe(mask_t *pins){ 
  sendGetFailSafe(pins);
  //TODO

}

int Protocol::setFailSafe(int timeout, enum types type, mask_t *pins, mask_t *values){
  sendSetFailSafe(timeout, type, pins, values);
  
  int command, reply_code, payload_size;
  char *payload = NULL;
  receiveCommand(command, reply_code, payload_size, payload);
  
  if (payload == NULL)
    return -1;
  if (command != SET_FAIL_SAFE || (reply_code != SUCCESS && reply_code != PARTIAL_SUCCESS)){
    delete payload;
    return -2;
  }

  delete[] payload;

  return 0;
  
}





//////////////////////////////// ASYNC ////////////////////////////////////

void Protocol::sendGetCaps() {
  char header=0;
  binary_write(&header, 0, COMMAND_SIZE, GET_CAPS);
  sendCommand(header, NULL, 0);
}

void Protocol::sendReset() {
  char header=0;
  binary_write(&header, 0, COMMAND_SIZE, RESET);
  sendCommand(header, NULL, 0);
}

void Protocol::sendPing() {
  char header=0;
  binary_write(&header, 0, COMMAND_SIZE, PING);
  char version=PROTOCOL_VERSION;
  sendCommand(header, &version, 1);
}

void Protocol::sendRead(enum types type, mask_t *pins) {
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

void Protocol::sendWrite(enum types type, mask_t *pins, mask_t *values) {
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

void Protocol::sendSetType(mask_t *pins, mask_t *states) {
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

void Protocol::sendGetType(mask_t *pins) {
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

void Protocol::sendGetFailSafe(mask_t *pins) {
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

void Protocol::sendSetFailSafe(int timeout, enum types type, mask_t *pins, mask_t *values) {
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
  //    p.ping();

  int command, reply_code, payload_size;
  char *payload = NULL;
  // while(1){
  //   int r = p.parse(command, reply_code, payload_size, payload);
  //   if (r != -2)
  //     std::cout << "> " << r << " command=" << command << std::endl;
  // }

  return 0;
}

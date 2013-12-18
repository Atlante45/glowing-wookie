#include "protocol.h"
#include "communication.h"

#include "../common/protocol_util.h"
#include "../common/bits.h"
#include "../common/mask.h"
#include <stdlib.h>

#ifdef DEBUG
#include "stdio.h"
#endif

#define PAYLOAD_OFFSET_LENGTH REPLY_ID_LENGTH
#define PAYLOAD_OFFSET_SIZE REPLY_ID_SIZE

/* Send a command.
 * <<WARNING>> payload and payload_length should include a PAYLOAD_OFFSET (to write the reply_id) !
 */
void send_command(state *current, enum command command, enum reply_code reply_code, char *payload, int payload_length){
  // build command
  /* Format of a command :
     The corresponding command number, on 4 bits
     A reply code, on 4 bits
     A packet size in 8-bit bytes, on 16 bits
     A reply id, on 8 bits
     A reply-specific payload, on a variable number of bits
     A checksum, on 8 bits, aligned on an 8-bit boundary
  */

#ifdef DEBUG
  printf("Sending command...\n");
  printf(" command        = %d\n", command);
  printf(" reply code     = %d\n", reply_code);
  printf(" payload length = %d\n", payload_length);
  printf(" reply specific = %d\n", payload_length - REPLY_ID_LENGTH);
  #endif

  char header = 0;
  binary_write(&header, 0, COMMAND_SIZE, (int) command);
  binary_write(&header, REPLY_CODE_INDEX, REPLY_CODE_SIZE, (int) reply_code);
  binary_write(payload, 0, REPLY_ID_SIZE, current->reply_id);

  // make packet
  int packet_length = 0;
  char *buffer = protocol__make_packet(&packet_length, header,
				       payload,
				       payload_length);


#ifdef DEBUG
  printf(" reply id       = %d\n", current->reply_id);
  printf(" packet length  = %d\n", packet_length);
  int i;
  for (i=0; i < packet_length; i++){
    if (i>0 && i%5==0) printf("\n");
    printf(" [%02d] ", i);
    binary_print(8, buffer[i]); 
  }
  printf("\n");

#endif


  // send message
  current->reply_id ++;
  send_msg(buffer, packet_length * 8);

  if (buffer != NULL)
    free(buffer);
}


/*** GET_CAPS ***/
void answerGET_CAPS(state *current)
{
  mask_t *type_mask = mask__new(NB_PINS, TYPE_DATA_SIZE);

  // TODO fill typeMask with correct values

  char data[PAYLOAD_OFFSET_LENGTH + NB_PINS_LENGTH + NB_PINS * TYPE_DATA_SIZE];
  binary_write(&(data[0]), PAYLOAD_OFFSET_SIZE, NB_PINS_SIZE, NB_PINS);
  mask__to_string(type_mask, &(data[0]), PAYLOAD_OFFSET_SIZE + NB_PINS_SIZE);

  send_command(current, GET_CAPS, SUCCESS, &(data[0]), PAYLOAD_OFFSET_LENGTH + NB_PINS_LENGTH + NB_PINS * TYPE_DATA_SIZE);
}

void parseGET_CAPS(state *current, char* header, int length, char *payload)
{
  answerGET_CAPS(current);
}



/*** RESET ***/ 
void actionRESET()
{
  /* TODO */
}

void parseRESET(char* header, int length, char *payload)
{
  actionRESET();
}

/*** PING ***/
void answerPING(state *current)
{
  char buff[PAYLOAD_OFFSET_LENGTH + 1];
  buff[PAYLOAD_OFFSET_LENGTH]=PROTOCOL_VERSION;
  send_command(current, PING, SUCCESS, buff, PAYLOAD_OFFSET_LENGTH + 1);
}

void parsePING(state *current, char* header, int length, char *payload)
{

  int version = binary_read(payload, VERSION_INDEX, VERSION_SIZE);

#ifdef DEBUG
  printf("[PING] Version = %d\n", version);
#endif


  answerPING(current);
}

/*** READ ***/
void parseREAD(state *current, char* header, int length, char *payload){
  char type = binary_read(header, TYPE_PARAMETER_INDEX,
			  TYPE_PARAMETER_SIZE);
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX,
				    MASKP_PARAMETER_SIZE);
  int offset = 0;

  //////////////////////////// MULTIPLE PINS ///////////////////////
  if(mask_is_present){ 
    mask_t * mask = mask__from_string (payload, NB_PINS,
				       TYPE_SIZE[BOOLEAN], 0);

    /////////////////// BOOLEAN ///////////////
    if(type == BOOLEAN){
      char reply_payload[PAYLOAD_OFFSET_LENGTH + MASK_LENGTH] = {0};
      char *values = &reply_payload[PAYLOAD_OFFSET_LENGTH];
      int i, j=0;
      for(i=0; i<NB_PINS; i++){
	if(mask->values[i]){
	  binary_write(values, j, 1, digital_read(i));
	  j++; 
	}
      }
      send_command(current, READ, SUCCESS, reply_payload,
		   PAYLOAD_OFFSET_LENGTH + (j-1)/8 +1);
    }

    //////////////////  ANALOG ///////////////
    else if(type == ANALOG_8){
      char reply_payload[PAYLOAD_OFFSET_LENGTH + NB_PINS] = {0};
      char* values = &reply_payload[PAYLOAD_OFFSET_LENGTH]; 
      int i, j=0;
      for(i=0; i<NB_PINS; i++){
	if(mask->values[i]) {
	  binary_write(values, j*TYPE_SIZE[ANALOG_8],
		       TYPE_SIZE[ANALOG_8], analog_read(i));
	  j++; 
	}
      }
      send_command(current, READ, SUCCESS, reply_payload,
		   PAYLOAD_OFFSET_LENGTH + j);
    }
    mask__free(mask);

    ///////////////////////// SINGLE PIN ///////////////////////////////
  } else {
    int pinID = binary_read(payload, 0, PIN_ID_SIZE);

    //////////// BOOLEAN ///////////
    if(type == BOOLEAN){
      char reply_payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
      char* value = &reply_payload[PAYLOAD_OFFSET_LENGTH];
      binary_write(value, 0, TYPE_SIZE[BOOLEAN], digital_read(pinID));
      send_command(current, READ, SUCCESS, reply_payload,
		   PAYLOAD_OFFSET_LENGTH + 1);
    }

    //////////// ANALOG ////////////
    else if(type == ANALOG_8) {
      char reply_payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
      char* value = &reply_payload[PAYLOAD_OFFSET_LENGTH];
      binary_write(value, 0, TYPE_SIZE[ANALOG_8], analog_read(pinID));
      send_command(current, READ, SUCCESS, reply_payload,
		   PAYLOAD_OFFSET_LENGTH + 1);
    }
  }
}

/*** WRITE ***/
void parseWRITE(state *current, char* header, int length, char *payload){
  char type = binary_read(header, TYPE_PARAMETER_INDEX,
			  TYPE_PARAMETER_SIZE);
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX,
				    MASKP_PARAMETER_SIZE);

  int offset = 0;

  //////////////////////////// MULTIPLE PINS ///////////////////////
  if(mask_is_present){
    mask_t * mask = mask__from_string (payload, NB_PINS,
				       TYPE_SIZE[BOOLEAN], 0);
    offset += TYPE_SIZE[BOOLEAN];
    
    //////////// BOOLEAN ///////////
    if(type == BOOLEAN){
      char buffer= 0;
      int i, j=0;
      for(i=0; i<NB_PINS; i++){
	if(mask->values[i]){
	  int value = binary_read(payload, offset + j,
				  TYPE_SIZE[BOOLEAN]); 
	  digital_write(i,value); 
	  j++; 
	}
      }
    }

    //////////// ANALOG ////////////
    else if(type == ANALOG_8){
      int i, j=0;
      for(i=0; i<NB_PINS; i++){
	if(mask->values[i]){
	  int value = binary_read(payload, offset + j,
				  TYPE_SIZE[ANALOG_8]); 
	  pwm_write(i,value); 
	  j++;
	}
      }

    }
    mask__free(mask);
  }

  ///////////////////////// SINGLE PIN ///////////////////////////////
  else {

    int pinID = binary_read(payload, 0, PIN_ID_SIZE);
    offset += PIN_ID_SIZE;

    //////////// BOOLEAN ///////////
    if(type == BOOLEAN){
      char val_buffer;
      int value = binary_read(&val_buffer, offset, TYPE_SIZE[BOOLEAN]); 
      digital_write(pinID,value); 
    }
    //////////// ANALOG ////////////
    else if(type == ANALOG_8){
      int value = binary_read(payload, offset, TYPE_SIZE[ANALOG_8]); 
      pwm_write(pinID,value); 
    }
  }
  char reply_payload[PAYLOAD_OFFSET_LENGTH];
  send_command(current, WRITE, SUCCESS, reply_payload, PAYLOAD_OFFSET_LENGTH );
}

/*** GET_TYPE ***/

void parseGET_TYPE(state *current, char* header, int length, char *payload){
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX,
				    MASKP_PARAMETER_SIZE);
  ////////////////////////// MULTIPLE PINS ///////////////////////////////
  if(mask_is_present){
    mask_t *value_mask = mask__new(NB_PINS,TYPE_DATA_SIZE);
    mask_t *mask = mask__from_string (payload, NB_PINS,
				      TYPE_SIZE[BOOLEAN], 0);
    int i,j=0;
    for(i=0; i<NB_PINS; i++){
      if(mask->values[i]) {
	value_mask->values[j] = current->type[i];
	j++;
      }

      mask->nb_values=j;
      int size = (j*TYPE_DATA_SIZE-1)/8 +1;
      char *reply_payload = malloc(PAYLOAD_OFFSET_LENGTH + size);
      mask__to_string(mask, reply_payload, PAYLOAD_OFFSET_LENGTH);

      send_command(current, GET_TYPE, SUCCESS, payload,
		   PAYLOAD_OFFSET_LENGTH + size);
    }
    mask__free(mask);
    mask__free(value_mask);
  }

  //////////////////////// SINGLE PIN /////////////////////////////////
  else {
      int pinID = binary_read(payload, 0, PIN_ID_SIZE);

      char reply_payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
      char *type = &reply_payload[PAYLOAD_OFFSET_LENGTH];
      binary_write(type, TYPE_DATA_INDEX, TYPE_DATA_SIZE,
		   current->type[pinID]);
      send_command(current, GET_TYPE, SUCCESS, payload,
		   PAYLOAD_OFFSET_LENGTH+1 );
    }
}

/*** SET_TYPE ***/
void parseSET_TYPE(state *current, char* header, int length, char *payload){
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX,
				    MASKP_PARAMETER_SIZE);

  ////////////////////////// MULTIPLE PINS ///////////////////////////////
  if(mask_is_present){
    mask_t *mask = mask__from_string (payload, NB_PINS,
				      TYPE_SIZE[BOOLEAN], 0);
    int i, j=0;
    for(i=0; i < mask->nb_values; i++)
      if(mask->values[i])
	j++;

    int payload_size = 
      length - HEADER_LENGTH - DATA_SIZE_LENGTH - CHECKSUM_LENGTH;

    int offset = mask__get_size(mask);
    mask_t * value_mask = mask__from_string (payload,
					     j, TYPE_DATA_SIZE,
					     offset);
    j=0;
    for(i=0; i < mask->nb_values; i++){
      if(mask->values[i]){
	current->type[i] = value_mask->values[j];
	j++;
      }
    }

    mask__free(mask);
  }

  ////////////////////////// SINGLE PIN ///////////////////////////////
  else{
    int pinID = binary_read(payload, 0, PIN_ID_SIZE);
    int type  = binary_read(payload,
			    PIN_ID_SIZE + TYPE_DATA_INDEX,
			    TYPE_DATA_SIZE);

    current->type[pinID] = type;
  }
  char reply_payload[PAYLOAD_OFFSET_LENGTH];
  send_command(current, SET_TYPE, SUCCESS, reply_payload,
	       PAYLOAD_OFFSET_LENGTH);
}

/*** GET_FAIL_SAFE ***/ 
void parseGET_FAIL_SAFE(char* header, int length, char *payload)
{
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
  if(mask_is_present)
    {
      /* TODO */
    }
  else
    {
      /* TODO */
    }
}


/*** SET_FAIL_SAFE ***/ 
void parseSET_FAIL_SAFE(char* header, int length, char *payload)
{
  int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
  if(mask_is_present)
    {
      /* TODO */
    }
  else
    {
      /* TODO */
    }
}

/*** Parse Protocol ***/
void parseProtocol(state *current)
{
  char header[HEADER_LENGTH];
  recv_msg(header, HEADER_LENGTH);
  int command = binary_read(header, COMMAND_INDEX, COMMAND_SIZE);

#ifdef DEBUG
  printf("Received command: \n header  = " );
  binary_print(HEADER_SIZE, (int)header[0]);
  printf("\n command = %d\n", command);
#endif

  char buffer[DATA_SIZE_LENGTH];
  recv_msg(&(buffer[0]), DATA_SIZE_LENGTH);
  int length = binary_read(&(buffer[0]), DATA_SIZE_INDEX, DATA_SIZE_SIZE);

  int payload_length = length - HEADER_LENGTH - CHECKSUM_LENGTH - DATA_SIZE_LENGTH;

#ifdef DEBUG
  printf(" length  = %d\n payload = %d\n", length, payload_length);
#endif
  char *payload = (char*)malloc(sizeof(char) * payload_length);
  recv_msg(&(payload[0]), payload_length);

  char checksum;
  recv_msg(&checksum, CHECKSUM_LENGTH);    

#ifdef DEBUG
  printf(" check s.= %d\n", (unsigned char)checksum);
#endif

  switch(command){
  case GET_CAPS:
    parseGET_CAPS(current, header, length, payload);
    break;
  case RESET:
    parseRESET(header,  length, payload);
    break;
  case PING:
    parsePING(current, header, length, payload);
    break;
  case READ:
    parseREAD(current, header, length, payload);
    break;
  case WRITE:
    parseWRITE(current, header, length, payload);
    break;
  case GET_TYPE:
    parseGET_TYPE(current, header, length, payload);
    break;
  case SET_TYPE:
    parseSET_TYPE(current, header, length, payload);
    break;
  case GET_FAIL_SAFE:
    parseGET_FAIL_SAFE(header, length, payload);
    break;
  case SET_FAIL_SAFE:
    parseSET_FAIL_SAFE(header, length, payload);
    break;
  case RESERVED_COMMAND:
    break;

  }

  free(payload);
}


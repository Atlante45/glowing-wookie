#include "protocol.h"
#include "communication.h"

#include "../common/protocol_util.h"
#include "../common/bits.h"
#include "../common/mask.h"
#include <stdlib.h>

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
    char header = 0;
    binary_write(&header, 0, COMMAND_SIZE, (int) command);
    binary_write(&header, REPLY_CODE_INDEX, REPLY_CODE_SIZE, (int) reply_code);
    binary_write(payload, 0, REPLY_ID_SIZE, current->reply_id);

    // make packet
    int packet_length = 0;
    char *buffer = protocol__make_packet(&packet_length, header, payload, REPLY_ID_LENGTH + payload_length);

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

void parseGET_CAPS(state *current, char* header, int size)
{
    answerGET_CAPS(current);
}



/*** RESET ***/ 
void actionRESET()
{
    /* TODO */
}

void parseRESET(char* header, int size)
{
    actionRESET();
}

/*** PING ***/
void answerdPING(state *current)
{
    char version=PROTOCOL_VERSION;
    send_command(current, PING, SUCCESS, &version, PAYLOAD_OFFSET_LENGTH + 1);
}

void parsePING(state *current, char* header, int size)
{
    char buffer[2];
    recv_msg(buffer, DATA_SIZE_LENGTH);
    recv_msg(buffer, VERSION_LENGTH);
    int version = binary_read(buffer, VERSION_INDEX, VERSION_SIZE);

    answerdPING(current);
}

/*** READ ***/
void parseREAD(char* header, int size)
{
    char type = binary_read(header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE);
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

/*** WRITE ***/
void parseWRITE(char* header, int size)
{
    char type = binary_read(header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE);
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

/*** GET_TYPE ***/
void parseGET_TYPE(char* header, int size)
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

/*** SET_TYPE ***/
void parseSET_TYPE(char* header, int size)
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

/*** GET_FAIL_SAFE ***/ 
void parseGET_FAIL_SAFE(char* header, int size)
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
void parseSET_FAIL_SAFE(char* header, int size)
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

    char buffer[DATA_SIZE_LENGTH];
    recv_msg(&(buffer[0]), DATA_SIZE_LENGTH);
    int size = binary_read(&(buffer[0]), DATA_SIZE_INDEX, DATA_SIZE_SIZE);

    switch(command){
    case GET_CAPS:
        parseGET_CAPS(current, header, size);
        break;
    case RESET:
        parseRESET(header, size);
        break;
    case PING:
        parsePING(current, header,size);
        break;
    case READ:
        parseREAD(header,size);
        break;
    case WRITE:
        parseWRITE(header,size);
        break;
    case GET_TYPE:
        parseGET_TYPE(header,size);
        break;
    case SET_TYPE:
        parseSET_TYPE(header,size);
        break;
    case GET_FAIL_SAFE:
        parseGET_FAIL_SAFE(header,size);
        break;
    case SET_FAIL_SAFE:
        parseSET_FAIL_SAFE(header,size);
        break;
    case RESERVED_COMMAND:
        break;

    }
}


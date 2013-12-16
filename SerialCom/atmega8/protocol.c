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
    char buff[PAYLOAD_OFFSET_LENGTH + 1];
    buff[PAYLOAD_OFFSET_LENGTH]=PROTOCOL_VERSION;
    send_command(current, PING, SUCCESS, buff, PAYLOAD_OFFSET_LENGTH + 1);
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
void parseREAD(state *current, char* header, int size)
{
    char type = binary_read(header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE);
    int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
    if(mask_is_present)
    {
        char mask[MASK_LENGTH]; 
        recv_msg(mask,MASK_LENGTH);
        if(type == BOOLEAN)
        {
            char payload[PAYLOAD_OFFSET_LENGTH + MASK_LENGTH] = {0};
            char * values = &payload[PAYLOAD_OFFSET_LENGTH];
            int i, j=0;

            for(i=0; i<NB_PINS; i++)
            {
                if(binary_read(mask, i, 1))
                {
                    binary_write(values, j, 1, digital_read(i));
                    j++; 
                }
            }
            send_command(current, READ, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH + (j-1)/8 +1);
        }
        else if(type == ANALOG_8)
        {
            char payload[PAYLOAD_OFFSET_LENGTH + NB_PINS] = {0};
            char* values = &payload[PAYLOAD_OFFSET_LENGTH]; 
            int i, j=0;
            for(i=0; i<NB_PINS; i++)
            {
                if(binary_read(mask, i, 1))
                {
                    binary_write(values, j*TYPE_SIZE[ANALOG_8], TYPE_SIZE[ANALOG_8], analog_read(i));
                    j++; 
                }
           }
           send_command(current, READ, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH + j);
        }
    }
    else
    {
        char buffer[PIN_ID_LENGTH];
        recv_msg(buffer,PIN_ID_LENGTH);
        int pinID = binary_read(buffer, 0, PIN_ID_SIZE);
        if(type == BOOLEAN)
        {
            char payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
            char* value = &payload[PAYLOAD_OFFSET_LENGTH];
            binary_write(value, 0, TYPE_SIZE[BOOLEAN], digital_read(pinID));
            send_command(current, READ, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH + 1);
        }
        else if(type == ANALOG_8)
        {
            char payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
            char* value = &payload[PAYLOAD_OFFSET_LENGTH];
            binary_write(value, 0, TYPE_SIZE[ANALOG_8], analog_read(pinID));
            send_command(current, READ, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH + 1);
        }
    }
}

/*** WRITE ***/
void parseWRITE(state *current, char* header, int size)
{
    char type = binary_read(header, TYPE_PARAMETER_INDEX, TYPE_PARAMETER_SIZE);
    int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
    if(mask_is_present)
    {
        char mask[MASK_LENGTH]; 
        recv_msg(mask,MASK_LENGTH);
        if(type == BOOLEAN)
        {
            char buffer= 0;
            recv_msg(&buffer,TYPE_LENGTH[BOOLEAN]);
            int i, j=0;
            for(i=0; i<NB_PINS; i++)
            {
                if(binary_read(mask, i, 1))
                {
                    int value = binary_read(&buffer, j, TYPE_SIZE[BOOLEAN]); 

                    digital_write(i,value); 

                    j++; 
                    if(j == 8)
                    {
                        j=0;
                        recv_msg(&buffer,TYPE_LENGTH[BOOLEAN]);
                    }

                }
            }
        }
        else if(type == ANALOG_8)
        {
           int i;
            for(i=0; i<NB_PINS; i++)
            {
                if(binary_read(mask, i, 1))
                {
                    char buffer = 0;
                    recv_msg(&buffer,TYPE_LENGTH[ANALOG_8]);
                    int value = binary_read(&buffer, 0, TYPE_SIZE[ANALOG_8]); 

                    pwm_write(i,value); 

                }
            }

        }
    }
    else
    {
        char buffer[PIN_ID_LENGTH];
        recv_msg(buffer,PIN_ID_LENGTH);
        int pinID = binary_read(buffer, 0, PIN_ID_SIZE);
        if(type == BOOLEAN)
        {
            char val_buffer;
            recv_msg(&val_buffer,TYPE_LENGTH[BOOLEAN]); 
            int value = binary_read(&val_buffer, 0, TYPE_SIZE[BOOLEAN]); 

            digital_write(pinID,value); 

        }
        else if(type == ANALOG_8)
        {
            char val_buffer;
            recv_msg(&val_buffer,TYPE_LENGTH[ANALOG_8]); 
            int value = binary_read(&val_buffer, 0, TYPE_SIZE[ANALOG_8]); 

            pwm_write(pinID,value); 

 
        }
    }
    char payload[PAYLOAD_OFFSET_LENGTH];
    send_command(current, WRITE, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH );
}

/*** GET_TYPE ***/
void parseGET_TYPE(state *current, char* header, int size)
{
    int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
    if(mask_is_present)
    {
        char mask[MASK_LENGTH]; 
        recv_msg(mask,MASK_LENGTH);
        char payload[PAYLOAD_OFFSET_LENGTH + NB_PINS] = {0};
        char *types  = &payload[PAYLOAD_OFFSET_LENGTH];
        int i,j=0;
        for(i=0; i<NB_PINS; i++)
        {
            if(binary_read(mask, i, 1))
            {
                binary_write(&(types[j]), TYPE_DATA_INDEX, TYPE_DATA_SIZE, current->type[i]);
                j++;
            }
            send_command(current, GET_TYPE, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH + j );
        }
    }
    else
    {
        char buffer[PIN_ID_LENGTH];
        recv_msg(buffer,PIN_ID_LENGTH);
        int pinID = binary_read(buffer, 0, PIN_ID_SIZE);

        char payload[PAYLOAD_OFFSET_LENGTH + 1] = {0};
        char* type = &payload[PAYLOAD_OFFSET_LENGTH];
        binary_write(type, TYPE_DATA_INDEX, TYPE_DATA_SIZE, current->type[pinID]);

        send_command(current, GET_TYPE, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH+1 );
    }

}

/*** SET_TYPE ***/
void parseSET_TYPE(state *current, char* header, int size)
{
    int mask_is_present = binary_read(header, MASKP_PARAMETER_INDEX, MASKP_PARAMETER_SIZE);
    if(mask_is_present)
    {
        char mask[MASK_LENGTH]; 
        recv_msg(mask,MASK_LENGTH);
        int i, j=0;
        for(i=0; i<NB_PINS; i++)
        {
            if(binary_read(mask, i, 1))
            {
                char val_buffer[TYPE_DATA_LENGTH];
                recv_msg(val_buffer,TYPE_DATA_LENGTH);
                int type = binary_read(val_buffer, TYPE_DATA_INDEX, TYPE_DATA_SIZE);

                current->type[j] = type;
                j++;
            }
        }
    }
    else
    {
        char buffer[PIN_ID_LENGTH];
        recv_msg(buffer,PIN_ID_LENGTH);
        int pinID = binary_read(buffer, 0, PIN_ID_SIZE);
        char val_buffer[TYPE_DATA_LENGTH];
        recv_msg(val_buffer,TYPE_DATA_LENGTH);
        int type = binary_read(val_buffer, TYPE_DATA_INDEX, TYPE_DATA_SIZE);

        current->type[pinID] = type;
    }
    char payload[PAYLOAD_OFFSET_LENGTH];
    send_command(current, SET_TYPE, SUCCESS, payload, PAYLOAD_OFFSET_LENGTH );
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
        parseREAD(current, header,size);
        break;
    case WRITE:
        parseWRITE(current, header,size);
        break;
    case GET_TYPE:
        parseGET_TYPE(current, header,size);
        break;
    case SET_TYPE:
        parseSET_TYPE(current, header,size);
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


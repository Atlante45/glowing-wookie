#include "protocol.h"
#include "communication.h"
#include "../common/protocol_command.h"
#include "../common/protocol_util.h"
#include "../common/bits.h"
#include "../common/mask.h"
#include <stdlib.h>
#include <stdio.h>

void send_command(char header, char *payload, int payload_length){
    int packet_length = 0;
    char *buffer = protocol__make_packet(&packet_length, header, payload, payload_length);

    send_msg(buffer, packet_length * 8);

    if (buffer != NULL)
        free( buffer);
}


/*** GET_CAPS ***/
void answerGET_CAPS()
{
    char header = 0;
    binary_write(&header, 0, COMMAND_SIZE, GET_CAPS);

    mask_t type_mask = mask__new(NB_PINS, TYPE_DATA_SIZE);

    // TODO fill typeMask with correct values

    char data[NB_PINS_LENGTH + NB_PINS * TYPE_DATA_SIZE];
    binary_write(&(data[0]), 0, NB_PINS_SIZE, NB_PINS);
    mask__to_string(type_mask, &(data[0]), NB_PINS_SIZE);

    send_command(header, &(data[0]), NB_PINS_LENGTH + NB_PINS * TYPE_DATA_SIZE);
}

void parseGET_CAPS(char* header, int size)
{
    answerGET_CAPS();
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
void answerdPING()
{
    /* TODO */
}

void parsePING(char* header, int size)
{
	char buffer[2];
	recv_msg(buffer, DATA_SIZE_LENGTH);
	recv_msg(buffer, VERSION_LENGTH);
	int version = binary_read(buffer, VERSION_INDEX, VERSION_SIZE);
	
	answerdPING();
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
void parseProtocol()
{
	char header[HEADER_LENGTH];
	recv_msg(header, HEADER_LENGTH);
	int command = binary_read(header, COMMAND_INDEX, COMMAND_SIZE);

	char buffer[DATA_SIZE_LENGTH];
	recv_msg(&buffer, DATA_SIZE_LENGTH);
	int size = binary_read(buffer, DATA_SIZE_INDEX, DATA_SIZE_SIZE);

	switch(command){
		case GET_CAPS:
			parseGET_CAPS(header, size);
			break;
		case RESET:
			parseRESET(header, size);
			break;		 
		case PING:
			parsePING(header,size);	
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
		case RESERVED:
			break;

	}
}


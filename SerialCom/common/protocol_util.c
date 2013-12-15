#include "../common/protocol_util.h"
#include "../common/protocol_command.h"
#include "../common/bits.h"
#include "../common/checksum.h"
#include <stdlib.h>

char *protocol__make_packet(int *output_length, char header, char *payload, int payload_length){
    unsigned int packet_length = HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length + CHECKSUM_LENGTH;
    char *buffer = (char*)malloc(sizeof(char)*packet_length);
    if (buffer == NULL)
        return NULL;
    int i;
    buffer[0] = header;
    binary_write(buffer, HEADER_SIZE, DATA_SIZE_SIZE, payload_length);
    for (i = 0; i < payload_length; i++)
        buffer[HEADER_LENGTH + DATA_SIZE_LENGTH + i] = payload[i];
    binary_write(buffer,
                 HEADER_SIZE + DATA_SIZE_SIZE + payload_length * 8,
                 CHECKSUM_SIZE,
                 checksum(buffer, HEADER_LENGTH + DATA_SIZE_LENGTH + payload_length));
    if (output_length != NULL)
        *output_length = packet_length;
    return buffer;
}


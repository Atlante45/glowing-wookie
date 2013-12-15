#ifndef PROTOCOL_UTIL_
#define PROTOCOL_UTIL_

char *protocol__make_packet(int *output_length, char header, char *payload, int payload_length);


#endif

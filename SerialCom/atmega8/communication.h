#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

/* Function to send and receive msg*/

void init_com();
void send_msg(char * msg, int size);
int recv_msg(char * buffer, int size);

#endif

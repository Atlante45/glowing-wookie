#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

/* Function to send and receive msg*/

void init_com();
void close_com();
void send_msg(char * msg, int size);
int recv_msg(char * buffer, int size);
int is_ready_to_read();
int is_ready_to_send();

#endif

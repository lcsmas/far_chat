#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int receiveStrMsg(int* csocket_fd, char * recv_buffer, int buffer_len);
int sendStrMsg(int* socket_fd, char * send_buffer);
void sendIntMsg(int* , int* );
void logServerMessage(char *);

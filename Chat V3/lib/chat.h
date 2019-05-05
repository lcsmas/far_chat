#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int receiveStrMsg(int* csocket_fd, char * recv_buffer, int buffer_len,int flag);
int sendStrMsg(int* socket_fd, char * send_buffer, int flag);
void sendIntMsg(int* , int* );
void logServerMessage(char *);


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct client_pool{
    int pool_size;
    int * csockfd;
    struct sockaddr_in* csin;
    int * cnum; // Client number
    int ccnum; // Number of connected client   
};

int createServer(int*, struct sockaddr_in*, int);
int serverListen(int*, int );
void clientWaitingProcess(int, struct client_pool*);
void connectClient(int, struct sockaddr_in);
void disconnectClient(struct client_pool*, const int);
void clientLoop(int);
void serverLoop(struct client_pool*);
void initClientPool(struct client_pool*, int);
void destroyClientPool(struct client_pool*);
void initClient(int *, struct sockaddr_in*);
int acceptClient(int*, struct sockaddr_in*);
int receiveStrMsg(int*, char *, int);
int sendStrMsg(int*, char *);
int recvStrMsgUDP(int*, char *, struct sockaddr_in*);
#include "../chat.h"

struct server {
    struct channel* channels; 

};

struct channel{
    char * name; // Name of the channel
    char * descr; // Description of the channel
    struct client_pool* cpool; // Client pool connected to the channel
    int size; // Size of the channel
    pthread_t * channel_thread; // Channel thread

};

struct client_pool{
    int pool_size; // Size of the client pool
    int ccnum; // Number of connected client

    char ** recv_buffer; // Clients reception buffers
    char ** send_buffer; // Clients sending buffers
    int * csockfd; // Clients socket file descriptor
    struct sockaddr_in* csin; // Clients internet informations
    int * cnum; // Clients number
    pthread_t * cthread; // Clients thread
    
};

struct threadArgs {
    int * sockfd;
    int emptySlot;
    struct client_pool* cpool;
};

void createServer(int*, struct sockaddr_in*, int);
void serverListen(int*, int );
void clientAcceptationLoopThread(struct threadArgs * args);
void clientThread(struct threadArgs * args);
void serverLoop(int *, struct client_pool*);
void initClientPool(struct client_pool*);
void destroyClientPool(struct client_pool*);
int acceptClient(int*, struct sockaddr_in*);
void disconnectClient(struct client_pool*, const int);
int isFull(struct client_pool);
int getEmptySlot(struct client_pool);
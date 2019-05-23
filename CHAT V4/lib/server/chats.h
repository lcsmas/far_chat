#include "../chat.h"

struct server {
    int sockfd; //  Socket file descriptor of the server
    struct sockaddr_in sin; //server address info;
    struct channel* channels; // Array containing all the channel
    struct client_pool* cpool; // The client pool of the server
};

struct channel{
    int channel_id; // ID of the channel
    char * name; // Name of the channel
    char * descr; // Description of the channel
    int * clients_id; // An array containing the id of the client connected to the channel
    int size; // Size of the channel
    //pthread_t * channel_thread; // Channel thread

};

struct client_pool{
    int pool_size; // Size of the client pool
    int ccnum; // Number of connected client
    char ** recv_buffer; // Clients reception buffers
    char ** send_buffer; // Clients sending buffers
    int * csockfd; // Clients socket file descriptor
    struct sockaddr_in* csin; // Clients internet informations
    int * cnum; // Clients number
    struct channel* channel; // Channel the client is connected to 
    pthread_t * cthread; // Clients thread
    
};

struct threadArgs {
    int * sockfd; 
    int emptySlot;
    struct client_pool* cpool;
    struct server* sinfo;
};

void initServer(struct server* server_info);
void initChannel(struct server* server_info);
void createServer(int*, struct sockaddr_in*, int);
void serverListen(struct server* server_info);
void clientAcceptationLoopThread(struct threadArgs * args);
void clientThread(struct threadArgs * args);
void serverLoop(int *, struct server* sinfo);
void initClientPool(struct server*);
void destroyClientPool(struct client_pool*);
int acceptClient(int*, struct sockaddr_in*);
void disconnectClient(struct client_pool*, const int);
int isFull(struct client_pool);
int getEmptySlot(struct client_pool);
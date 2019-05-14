#include "chats.h"

#if !defined PORT 
#define PORT 1025
#endif

#if !defined SERVER_ADDR 
#define SERVER_ADDR "127.0.0.1"
#endif

#if !defined CHAT_SIZE
#define CHAT_SIZE 25
#endif

#if !defined NB_CHAN
#define NB_CHAN 5
#endif

#if !defined CHANNEL_SIZE
#define CHANNEL_SIZE 5
#endif

#if !defined SIZEBUFF
#define SIZEBUFF 512
#endif

void initServer(struct server* server_info){
    initChannel(server_info);
    
}

void initChannel(struct server* server_info){
    struct channel* channels = malloc(sizeof(struct channel) * NB_CHAN);
    server_info->channels = channels;
    for(int i =0; i < NB_CHAN; i++){
        channels[i].cpool = malloc(sizeof(channels[i].cpool));
        initClientPool(channels[i].cpool); // Populate the client pool of the channel
        sprintf(channels[i].name, "Channel %d", i); // Give a name to the channel
        sprintf(channels[i].descr, "Description of channel %d", i); // Give a description to the channel
        channels[i].size = CHANNEL_SIZE;
        

    }
    initClientPool(&cpool);
}

void initClientPool(struct client_pool* cpool) {
    cpool->ccnum = 0;
    cpool->pool_size = CHAT_SIZE;

    cpool->csockfd = malloc(sizeof(int*) * CHAT_SIZE);
    cpool->csin = malloc(sizeof(struct sockaddr_in*) * CHAT_SIZE);
    cpool->cnum = malloc(sizeof(int*) * CHAT_SIZE);
    cpool->recv_buffer = malloc(sizeof(char *) * CHAT_SIZE);
    cpool->send_buffer = malloc(sizeof(char *) * CHAT_SIZE);
    cpool->cthread = malloc(sizeof(pthread_t *) * CHAT_SIZE);


    for(int i = 0; i < cpool->pool_size; i++){
        cpool->recv_buffer[i] = malloc(sizeof(char *) * SIZEBUFF);
        cpool->send_buffer[i] = malloc(sizeof(char *) * SIZEBUFF);
        cpool->csockfd[i] = -1;
    }
}

//Free malloc of client pool
void destroyClientPool(struct client_pool* cpool){
    free(cpool->csockfd);
    free(cpool->csin);
    free(cpool->cnum);
}


// Create the server with 'sin' address information, 'type' protocol (TCP or UDP) and accept any addresses
void createServer(int* socket_fd, struct sockaddr_in* sin, int type){
    const int       optVal = 1;
    const socklen_t optLen = sizeof(optVal);
    *socket_fd = socket(PF_INET, type, 0);

    if(*socket_fd == -1){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
    sin->sin_family = AF_INET;
    sin->sin_port = htons(PORT);
    sin->sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(*socket_fd, (struct sockaddr*) sin, sizeof(*sin)) == -1 ){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    printf("Server created on port %d \n", PORT);
}

// Make the server listen on the defined port (defined in preprocess directives) with the defined queue size
void serverListen(int* socket_fd, int queue_size){
    if( listen(*socket_fd, queue_size) == -1 ) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    } 
    printf("Server listening on port %d \n", PORT);
}

//The server loop
void serverLoop(int * sockfd, struct client_pool* cpool){
    pthread_t connectionHandlingThread;
    struct threadArgs t_args;

    t_args.sockfd = sockfd;
    t_args.cpool = cpool;

    // Create a thread that handle the connection of new client.
    // This thread will produce a new thread for every client when a connection is made with him
    if( pthread_create(&connectionHandlingThread, NULL, (void *)&clientAcceptationLoopThread, &t_args) == -1 ){
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    if( pthread_join(connectionHandlingThread, NULL) == -1 ){
        perror("pthread_join()");
        exit(EXIT_FAILURE);
    }
}

//Disconnect client number cnum from the server (clear it from the client pool and set his sockfd to -1)
void disconnectClient(struct client_pool* cpool, const int cnum){
    sendStrMsg(&(cpool->csockfd[cnum]), "-1"); // Notify the client that the communication is terminated
    close(cpool->csockfd[cnum]); // Close the socket
    printf("Client %d has been disconnected\n", cnum);
    cpool->csockfd[cnum] = -1; 
    cpool->ccnum = cpool->ccnum - 1;
    printf("%d\n", cpool->ccnum);
}


// Accept a TCP connection on the socket, csin is populated with the client informations requesting the connection.
// Eeturn the socket fd established with the client
int acceptClient(int* sock_fd, struct sockaddr_in* csin){
    char * cipaddr; 
    socklen_t csin_len = sizeof(*csin);
    int csocket_fd = accept(*sock_fd, (struct sockadrr * restrict)csin, &csin_len);
    if (csocket_fd == -1){
        perror("accept() failed");
    } else {
        cipaddr = inet_ntoa(csin->sin_addr);
        printf("Connection accepted with address %s \n", cipaddr);
    }
    
    return csocket_fd;
}

// Accept a client and create a thread to transfer the messages of this client to the other clients
void clientAcceptationLoopThread(struct threadArgs * args){
    int * sockfd = (int *)(args->sockfd);
    struct client_pool* cpool = (struct client_pool*)(args->cpool);
    while(1){
        if(!isFull(*cpool)){
            int emptySlot = getEmptySlot(*cpool);
            cpool->csockfd[emptySlot] = acceptClient(sockfd, &(cpool->csin[emptySlot]) );
            cpool->ccnum++;
            cpool->cnum[emptySlot] = emptySlot;
            args->emptySlot = emptySlot;

            if( pthread_create(&(cpool->cthread[emptySlot]), NULL, (void *) clientThread, args) == -1 ) {
                perror("pthread_create()");
                exit(EXIT_FAILURE);
            }

        } else {
            sleep(1);
        }
    }
}

// Transfer an incoming message of the client to all the others client
void clientThread(struct threadArgs * args){
    struct client_pool* cpool = (struct client_pool*)(args->cpool);
    int slot = (int)(args->emptySlot);

    int csockfd = cpool->csockfd[slot];
    char * recv_buffer = cpool->recv_buffer[slot];

    while(1){
        receiveStrMsg(&csockfd, recv_buffer, sizeof(recv_buffer));
        printf("From client %d\n",cpool->cnum[slot]);
        for(int i = 0; i < cpool->pool_size; i++){
            if(cpool->csockfd[i] != -1 && csockfd != cpool->csockfd[i]){
                strcpy(cpool->send_buffer[i], recv_buffer);
                sendStrMsg(&(cpool->csockfd[i]), cpool->send_buffer[i]);
                printf("To client %d \n", cpool->cnum[i]);
            }
        }
        if( strcmp("-1", recv_buffer) == 0 ){
            printf("tub\n");
            disconnectClient(cpool, cpool->cnum[slot]);
            pthread_exit(NULL);
        }
    }
}

// Return the id of the first empty slot found in the client pool
int getEmptySlot(struct client_pool cpool){
    int emptySlot = -1;
    for(int i = 0; i < cpool.pool_size; i++){
        if(cpool.csockfd[i] == -1){
            emptySlot = i;
            break;
        }
    }
    return emptySlot;
}

// Indicate if the client pool is full
int isFull(struct client_pool cpool){
    return cpool.ccnum == cpool.pool_size;
}

#include "../lib/server/chats.h"
#define CHAT_SIZE 2

int main(int argc, char* argv[]){
    int sockfd; //server socket file descriptor
    struct sockaddr_in sin; //server address info;
    struct client_pool cpool; //client pool cointaining all the client connected to the server;
    
    initClientPool(&cpool);
    createServer(&sockfd, &sin, SOCK_STREAM);
    serverListen(&sockfd, cpool.pool_size);
    serverLoop(&sockfd, &cpool);
    destroyClientPool(&cpool);
    return 0;
}


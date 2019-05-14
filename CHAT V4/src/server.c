#include "../lib/server/chats.h"
#define CHAT_SIZE 2
#define NB_CHAN 5

int main(int argc, char* argv[]){
    int sockfd; //server socket file descriptor
    struct sockaddr_in sin; //server address info;
    struct server server_info;
    //struct client_pool cpool; //client pool cointaining all the client connected to the server;
    
    initServer(&server_info);
    createServer(&sockfd, &sin, SOCK_STREAM);
    serverListen(&sockfd, cpool.pool_size);
    serverLoop(&sockfd, &cpool);
    destroyClientPool(&cpool);
    return 0;
}


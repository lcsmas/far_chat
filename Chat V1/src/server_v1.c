#include "../lib/chat.h"
#define CHAT_SIZE 2

int main(int argc, char* argv[]){
    int sockfd; //server socket file descriptor
    struct sockaddr_in sin; //server address info;
    struct client_pool cpool; //client pool cointaining all the client connected to the server;
    
    
    initClientPool(&cpool, CHAT_SIZE);
    
    while(createServer(&sockfd, &sin, SOCK_STREAM) == -1){
        sleep(1);
        printf("Retrying creation...");
    }
    
    while(serverListen(&sockfd, cpool.pool_size) == -1){
        sleep(1);
        printf("Retrying listening...");
    }
    
    while(1){
        clientWaitingProcess(sockfd, &cpool);
        sendStrMsg(&(cpool.csockfd[0]), "[SERVER INSTRUCTION] You start\n");
        serverLoop(&cpool);
    }
    
    destroyClientPool(&cpool);
    return 0;
}


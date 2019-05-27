#include "../lib/server/chats.h"

int main(int argc, char* argv[]){
    struct server* server_info = malloc(sizeof(struct server*));
    int * sockfd = &(server_info->sockfd);
    struct sockaddr_in sin = server_info->sin;

    initServer(server_info);
    createServer(sockfd, &sin, SOCK_STREAM);
    serverListen(server_info);
    serverLoop(sockfd, server_info);
    destroyClientPool(server_info->cpool);
    return 0;
}


#include "chat.h"
#define PORT 1025
#define SERVER_ADDR "127.0.0.1"


void initClientPool(struct client_pool* cpool, int pool_size) {
    cpool->ccnum = 0;
    cpool->pool_size = pool_size;

    cpool->csockfd = malloc(sizeof(int*) * pool_size);
    cpool->csin = malloc(sizeof(struct sockaddr_in*) * pool_size);
    cpool->cnum = malloc(sizeof(int*) * pool_size);

    for(int i = 0; i < cpool->pool_size; i++){
        cpool->csockfd[i] = -1;
    }
}

//Free malloc of client pool
void destroyClientPool(struct client_pool* cpool){
    free(cpool->csockfd);
    free(cpool->csin);
    free(cpool->cnum);
}


//Initialize the client
//sockfd is the client socket
//sin is the future internets informations of the server, it is filled by this function
void initClient(int * sockfd, struct sockaddr_in* sin){
    *sockfd = socket(PF_INET,SOCK_STREAM,0);
	sin->sin_family = AF_INET;
	sin->sin_port = htons(PORT);
	int res = inet_pton(AF_INET, SERVER_ADDR, &(sin->sin_addr));
	if(res < 0){
		perror("inet_pton() failed");
	}
}

//Connect the client to the chat. 
//sockfd is the client socket
//sin is the server's internets informations previously filled by initClient()
void connectClient(int sockfd, struct sockaddr_in sin){
    char recv_buffer[32];
    socklen_t IgA = sizeof(sin);
    while( connect(sockfd,(struct sockaddr *) &sin, IgA) == -1 ){
        perror("connect() failed");
        printf("Retrying connection...");
        sleep(1);
    }
    printf("Connection done !\n");
    receiveStrMsg(&sockfd, recv_buffer, sizeof(recv_buffer));
    printf("[SERVER ANSWER] %s\n",recv_buffer);
}


// Create the server with 'sin' address information, 'type' protocol (TCP or UDP) and accept any addresses
int createServer(int* socket_fd, struct sockaddr_in* sin, int type){
    const int       optVal = 1;
    const socklen_t optLen = sizeof(optVal);
    *socket_fd = socket(PF_INET, type, 0);

    if(*socket_fd == -1){
        perror("socket() failed");
        return -1;
    }

    setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
    sin->sin_family = AF_INET;
    sin->sin_port = htons(PORT);
    sin->sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(*socket_fd, (struct sockaddr*) sin, sizeof(*sin)) == -1 ){
        perror("bind() failed");
        return -1;
    }

    printf("Server created on port %d \n", PORT);

    return 0;
}

// Make the server listen on the defined port (defined in preprocess directives) with the defined queue size
int serverListen(int* socket_fd, int queue_size){
    
    int l = listen(*socket_fd, queue_size);
    if( l == -1 ) {
        perror("listen() failed");
    } else {
        printf("Server listening on port %d \n", PORT);
    }
    
    return l;
}

//The client waiting process
void clientWaitingProcess(int sockfd, struct client_pool* cpool){
    
    int i = cpool->pool_size;
    char send_buffer[32];
    while(cpool->ccnum < cpool->pool_size){
        const int CURR_C = i % cpool->pool_size; // CURR_C = Current client : the client we are waiting the message from;
        if(cpool->csockfd[CURR_C] == -1) {
            cpool->csockfd[CURR_C] = acceptClient(&sockfd, &(cpool->csin[CURR_C]));
            if( cpool->csockfd[CURR_C] != -1 ){
                cpool->cnum[CURR_C] = CURR_C;
                cpool->ccnum++;
                sprintf(send_buffer, "Bonjour client %d", cpool->cnum[CURR_C]);
                sendStrMsg(&(cpool->csockfd[CURR_C]), send_buffer);
                
            } 
        }
        i++;
    }
}

//The chat loop
void serverLoop(struct client_pool* cpool){
    char recv_buffer[32];
    int i = cpool->pool_size;
    while(1){
        const int CURR_C = i % cpool->pool_size;
        int recv_size = receiveStrMsg(&(cpool->csockfd[CURR_C]), recv_buffer, sizeof recv_buffer);
        if(recv_size == -1){
            perror("recv() failed");
            sleep(1);
        } else {
            printf("Message du client %d : %s", cpool->cnum[CURR_C], recv_buffer);
            i++;
            int nextC = i % cpool->pool_size;
            sendStrMsg(&(cpool->csockfd[nextC]), recv_buffer);
            printf("%s\n", recv_buffer);
            if(strcmp(recv_buffer, "-1\n") == 0 ){
                disconnectClient(cpool, CURR_C);
                break;
            }
        }
    }
}

//Disconnect client number cnum from the server (clear it from the client pool and set his sockfd to -1)
void disconnectClient(struct client_pool* cpool, const int cnum){
    close(cpool->csockfd[cnum]);
    printf("Client %d has been disconnected\n", cnum);
    cpool->csockfd[cnum] = -1;
    cpool->ccnum = cpool->ccnum - 1;
    printf("%d\n", cpool->ccnum);
}

//The client loop
void clientLoop(int sockfd){
    while(1){
        char send_buffer[32];
        char recv_buffer[32];
		receiveStrMsg(&sockfd, recv_buffer, sizeof(recv_buffer)); // Reçoit le message du serveur
		printf("%s\n", recv_buffer); // Affiche ce message
        if(strcmp(recv_buffer, "-1\n") == 0) break;
		printf("Moi: ");
		fgets(send_buffer,sizeof(send_buffer),stdin); // Récupère le message à envoyer

		sendStrMsg(&sockfd,send_buffer); // Envoie ce message au serveur
        if(strcmp(send_buffer, "-1\n") == 0){
            exit(EXIT_SUCCESS);
        }
	}
}

// Send an int message to the destination
void sendIntMsg(int* socket_fd, int* send_buffer){
    if( send(*socket_fd, send_buffer, sizeof(*send_buffer), 0) == -1 ){
        perror("send() failed");
    }
}

// Send a string message to the to destination
int sendStrMsg(int* socket_fd, char * send_buffer){
    int send_size = send(*socket_fd, send_buffer, strlen(send_buffer)+1, 0);
    if( send_size == -1 ){
        perror("send() failed");
    } else
    {
        printf("Message sent : %s\n", send_buffer);
    }
    
    return send_size;
}

// Receive a string message from the client
int receiveStrMsg(int* csocket_fd, char * recv_buffer, int buffer_len){
    int recv_size = recv(*csocket_fd, recv_buffer, buffer_len, 0);
    if(recv_size == -1){
        perror("recv() failed");
    } 
    return recv_size;
}

//Receive string message from a client with UDP protocol
int recvStrMsgUDP(int* csockfd, char * recv_buffer, struct sockaddr_in* sin){
    socklen_t slen = sizeof(*sin);
    char * recv_buffer_tmp = recv_buffer;
    int recv_size = recvfrom(*csockfd, recv_buffer_tmp, sizeof(recv_buffer_tmp), 0, (struct sockaddr*)sin, &slen);
    if (recv_size == -1){
        perror("recvfrom() failed");
    }
    return recv_size;
}

// Accept a TCP connection from a client, return the socket established with the client
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


#include "chatc.h"

#if !defined PORT 
#define PORT 1025
#endif

#if !defined SERVER_ADDR 
#define SERVER_ADDR "127.0.0.1"
#endif

#if !defined SIZEBUFF
#define SIZEBUFF 512
#endif

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
    char recv_buffer[SIZEBUFF];
    socklen_t IgA = sizeof(sin);
    if( connect(sockfd,(struct sockaddr *) &sin, IgA) == -1 ){
        exit(EXIT_FAILURE);
    }
    
    logServerMessage("Connection successful !\n");
}

//The client loop
void clientLoop(int sockfd){
    pthread_t readThread;
    pthread_t writeThread;

    if (pthread_create(&readThread, NULL, (void*)readingLoop, &sockfd) == -1){
        perror("pthread_create()");
        exit(EXIT_FAILURE);        
    }

    if (pthread_create(&writeThread, NULL, (void *)writingLoop, &sockfd) == -1){
        perror("pthread_create()");
        exit(EXIT_FAILURE);        
    }
    
    if (!pthread_join(readThread,NULL) || !pthread_join(writeThread,NULL)){
        exit(EXIT_SUCCESS);
    } else {
        perror("pthread_join()");
        exit(EXIT_FAILURE);
    }
}

//The client reading loop
void *readingLoop(int * sockfd){
    while(1){
        char recv_buffer[SIZEBUFF];
        receiveStrMsg(sockfd, recv_buffer, sizeof(recv_buffer)); // Reçoit le message du serveur et l'affiche
        if(strcmp(recv_buffer, "-1\n") == 0){
            printf("%s\n", "Distant client terminated the communication");
            pthread_exit(NULL);
        }
        
    }
}


//The client writing loop
void *writingLoop(int * sockfd){
    char send_buffer[SIZEBUFF];
    while(1){
        printf("Moi: ");
        fgets(send_buffer,sizeof(send_buffer),stdin); // Récupère le message à envoyer
        sendStrMsg(sockfd,send_buffer); // Envoie ce message au serveur
        if(strcmp(send_buffer, "-1\n") == 0){
            printf("%s\n", "You terminated the communication");
            pthread_exit(NULL);
        }
        
    }
}
#include "chatc.h"
#include <dirent.h>
#if !defined PORT 
#define PORT 1025
#endif

#if !defined SERVER_ADDR 
#define SERVER_ADDR "127.0.0.1"
#endif

#if !defined SIZEBUFF
#define SIZEBUFF 1023
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
        printf("\n");;
        perror("pthread_create()");
        exit(EXIT_FAILURE);        
    }

    if (pthread_create(&writeThread, NULL, (void *)writingLoop, &sockfd) == -1){
        perror("pthread_create()");
        exit(EXIT_FAILURE);        
    }
    
    if (!pthread_join(writeThread,NULL)){
        exit(EXIT_SUCCESS);
    } else {
        perror("pthread_join()");
        exit(EXIT_FAILURE);
    }
}

int recvFile(int * sockfd){
    char recv_buffer[SIZEBUFF];
    char fileName[SIZEBUFF];
    char fichier[SIZEBUFF]="";
    receiveStrMsg(sockfd, fileName, sizeof(fileName),1); // Reçoit un fichier
    strcat(fichier,"./lib/client/dest/");
    printf("fichier\n");
    strcat(fichier,fileName);
    FILE * fe;
    fe = fopen(fichier,"wb+");
    if (fe == NULL){
        perror("Error while opening the file.\n");
        return 1;
    }
    receiveStrMsg(sockfd, recv_buffer, sizeof(recv_buffer),1); // Reçoit un fichier
    while (strcmp("-2\n", recv_buffer) != 0){
        fprintf(fe, "%s",recv_buffer);
        receiveStrMsg(sockfd, recv_buffer, sizeof(recv_buffer),1); // Reçoit un fichier
    }
    fclose(fe);
    return 0;

}

//The client reading loop
void *readingLoop(int * sockfd){
    char recv_buffer[SIZEBUFF];
    while(1){
        receiveStrMsg(sockfd, recv_buffer, sizeof(recv_buffer),0); // Reçoit le message du serveur et l'affiche
        if(strcmp(recv_buffer, "-1\n") == 0){
            printf("%s\n", "Distant client terminated the communication\n");
        }
        if(strcmp(recv_buffer, "file\n") == 0){
            int res = recvFile(sockfd);
            if (res==0){
                printf("Fichier reçu avec succes\n");
            }
            else{
                printf("Le fichier n'a pas pu être reçu\n");
            }    
        }
    }
}

int sendFile(int *sockfd){
    char send_buffer[SIZEBUFF];
    DIR *dp;
    struct dirent *ep; 
    dp = opendir ("./lib/client/source/");
    if (dp != NULL) {
        printf("Liste des fichiers transferable:\n");
        while (ep = readdir (dp)) {
            if(strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0) {
                printf("%s\n",ep->d_name);
            }
        }    
        closedir (dp);
      }
    else {
        perror ("Ne peux pas ouvrir le répertoire");
        return 1;
    }
    printf("Indiquer le nom du fichier : ");
    char fileName[SIZEBUFF];
    char fichier[SIZEBUFF]="";
    fgets(fileName,sizeof(fileName),stdin);
    fileName[strlen(fileName)-1]='\0';
    sleep(3);
    sendStrMsg(sockfd,fileName,1); 
    strcat(fichier,"./lib/client/source/");
    strcat(fichier,fileName);
    FILE * fe;
    fe = fopen(fichier,"rb");
    if (fe == NULL){
        perror("Error while opening the file.\n");
        return 1;
    }
    while (fgets(send_buffer,SIZEBUFF,fe) != NULL){
        sendStrMsg(sockfd,send_buffer,1); 
    }
    char * fin = "-2\n";
    sendStrMsg(sockfd,fin,1); 
    fclose(fe);
    return 0;
}

//The client writing loop
void *writingLoop(int * sockfd){
    char send_buffer[SIZEBUFF];
    while(1){
        printf("Moi: ");
        fgets(send_buffer,sizeof(send_buffer),stdin); // Récupère le message à envoyer
        sendStrMsg(sockfd,send_buffer,0); // Envoie ce message au serveur
        if(strcmp(send_buffer, "file\n") == 0){
            if (sendFile(sockfd)==0){
                printf("Fichier envoyer avec succes\n");
            }
            else{
                printf("Le fichier n'a pas pu être envoyer\n");
            }
        }
        if(strcmp(send_buffer, "-1\n") == 0){
            printf("%s\n", "You terminated the communication");
            pthread_exit(NULL);
        }
    }
}


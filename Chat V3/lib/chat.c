#include "chat.h"

#if !defined PORT 
#define PORT 1025  
#endif

#if !defined SERVER_ADDR 
#define SERVER_ADDR "127.0.0.1"
#endif



// Send an int message to the destination
void sendIntMsg(int* socket_fd, int* send_buffer){
    if( send(*socket_fd, send_buffer, sizeof(*send_buffer), 0) == -1 ){
        perror("send() failed");
    }
}

// Send a string message to the to destination and print it
int sendStrMsg(int* socket_fd, char * send_buffer,int flag){
    int send_size = send(*socket_fd, send_buffer, strlen(send_buffer)+1, 0);
    if( send_size == -1 ){
        perror("send() failed");
    } else
    {
        if (flag == 0)        {
            printf("\nMessage sent : %s\n", send_buffer);
        }
    }
    
    return send_size;
}

// Receive a string message from the client and print it
int receiveStrMsg(int* csocket_fd, char * recv_buffer, int buffer_len,int flag){
    int recv_size = recv(*csocket_fd, recv_buffer, buffer_len, 0);
    if(recv_size == -1){
        perror("recv() failed");
    } else {
        if (flag == 0){
            printf("\nMessage received : %s", recv_buffer);
        }
    }
    return recv_size;
}


// Log a server message in the console
void logServerMessage(char * msg){
    printf("[SERVER MESSAGE] %s\n", msg);
}
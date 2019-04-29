#include "../lib/client/chatc.h"

int main(int argc,char* argv[]){

	int dSock;
	struct sockaddr_in adServ;
	char recv_buffer[32];
	char send_buffer[32];

	initClient(&dSock, &adServ);
	connectClient(dSock, adServ);
	clientLoop(dSock);

	return 0;
}
#include "../chat.h"

void initClient(int *, struct sockaddr_in*);
void connectClient(int, struct sockaddr_in);
void clientLoop(int);
void * readingLoop(int*);
void * writingLoop(int*);
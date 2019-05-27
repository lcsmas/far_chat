all : client server clean

client : client.o chat.o chatc.o
	gcc -g -o client client.o chat.o chatc.o -pthread

client.o : src/client.c
	gcc -g -o client.o -c src/client.c

server : server.o chats.o chat.o
	gcc -g -o server server.o chat.o chats.o -pthread

server.o : src/server.c
	gcc -g -o server.o -c src/server.c

chatc.o : lib/client/chatc.c 
	gcc -g -o chatc.o -c lib/client/chatc.c 

chats.o : lib/server/chats.c 
	gcc -g -o chats.o -c lib/server/chats.c 

chat.o : lib/chat.c 
	gcc -g -o chat.o -c lib/chat.c 

clean : 
	rm -rf *.o
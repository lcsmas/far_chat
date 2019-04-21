all : client server clean

client : client.o chat.o chatc.o
	gcc -o client client.o chat.o chatc.o -pthread

client.o : src/client.c
	gcc -o client.o -c src/client.c

server : server.o chats.o chat.o
	gcc -o server server.o chat.o chats.o -pthread

server.o : src/server.c
	gcc -o server.o -c src/server.c

chatc.o : lib/client/chatc.c 
	gcc -o chatc.o -c lib/client/chatc.c 

chats.o : lib/server/chats.c 
	gcc -o chats.o -c lib/server/chats.c 

chat.o : lib/chat.c 
	gcc -o chat.o -c lib/chat.c 

clean : 
	rm -rf *.o
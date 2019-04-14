all : client_v1 server_v1 clean

client_v1 : client_v1.o chat.o
	gcc -o client_v1 client_v1.o chat.o

client_v1.o : src/client_v1.c
	gcc -o client_v1.o -c src/client_v1.c

server_v1 : server_v1.o chat.o
	gcc -o server_v1 server_v1.o chat.o

server_v1.o : src/server_v1.c
	gcc -o server_v1.o -c src/server_v1.c

chat.o : lib/chat.c
	gcc -o chat.o -c lib/chat.c

clean : 
	rm -rf *.o
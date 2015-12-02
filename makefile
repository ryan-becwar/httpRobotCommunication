CC = gcc
CFLAGS = -Wall
C = client
P = proxy

all: 1

1: client1 proxy1

2: client2 proxy2

client1: $C/UDPEchoClient1.c
	$(CC) $(CFLAGS) UDPEchoClient.c -o client

proxy1: $P/DieWithError.c $P/getfile.c $P/getfile.h $P/UDPEchoServer.c
	$(CC) $(CFLAGS) UDPEchoServer.c -o proxy
	
client2:

proxy2:

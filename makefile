CC = gcc
CFLAGS = -Wall
C = client
P = proxy

all: 1

1: client1 proxy1

2: client2 proxy2

client1: $C/client.c $C/DieWithError.c
	$(CC) $(CFLAGS) $C/client.c $C/DieWithError.c -o client

proxy1: $P/DieWithError.c $P/proxy.h $P/protocol.h $P/proxy.c $P/communicate.c
	$(CC) $(CFLAGS) $P/proxy.c $P/DieWithError.c $P/communicate.c -o proxy
	
client2:

proxy2:

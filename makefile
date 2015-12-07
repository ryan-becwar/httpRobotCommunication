CC = gcc
CFLAGS = -Wall
C = clientSource
P = proxy

all: client1 proxy1

client1: $C/client.c $C/DieWithError.c
	$(CC) $(CFLAGS) $C/client.c $C/DieWithError.c -o client

proxy1: $P/DieWithError.c $P/proxy.h $P/protocol.h $P/proxy.c $P/communicate.c
	$(CC) $(CFLAGS) $P/proxy.c $P/DieWithError.c $P/communicate.c -o server
clean:
		rm -rf ${CLEANFILES} laser*.txt output*.txt *~

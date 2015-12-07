CC = gcc
CFLAGS = -Wall
C = client
P = proxy

all: client1 proxy1

client1: $C/client.c $C/DieWithError.c
	$(CC) $(CFLAGS) $C/client.c $C/DieWithError.c -o clientExe

proxy1: $P/DieWithError.c $P/proxy.h $P/protocol.h $P/proxy.c $P/communicate.c
	$(CC) $(CFLAGS) $P/proxy.c $P/DieWithError.c $P/communicate.c -o proxyExe

clean:
		rm -rf ${CLEANFILES} *~

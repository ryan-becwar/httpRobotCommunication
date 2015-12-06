#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <errno.h>
#include <sys/types.h>
#include <string.h>     /* for memset() */
#include <netdb.h>      /* for getHostByName() */
#include <stdbool.h>

void DieWithError(char *errorMessage);
char* communicate(int requestType, char* host, int robot_number, char* robot_id, int data);

#define ECHOMAX 300
#define RCVBUFSIZE 32   /* Size of receive buffer */

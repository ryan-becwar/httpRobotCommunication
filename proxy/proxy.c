#include "proxy.h"

int main(int argc, char *argv[]){

	/*
	===========UDP SERVER SETUP==============
	*/
	int udpSock;                        /* Socket */
	struct sockaddr_in udpServAddr; /* Local address */
	struct sockaddr_in udpClntAddr; /* Client address */
	unsigned int udpCliAddrLen;         /* Length of incoming message */
	char udpBuffer[ECHOMAX];        /* Buffer for echo string */
	unsigned short udpServPort = 8080;     /* Server port */
	int recvMsgSize;                 /* Size of received message */



	/*
	===========TCP CLIENT SETUP =============
	*/
	int tcpSock;                        /* Socket descriptor */
	struct sockaddr_in robotAddr; /* Echo server address */
	int robotPort = -1;     /* robot port */
	char filename[30];             /* The save name for output */
	char fileRequest[100];
	char* doc = NULL;
	char* host = "localhost";
	char echoString[300];                /* String to send to echo server */
	char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
	int bytesRec;   /* Bytes read in single recv()
                     and total bytes read */
	char* robotId = "default";
	int robotNumber = 0; /* Used to get pictures */

	
	//Readin args
	int i;
	for(i=1; i < argc-1; i += 2){
		char *flag = argv[i];
		char *arg = argv[i+1];
		fprintf(stderr, "Flag: %s Arg: %s\n", flag, arg);
		if(strcmp(flag, "-h") == 0)
			host = arg;
		else if(strcmp(flag, "-i") == 0)
			robotId = arg;
		else if(strcmp(flag, "-n") == 0)
			robotNumber = atoi(arg);
		else if(strcmp(flag, "-p") == 0)
			udpServPort = atoi(arg);
		else
			DieWithError("Invalid flags");
	}

	//Test print out of arguments
	fprintf(stderr, "host: %s  robotId: %s robotNumber: %i udpServPort: %i\n",
		host, robotId, robotNumber, udpServPort);
	return 0;
}

#include "proxy.h"
#include "protocol.h"

char* dataToHeader(headerData data, char* header) {
  uint32_t *writeLoc;
  writeLoc = (uint32_t*)(&writeLoc[IDENTIFIER_LOC]);
  *writeLoc = data.protocolID;
  writeLoc = (uint32_t*)(&writeLoc[PASSWORD_LOC]);
  *writeLoc = data.password;
  writeLoc = (uint32_t*)(&writeLoc[CLIENT_REQUEST_LOC]);
  *writeLoc = data.clientRequest;
  writeLoc = (uint32_t*)(&writeLoc[REQUEST_DATA_LOC]);
  *writeLoc = data.requestData;
  writeLoc = (uint32_t*)(&writeLoc[BYTE_OFFSET_LOC]);
  *writeLoc = data.byteOffset;
  writeLoc = (uint32_t*)(&writeLoc[TOTAL_SIZE_LOC]);
  *writeLoc = data.totalSize;
  writeLoc = (uint32_t*)(&writeLoc[PAYLOAD_SIZE_LOC]);
  *writeLoc = data.payloadSize;

  return header;
}

void bufToData(uint32_t *header, char* buf){
  int i;
  for(i=0; i<7; i++){
    header[i] = *(uint32_t *)&buf[i*4];
    printf("header: %d %d\n", header[i], i);
  }
}

int main(int argc, char *argv[]){

  /*
    ===========UDP SERVER VARS==============
  */
  int udpSock;                        /* Socket */
  struct sockaddr_in udpServAddr;     /* Local address */
  struct sockaddr_in udpClntAddr;     /* Client address */
  unsigned int udpCliAddrLen;         /* Length of incoming message */
  char udpBuffer[ECHOMAX];            /* Buffer for echo string */
  unsigned short udpServPort = 8080;  /* Server port */
  int recvMsgSize;                    /* Size of received message */
  int password = rand() % 100;
  char packetBuffer[300];
  headerData hData;	
  uint32_t header[7];

  /*
    ===========TCP CLIENT VARS =============
  */
  int tcpSock;                        /* Socket descriptor */
  struct sockaddr_in robotAddr;       /* Echo server address */
  int robotPort = -1;                 /* robot port */
  char filename[30];                  /* The save name for output */
  char fileRequest[100];
  char* doc = NULL;
  char* host = "localhost";
  char echoString[300];               /* String to send to echo server */
  char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
  int bytesRec;                       /* Bytes read in single recv() and total bytes read */
  char* robotId = "default";
  int robotNumber = 0;                /* Used to get pictures */

	
  //Reading args
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

  /*
    ===========UDP SERVER SETUP===============
  */

  /* Create socket for sending/receiving datagrams */
  if ((udpSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");

  /* Construct local address structure */
  memset(&udpServAddr, 0, sizeof(udpServAddr));    /* Zero out structure */
  udpServAddr.sin_family = AF_INET;                /* Internet address family */
  udpServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  udpServAddr.sin_port = htons(udpServPort);       /* Local port */

  /* Bind to the local address */
  printf("UDPEchoServer: About to bind to port %d\n", udpServPort);    
  if (bind(udpSock, (struct sockaddr *) &udpServAddr, sizeof(udpServAddr)) < 0)
    DieWithError("bind() failed");

  for (;;) /* Run forever */
    {
      /* Set the size of the in-out parameter */
      udpCliAddrLen = sizeof(udpClntAddr);

      /* Block until receive message from a client */
      if ((recvMsgSize = recvfrom(udpSock, packetBuffer, ECHOMAX, 0, (struct sockaddr *) &udpClntAddr, &udpCliAddrLen)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}

      printf("Handling client %s\n", inet_ntoa(udpClntAddr.sin_addr));

      /* Handle session with the new client*/
      //Send password to client
      bufToData(header, packetBuffer);

      if(header[IDENTIFIER_LOC] != 0)
	{
	  DieWithError("Invalid protocol");
	}

      //Set password
      printf("Password: %d\n", password);
      header[PASSWORD_LOC] = password;
		
      if(sendto(udpSock, header, 28, 0, (struct sockaddr *) &udpClntAddr, sizeof(udpClntAddr)) != 28)
	{
	  DieWithError("Sent wrong number of password bytes");
	}

      printf("Sent first response with password\n");
		
      bool exit = false;
      while(!exit)
	{
	  if((recvMsgSize = recvfrom(udpSock, packetBuffer, 300, 0, (struct sockaddr *) &udpClntAddr, &udpCliAddrLen)) < 0)
	    {
	      DieWithError("Recieving packet failed");
	    }

	  //Read the header from the buffer
	  bufToData(header, packetBuffer);
	  if(header[IDENTIFIER_LOC] != 0)
	    {
	      DieWithError("Invalid protocol");
	    }

	  if(header[PASSWORD_LOC] != password)
	    {
	      DieWithError("Invalid password");
	    }

	  if(header[CLIENT_REQUEST_LOC] == 255)
	    {
	      exit = true;
	      password++;
	    }
	  else
	    {
	      //communicate(header[CLIENT_REQUEST_LOC], host, robotNumber, robotId, header[REQUEST_DATA_LOC]);
	      printf("Handling request %d\n", header[CLIENT_REQUEST_LOC]);
	    }
	}
		
    }
  /* NOT REACHED */	
	
  return 0;
}

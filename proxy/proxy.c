#include "proxy.h"
#include "protocol.h"

char* dataToHeader(headerData data, char* header) 
{
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

void bufToData(uint32_t *header, char* buf)
{
  int i;
  for(i=0; i<7; i++)
    {
      header[i] = *(uint32_t *)&buf[i*4];
      //printf("header: %d %d\n", header[i], i);
    }
}

void dataToBuf(uint32_t *header, char* buf)
{
  int i;
  for(i=0; i<28; i++) 
    {
      buf[i] = *(char *)&header[i];
      //printf("buf %d: %c\n", i, buf[i]);
    }
}

int main(int argc, char *argv[])
{
  /*
    ===========UDP SERVER VARS==============
  */
  int udpSock;                        /* Socket */
  struct sockaddr_in udpServAddr;     /* Local address */
  struct sockaddr_in udpClntAddr;     /* Client address */
  unsigned int udpCliAddrLen;         /* Length of incoming message */
  unsigned short udpServPort = 8080;  /* Server port */
  int recvMsgSize;                    /* Size of received message */
  int password = rand() % 100;
  char packetBuffer[300];
  headerData hData;	
  uint32_t header[7];
  struct stat *fileinfo;
  int filesize;
	char characterHeader[28];
  int bytesRead=0;
  int totalBytesRead=0;
  

  /*
    ===========TCP CLIENT VARS =============
  */
  int tcpSock;                        /* Socket descriptor */
  struct sockaddr_in robotAddr;       /* Echo server address */
  int robotPort = -1;                 /* robot port */
  char filename[30];                  /* The save name for output */
  char fileRequest[100];
  char* doc = NULL;
  char* host = "169.55.155.236";
  char echoString[300];               /* String to send to echo server */
  char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
  int bytesRec;                       /* Bytes read in single recv() and total bytes read */
  char* robotId = "44procal";
  int robotNumber = 1;                /* Used to get pictures */

	
  //Reading args
  int i;
  for(i=1; i < argc-1; i += 2)
{
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

  /*
    ===========UDP SERVER SETUP===============
  */

  /* Create socket for sending/receiving datagrams */
  if ((udpSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
      DieWithError("socket() failed");
    }

  /* Construct local address structure */
  memset(&udpServAddr, 0, sizeof(udpServAddr));    /* Zero out structure */
  udpServAddr.sin_family = AF_INET;                /* Internet address family */
  udpServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  udpServAddr.sin_port = htons(udpServPort);       /* Local port */

  /* Bind to the local address */
  printf("UDPProxyServer: About to bind to port %d\n", udpServPort);    
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
	  DieWithError("Did not send correct number of bytes");
	}
      printf("Sent first response with password\n");	

      if ((recvMsgSize = recvfrom(udpSock, packetBuffer, ECHOMAX, 0, (struct sockaddr *) &udpClntAddr, &udpCliAddrLen)) < 0)
	{
	  DieWithError("Failed to receive ack");
	}
	
	//While connected to currentClient
      bool exit = false;
      while(!exit)
	{
	  if((recvMsgSize = recvfrom(udpSock, packetBuffer, 300, 0, (struct sockaddr *) &udpClntAddr, &udpCliAddrLen)) < 0)
	    {
	      DieWithError("Receiving packet failed");
	    }

	  //Read the header from the buffer
	  bufToData(header, packetBuffer);
	  if(header[IDENTIFIER_LOC] != 0)
	    {
	      DieWithError("Invalid protocol");
	    }

	  if(header[PASSWORD_LOC] != password)
	    {
	      	  header[CLIENT_REQUEST_LOC] = 256;

		  if(sendto(udpSock, header, 28, 0, (struct sockaddr *) &udpClntAddr, sizeof(udpClntAddr)) != 28)
		    {
		      DieWithError("Error message could not be sent");
		    }
	    }

	  if(header[CLIENT_REQUEST_LOC] == 255)
	    {
	      exit = true;
	      password++;
	      printf("Quitting client. New password is %d\n", password);
	    }
	  else
	    {
	      sprintf(filename, "%s", communicate(header[CLIENT_REQUEST_LOC], host, robotNumber, robotId, header[REQUEST_DATA_LOC]));
	      FILE *f = fopen(filename, "rb");

	      if(strcmp(filename, "fail") == 0)
		{
		  header[CLIENT_REQUEST_LOC] = 512;

		  if(sendto(udpSock, header, 28, 0, (struct sockaddr *) &udpClntAddr, sizeof(udpClntAddr)) != 28)
		    {
		      DieWithError("Error message could not be sent");
		    }
		}
	      else
		{
	      	  fseek(f, 0L, SEEK_END);
		  filesize = ftell(f);
		  fseek(f, 0L, SEEK_SET);
		  header[TOTAL_SIZE_LOC] = filesize;
		  header[BYTE_OFFSET_LOC] = totalBytesRead;

		  dataToBuf(header, characterHeader);
		  strncpy(packetBuffer, characterHeader, 28);
		  fseek(f, totalBytesRead, SEEK_SET);
		  bytesRead = fread(packetBuffer+28, 1, 272, f);
		  //printf("Sending data. Total read = %d\n", totalBytesRead); 
		  if(sendto(udpSock, packetBuffer, 28+bytesRead, 0, (struct sockaddr *) &udpClntAddr, sizeof(udpClntAddr)) != bytesRead+28)
		    {
		      DieWithError("Error message could not be sent");
		    }
		  totalBytesRead+=bytesRead;

	      	  printf("Handling request %d\n", header[CLIENT_REQUEST_LOC]);
		  while(totalBytesRead<filesize)
		    {
		      //printf("totalBytesRead is %d and filesize is %d\n",  totalBytesRead, filesize);

		      header[BYTE_OFFSET_LOC] = totalBytesRead;
		      dataToBuf(header, characterHeader);
		      strncpy(packetBuffer, characterHeader, 28);
		      fseek(f, totalBytesRead, SEEK_SET);
		      bytesRead = fread(packetBuffer+28, 1, 272, f);

		      //printf("Sending data. Total read = %d\n", totalBytesRead); 
		      //printf("sending %d bytes\n",  bytesRead);
		      if(sendto(udpSock, packetBuffer, 28+bytesRead, 0, (struct sockaddr *) &udpClntAddr, sizeof(udpClntAddr)) != bytesRead+28)
			{
			  DieWithError("Error message could not be sent");
			}
		      memset(packetBuffer, 0, 300);
		      totalBytesRead+=bytesRead;
		    }

		  //printf("\n%d read but filesize is %d\n", totalBytesRead, filesize);

		  totalBytesRead=0;
		}
	    }
	}
		
    }
  /* NOT REACHED */	
	
  return 0;
}

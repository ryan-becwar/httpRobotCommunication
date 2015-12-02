/*********************************************************

ROBOT CONTROL CLIENT

AUTHOR: MATT KARKOWSKI

This client connects to a specified server, which serves as a proxy for a 
pre-determined robot. This client sends requests to the proxy, which
are then forwarded to the robot. The client sends requests which
cause the robot to move in the shape of a polygon with a
user specified number of sides with a user defined size length.

*********************************************************/
#include "UDPEcho.h"
#include <signal.h>

void clientCNTCCode();

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    struct hostent *thehost;         /* Hostent from gethostbyname() */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int respStringLen;               /* Length of received response */
    int n;
    int l;
    char **request;
    char header[28] = {0};
    char pass[4];

    n = 4;
    l = 1;

    int i;
    for(i=1; i<=argc, i++)
      {
	if(strcmp(argv[i], "-h") == 0)
	  {
	    servIP = argv[i+1];
	  }
	else if(strcmp(argv[i], "-p") == 0)
	  {
	    echoServPort = atoi(argv[i+1]);
	  }
	else if(strcmp(argv[i], "-n") == 0)
	  {
	    n = atoi(argv[i+1]);
	    if(n < 4 || n > 8)
	      {
		DieWithError("Invalid number of sides");
	      }	
	  }
	else if(strcmp(argv[i], "-l") == 0)
	  {
	    l = atoi(argv[i+1]);
	  }
      }

    signal (SIGINT, clientCNTCCode);

    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoString = argv[2];       /* Second arg: string to echo */

    if (argc == 4)
      {
        echoServPort = atoi(argv[3]);  /* Use given port, if any */
      }
    else
      {
        echoServPort = 7;  /* 7 is the well-known port for the echo service */
      }

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      {
        DieWithError("socket() failed");
      }

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

    /* If user gave a dotted decimal address, we need to resolve it  */
    if (echoServAddr.sin_addr.s_addr == -1) 
      {
        thehost = gethostbyname(servIP);
	echoServAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
      }

    /* GET THE PASSWORD */
    if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
      {
	DieWithError("sendto() sent a different number of bytes than expected");
      }

    if ((respStringLen = recvfrom(sock, pass, 4, 0, (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
      {
        DieWithError("recvfrom() failed");
      }

    int q;
    for(q=4; q<8; q++)
      {
	header[q] = pass[q-4];
      }

    /* CALCULATE WHAT TO SEND */
    int x;
    for(i=0; i<n; i++)
      {
	header[11] = 2; //Image request

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	header[11] = 32; //Move

	if(l > 5)
	  {
	    header[15] = 5;
	  }
	else
	  {
	    header[15] = l;
	  }

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	header[11] = 128; //Stop

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	header[11] = 64; //Turn
	header[15] = 1;

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	header[11] = 128; //Stop

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }
      }

    /* SEND IT TO THE SERVER */
    printf("UDPEchoClient: Send the string: %s to the server: %s \n", echoString,servIP);    
    if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
      {
	DieWithError("sendto() sent a different number of bytes than expected");
      }
  
    /* Recv a response */
    printf("UDPEchoClient: And now wait for a response... \n");    
    fromSize = sizeof(fromAddr);

    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
      {
        DieWithError("recvfrom() failed");
      }

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
      {
        fprintf(stderr,"Error: received a packet from unknown source \n");
      }

    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0';
    printf("UDPEchoClient:  Received the following (%d bytes) data: %s\n",respStringLen,echoBuffer);
    close(sock);
    exit(0);
}

void clientCNTCCode() 
{
  printf("UDPEchoClient:  CNT-C Interrupt,  exiting....\n");
}



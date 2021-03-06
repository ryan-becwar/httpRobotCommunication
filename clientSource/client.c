/*********************************************************

ROBOT CONTROL CLIENT

This client connects to a specified server, which serves as a proxy for a 
pre-determined robot. This client sends requests to the proxy, which
are then forwarded to the robot. The client sends requests which
cause the robot to move in the shape of a polygon with a
user specified number of sides with a user defined size length.

*********************************************************/
#include "client.h"

#define BUFSIZE 300

void clientCNTCCode();

int main(int argc, char *argv[])
{
  int sock;                           /* Socket descriptor */
  struct sockaddr_in echoServAddr;    /* Echo server address */
  struct sockaddr_in fromAddr;        /* Source address of echo */
  struct hostent *thehost;            /* Hostent from gethostbyname() */
  unsigned short echoServPort;        /* Echo server port */
  unsigned int fromSize;              /* In-out of address size for recvfrom() */
  char *servIP;                       /* IP address of server */
  char respBuffer[BUFSIZE];           /* Buffer for receiving echoed string */
  int n = 4, N=3;                          /* Number of sides */
  double l = 1.0;                     /* Length of sides */
  char header[28] = {0};              /* 28 byte header for custom protocol */
  double waittime;                    /* Time to wait while an action is performed */
  int expSize;                        /* Expected size of incoming payload */
  int bytes_received;                 /* Bytes received from server */
  int bytes_written;                  /* Total bytes written to file */
  char *filename = "image";           /* Name of file to write to */
  int filecount = 1;                  /* File counter */
  char payload[BUFSIZE - 28];         /* Size of payload */
  char payloadSize[5];
  char laserFile[15];

  int i,fileNum=0;
  for(i=1; i<argc; i++)
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
	  l = atof(argv[i+1]);
	}
    }

  /* CREATE SOCKET */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
      DieWithError("socket() failed");
    }

  /* GET CORRECT ADDRESS STRUCTURE */
  memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
  echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
  echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

  /* RESOLVE DOTTED DECIMAL ADDRESS, IF NECESSARY */
  if (echoServAddr.sin_addr.s_addr == -1) 
    {
      thehost = gethostbyname(servIP);
      echoServAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
    }

  /* SEND INITIAL EMPTY PACKET */
  printf("Sending first packet\n");
  if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() sent a different number of bytes than expected"); 
    }

  /* GET SERVER PASSWORD */
  fromSize = sizeof(fromAddr);
  printf("Receiving password packet\n");
  if ((bytes_received = recvfrom(sock, header, sizeof(header), 0, (struct sockaddr *) &fromAddr, &fromSize)) != sizeof(header))
    {
      DieWithError("recvfrom() failed");
    }

  printf("%s", header);

  /* SEND ACK BACK TO SERVER */
  printf("Sending ack\n");
  if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() sent a different number of bytes than expected");
    }
  
  /*
  if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
    {
      DieWithError("recvfrom() failed");
      }*/
  /* CREATE AND SEND INSTRUCTION HEADERS FIRST SHAPE*/

  printf("\n\033[22;32mMoving robot in the shape of an %d sided polygon\033[0m\n", n);
  N= n-1;

  int y;
  for(y=0; y<n; y++)
    {

      //prin
      /*
      expSize = 0;
      bytes_received = 0;
      bytes_written = 0;
      filecount = 0;
      filename = "image1.jpg";
      while(access(filename, F_OK) != -1)
	{
	  sprintf(filename, "image%d.jpg", filecount);
	  filecount++;
	}
      */
     /* FILE *f = fopen(filename, "w");
      
      header[8] = 2; //Image

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
      
      /*
      if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}
      
      //Get expected size
      int v;
      for(v=23; v<27; v++)
	{
	  expSize *= 512;
	  expSize += respBuffer[v];
	}

      while(bytes_written < expSize)
	{
	  strncpy(payload, &respBuffer[28], (strlen(respBuffer)-28));
	  bytes_written += fprintf(f, "%s", payload);
	  bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize);
	}
      fclose(f);
      */

      header[8] = 4; //GPS

      if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}
	printf("\033[22;31morignal GPS:\033[0m %s\n", respBuffer+28);


	header[8] = 8; //DGPS
	if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}

	printf("\033[22;31moriginal DGPS:\033[0m %s\n", respBuffer+28);

	
	header[8] = 16; //lasers
	if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}
	sprintf(laserFile, "laser%d.txt", fileNum++);
	printf("lasers in  %s", laserFile);

	FILE* f = fopen(laserFile, "wb");
	fwrite(respBuffer+28, 1, bytes_received-28, f);
	printf("\n%s\n", respBuffer+28);

	/////////////////////
	while((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) >=300 )
	  {
	    fwrite(respBuffer+28, 1, bytes_received-28, f);
	    memset(respBuffer, 0, 300);
	  }
	fwrite(respBuffer+28, 1, bytes_received-28, f);
	printf("\n");
	fclose(f);
	

	header[8] = 32; //Move
	header[12] = 5;

	printf("Moving robot...\n");
	if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	  {
	    DieWithError("recvfrom() failed");
	  }

	waittime = (double)l/5.0; //Wait time during move
	waittime = waittime * 1000000; //Convert to microseconds
	usleep(waittime);

	printf("Stopping robot...\n");
	header[8] = 128; //Stop

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	  {
	    DieWithError("recvfrom() failed");
	  }

	printf("Turning robot...\n");
	header[8] = 64; //Turn
	header[12] = 1;

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	  {
	    DieWithError("recvfrom() failed");
	  }

	waittime = (double)(14/n); //Wait time during turn
	waittime = waittime * 1000000; //Convert to microseconds
	usleep(waittime); 

	printf("Stopping robot...\n");
	header[8] = 128; //Stop

	if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	  {
	    DieWithError("sendto() sent a different number of bytes than expected");
	  }

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	  {
	    DieWithError("recvfrom() failed");
	  }

      header[8] = 4; //GPS

      if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}
	printf("GPS: %s\n", respBuffer+28);


	header[8] = 8; //DGPS
	if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}

	printf("DGPS: %s\n", respBuffer+28);

	if(y == (N))
	  {
	    y = -1;
	    n = n-1;
	    N = 9;
	    printf("\n\033[22;32mMoving robot in the shape of an %d sided polygon\033[0m\n", n);

	  }
	header[8] = 16; //lasers
	if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

	if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
	  DieWithError("recvfrom() failed");
	}
	sprintf(laserFile, "laser%d.txt", fileNum++);
	printf("lasers in  %s", laserFile);

	f = fopen(laserFile, "wb");
	fwrite(respBuffer+28, 1, bytes_received-28, f);
	printf("\n%s\n", respBuffer+28);

	/////////////////////
	while((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) >=300 )
	  {
	    fwrite(respBuffer+28, 1, bytes_received-28, f);
	    memset(respBuffer, 0, 300);
	  }
	fwrite(respBuffer+28, 1, bytes_received-28, f);
	printf("\n");
	fclose(f);
    }

  

  //Quit
  printf("Client quitting...\n");
  header[8] = 255;

  if(sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() failed");
    }

  close(sock);
  printf("All requests sent, socket closed, client finished\n");
  exit(0);
}


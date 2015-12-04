/*********************************************************

ROBOT CONTROL CLIENT

This client connects to a specified server, which serves as a proxy for a 
pre-determined robot. This client sends requests to the proxy, which
are then forwarded to the robot. The client sends requests which
cause the robot to move in the shape of a polygon with a
user specified number of sides with a user defined size length.

*********************************************************/
#include "robot.h"

#define BUFSIZE 300

void clientCNTCCode();

int main(int argc, char *argv[])
{
  //Change variable names before submission
  int sock;                           /* Socket descriptor */
  struct sockaddr_in echoServAddr;    /* Echo server address */
  struct sockaddr_in fromAddr;        /* Source address of echo */
  struct hostent *thehost;            /* Hostent from gethostbyname() */
  unsigned short echoServPort;        /* Echo server port */
  unsigned int fromSize;              /* In-out of address size for recvfrom() */
  char *servIP;                       /* IP address of server */
  char respBuffer[BUFSIZE];           /* Buffer for receiving echoed string */
  int n = 4;                          /* Number of sides */
  double l = 1.0;                     /* Length of sides */
  char header[28] = {0};              /* 28 byte header for custom protocol */
  double waittime;                    /* Time to wait while an action is performed */
  int expSize;                        /* Expected size of incoming payload */
  int bytes_received;                 /* Bytes received from server */
  int bytes_written;                  /* Total bytes written to file */
  char *filename = "image";           /* Name of file to write to */
  char *filenum = "";                 /* String that holds the file number to be apended */
  int filecount = 1;                  /* File counter */
  char payload[BUFSIZE - 28];         /* Size of payload */

  int i;
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
  printf("Sending first packet");
  if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() sent a different number of bytes than expected");
    }

  /* GET SERVER PASSWORD */
  fromSize = sizeof(fromAddr);
  printf("Receiving password packet");
  if ((bytes_received = recvfrom(sock, header, sizeof(header), 0, (struct sockaddr *) &fromAddr, &fromSize)) != sizeof(header))
    {
      DieWithError("recvfrom() failed");
    }
  printf("%s", header);

  /* SEND ACK BACK TO SERVER */
  printf("Sending ack");
  if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() sent a different number of bytes than expected");
    }

  /* CREATE AND SEND INSTRUCTION HEADERS */
  int y;
  for(y=0; y<n; y++)
    {
      /*
      expSize = 0;
      bytes_received = 0;
      bytes_written = 0;
      //filename = "image";
      //snprintf(filenum, 1, "%d", filecount);
      //strcat(filename, filenum);
      FILE *f = fopen("img.jpg", "w");
      filecount++;
      */
      /*
      header[8] = 2; //Image

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

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
      */

      header[8] = 32; //Move

      if(l > 5)
	{
	  header[12] = 5;
	}
      else
	{
	  header[12] = l;
	}

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

      waittime = (double)l/5.0; //Wait time during move
      waittime = waittime * 1000000; //Convert to microseconds
      usleep(waittime);

      header[8] = 128; //Stop

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

      header[8] = 64; //Turn
      header[12] = 1;

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}

      waittime = (double)(14/n); //Wait time during turn
      waittime = waittime * 1000000; //Convert to microseconds
      usleep(waittime); 

      header[8] = 128; //Stop

      if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
	{
	  DieWithError("sendto() sent a different number of bytes than expected");
	}
    }

  /*
  expSize = 0;
  bytes_received = 0;
  bytes_written = 0;
  //filename = "image";
  //sprintf(filenum, "%d", filecount);
  //strcat(filename, filenum);
  FILE *f = fopen("finalimg.jpg", "w");

  header[11] = 2; //Final image

  if (sendto(sock, header, sizeof(header), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(header))
    {
      DieWithError("sendto() sent a different number of bytes than expected");
    }

  if((bytes_received = recvfrom(sock, respBuffer, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) <= 0)
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
  */

  //SEND QUIT

  close(sock);
  printf("All requests sent, socket closed, client finished");
  exit(0);
}


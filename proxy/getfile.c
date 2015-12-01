#include "getfile.h"

#define RCVBUFSIZE 10000   /* Size of receive buffer */

int main(int argc, char *argv[]){
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    int port = -1;     /* Echo server port */
    char filename[30];             /* The save name for output */
    char fileRequest[100];
    char* doc = NULL;
    char* host = NULL;
    char echoString[300];                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    int bytesRec;   /* Bytes read in single recv()
                     and total bytes read */
    
    switch(argc) {
        case 6:
            host = argv[1];
            port = atoi(argv[3]);
            strcpy(filename, argv[5]);
            break;
        case 4:
            //Parsing the command line arguments
            if(strcmp(argv[2], "-p") == 0){
                port = atoi(argv[3]);
                strcpy(filename, "stdout");
            }
            else if(strcmp(argv[2], "-f") == 0){
                strcpy(filename, argv[3]);
            }
            else DieWithError("Only -f or -p");
        case 2:
            host = argv[1];
            break;
        default:
            DieWithError("Usage: getfile URL -p [port] -f [filename]");
            break;
    }
    if(access(filename, F_OK) != -1){
        DieWithError("File already exists");
    }
    if(filename[0] == '\0'){
        strcpy(filename, "stdout");
    }
    if(port == -1){
        port = 8080;
    }

    if(strstr(host, "https") != NULL){
        host += 8;
    }
    else if(strstr(host, "http") != NULL){
        host += 7;
    }
    doc = strchr(host, '/');
    if (doc != NULL) {
        strcpy(fileRequest, doc);
        host[doc - host] = '\0';
    }
    else {
        strcpy(fileRequest, "/");
    }

    printf("Port: %d, Saving output to: %s, Host: %s\n"
           "Getting file: %s\n", port, filename, host, fileRequest);
    
    
    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));       /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;               /* Internet address family */
    echoServAddr.sin_port        = htons(port);   /* Server port */

    struct hostent        *he;
    if ( (he = gethostbyname(host) ) == NULL ) {
        exit(1); /* error */
    }
    memcpy(&echoServAddr.sin_addr, he->h_addr_list[0], he->h_length);

    
    //  This establishes a connection.
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
char* requestedMsg =  "/state?id=44procal";
    sprintf(echoString, "GET %s HTTP/1.1\r\nHOST: %s\r\n\r\n", requestedMsg, host);
	//sprintf(echoString, "/state?id=robot_44procal");
    /*  This sends the request to the server */
    if (send(sock, echoString, strlen(echoString), 0) < 0){
        DieWithError("send() failed");
    }
    
    printf("\nRECEIVED THIS FROM THE SERVER: \n");
    
    if ((bytesRec = recv(sock, echoBuffer, RCVBUFSIZE, 0)) <= 0){
        DieWithError("recv() failed or connection closed prematurely");
    }
    echoBuffer[bytesRec] = '\0';  /* Terminate the string! */
        printf("%s", echoBuffer);
	printf("\n");
    
    close(sock);
    exit(0);
    
}

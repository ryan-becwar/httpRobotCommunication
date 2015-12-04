//
//  temp.c
//  
//
//  Created by Devin White on 12/3/15.
//
//

#include "temp.h"
#define RCVBUFSIZE 1024   /* Size of receive buffer */


void communicate(int requestType, char* host, int robot_number, char* robot_id, int data){
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    int port = -1;     /* Echo server port */
    char filename[30];             /* The save name for output */
    char fileRequest[100];
    char removeHeader[RCVBUFSIZE];
    char* doc = NULL;
    char echoString[300];                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    int bytesRec;   /* Bytes read in single recv()
                     and total bytes read */
    char request[60];
    
    switch(requestType) {
        //Image request
        case 2:
            port = 8081;
            sprintf(request, "/snapshot?topic=/robot_%d/image?width=600?height=500", robot_number);
            sprintf(filename,"output.jpg");
            break;
        //GPS request
        case 4:
            port = 8082;
            sprintf(request, "/state?id=%s", robot_id);
            sprintf(filename,"output.txt");
            break;
        //DGPS request
        case 8:
            port = 8084;
            sprintf(request, "/state?id=%s", robot_id);
            sprintf(filename,"output.txt");
            break;
        //lasers request
        case 16:
            port = 8083;
            sprintf(request, "/state?id=%s", robot_id);
            sprintf(filename,"output.txt");
            break;
        //move request
        case 32:
            port = 8082;
            sprintf(request, "/state?id=%s&lx=%d", robot_id, data);
            sprintf(filename,"output.txt");
            break;
        //turn request
        case 64:
            port = 8082;
            sprintf(request, "/state?id=%s&lz=%d", robot_id, data);
            sprintf(filename,"output.txt");
            break;
        //stop request
        case 128:
            port = 8082;
            sprintf(request, "/state?id=%s&lx=0", robot_id);
            sprintf(filename,"output.txt");
            break;
        default:
            break;
    }
    if(access(filename, F_OK) != -1){
        remove(filename);
    }
    //    if(filename[0] == '\0'){
    //        strcpy(filename, "stdout");
    //    }
    printf("Port: %d, Saving output to: %s, Host: %s\n"
           "Getting file: %s\n", port, filename, host, request);
    
    
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
    
    sprintf(echoString, "GET %s HTTP/1.1\r\nHOST: %s\r\n\r\n", request, host);
    //sprintf(echoString, "/state?id=robot_44procal");
    /*  This sends the request to the server */
    
    if (send(sock, echoString, strlen(echoString), 0) < 0){
        DieWithError("send() failed");
    }
    memset(echoString, 0, 300);
    printf("\nRECEIVED THIS FROM THE SERVER: \n");
    FILE *fp = fopen("output.txt", "wb");
    
    unsigned int totalBytes=0;
    char payloadSize[30];
    bytesRec = recv(sock, echoBuffer, RCVBUFSIZE, 0);
    //    memset(removeHeader, 0, sizeof(removeHeader));
    sprintf(removeHeader, "%s", strtok(echoBuffer, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, " "));
    sprintf(payloadSize, "%s", strtok(NULL, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, "\n"));
    sprintf(removeHeader, "%s", strtok(NULL, "\n"));
    printf("%s\n\n", payloadSize);
    
    
    totalBytes+=sizeof(removeHeader);
    fprintf(fp,"%s", removeHeader);
    printf("%s", removeHeader);/////
    memset(removeHeader, 0, sizeof(removeHeader));
    memset(echoBuffer, 0, bytesRec);
    
    if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
    
    while ((bytesRec = recv(sock, echoBuffer, RCVBUFSIZE, 0)) > 0){
        totalBytes+=bytesRec;
        fprintf(fp,"%s", echoBuffer);
        //        printf("%s", echoBuffer);
        memset(echoBuffer, 0, bytesRec);
        
        if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
    }
    //fprintf(fp,"%s", totalBuffer);
    fclose(fp);
    printf("\n");
    
    close(sock);
    
}

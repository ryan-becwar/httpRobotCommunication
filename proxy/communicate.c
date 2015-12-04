#include "proxy.h"

void communicate(int requestType, char* host, int robot_number, char* robot_id, int data){
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    int port = -1;     /* Echo server port */
    char filename[30];             /* The save name for output */
    char fileRequest[100];
    char payload[1024];
    char* doc = NULL;
    char echoString[300];                /* String to send to echo server */
    char echoBuffer[1024];     /* Buffer for echo string */
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
            sprintf(filename,"output4.txt");
            break;
        //DGPS request
        case 8:
            port = 8084;
            sprintf(request, "/state?id=%s", robot_id);
            sprintf(filename,"output8.txt");
            break;
        //lasers request
        case 16:
            port = 8083;
            sprintf(request, "/state?id=%s", robot_id);
            sprintf(filename,"output16.txt");
            break;
        //move request
        case 32:
            port = 8082;
            sprintf(request, "/state?id=%s&lx=%d", robot_id, data);
            sprintf(filename,"output32.txt");
            break;
        //turn request
        case 64:
            port = 8082;
            sprintf(request, "/state?id=%s&lz=%d", robot_id, data);
            sprintf(filename,"output64.txt");
            break;
        //stop request
        case 128:
            port = 8082;
            sprintf(request, "/state?id=%s&lx=0", robot_id);
            sprintf(filename,"output128.txt");
            break;
        default:
            break;
    }
    if(access(filename, F_OK) != -1){
        remove(filename);
    }
    
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
        DieWithError("connect() to robot failed");

    sprintf(echoString, "GET %s HTTP/1.1\r\nHOST: %s\r\n\r\n", request, host);
    //sprintf(echoString, "/state?id=robot_44procal");
    /*  This sends the request to the server */
    
    if (send(sock, echoString, strlen(echoString), 0) < 0){
        DieWithError("send() failed");
    }
    memset(echoString, 0, 300);
    printf("\nRECEIVED THIS FROM THE SERVER: \n");
    FILE *fp = fopen(filename, "w");
    
    unsigned int totalBytes=0;
    char payloadSize[30];
    bytesRec = recv(sock, echoBuffer, 1024, 0);
    //    memset(payload, 0, sizeof(payload));
    if(requestType==2){
        printf("---%s\n", echoBuffer);
        sprintf(payload, "%s", strtok(echoBuffer, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));


        printf("This: %s is payload\n", payload);
        if(strlen(payload)>26){
            fprintf(fp,"%s", payload);
            totalBytes+=strlen(payload);
        }
        printf("%d bytes written", totalBytes);
        //memset(payload, 0, sizeof(payload));
        memset(echoBuffer, 0, bytesRec);
        
        if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
        
        while ((bytesRec = recv(sock, echoBuffer, 1024, 0)) > 0){
            totalBytes+=bytesRec;
            fprintf(fp,"%s", echoBuffer);
            //        printf("%s", echoBuffer);
            memset(echoBuffer, 0, bytesRec);
            
            if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
        }

        printf("\n total bytes read %u", totalBytes);
        fclose(fp);
        printf("\n");
        
        close(sock);
        
    }else{
//        printf("%s\n----", echoBuffer);
        sprintf(payload, "%s", strtok(echoBuffer, "\n"));
        sprintf(payload, "%s", strtok(NULL, " "));
        sprintf(payloadSize, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\n"));
        sprintf(payload, "%s", strtok(NULL, "\r\n"));
    
    printf("This number %s\n\n", payloadSize);
    
        if(strlen(payload)>26){
            fprintf(fp,"%s", payload);
            totalBytes+=strlen(payload);
        }
    printf("%d bytes written", totalBytes);
    //memset(payload, 0, sizeof(payload));
    memset(echoBuffer, 0, bytesRec);
    
    if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
    
    while (totalBytes != atoi(payloadSize) && (bytesRec = recv(sock, echoBuffer, 1024, 0)) > 0){
        totalBytes+=bytesRec;
        fprintf(fp,"%s", echoBuffer);
        //        printf("%s", echoBuffer);
        memset(echoBuffer, 0, bytesRec);
        
        if(bytesRec<0) DieWithError("recv() failed or connection closed prematurely");
    }
//    fseek(fp, 0L, SEEK_END);
//    totalBytes = ftell(fp);
//    fseek(fp, 0L, SEEK_SET);
    printf("\n total bytes read %u", totalBytes);
    fclose(fp);
    printf("\n");
    
    close(sock);
    }
}

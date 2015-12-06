#include "proxy.h"

char* communicate(int requestType, char* host, int robot_number, char* robot_id, int data){
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    int port = -1;     /* Echo server port */
    char filename[30];             /* The save name for output */
    char fileRequest[100];
    char payload[1024];
    char* doc = NULL;
    char requestMsg[300];                /* String to send to echo server */
    char buffer[1024];     /* Buffer for echo string */
    int bytesRec;   /* Bytes read in single recv()
                     and total bytes read */
    char request[60];
    unsigned int totalBytes=0;
    char payloadSize[30];
    int i;
    
    switch(requestType) {
        //Image request
        case 2:
            port = 8081;
            sprintf(request, "/snapshot?topic=/robot_%d/image?width=600?height=500", robot_number);
            sprintf(filename,"output2.jpg");
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
        return "fail";
    
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));       /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;               /* Internet address family */
    echoServAddr.sin_port        = htons(port);   /* Server port */
    
    struct hostent        *he;
    if ( (he = gethostbyname(host) ) == NULL )
        return "fail"; 
    
    memcpy(&echoServAddr.sin_addr, he->h_addr_list[0], he->h_length);
    
    //  This establishes a connection.
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        return "fail";

    sprintf(requestMsg, "GET %s HTTP/1.1\r\nHOST: %s\r\n\r\n", request, host);
    
    /*  This sends the request to the server */
    if (send(sock, requestMsg, strlen(requestMsg), 0) < 0)
        return "fail";
    
    memset(requestMsg, 0, 300);
    FILE *fp = fopen(filename, "wb");
    memset(buffer, 0, 1024);
    bytesRec = recv(sock, buffer, 1024, 0);
    if(bytesRec<0) return "fail";
    memset(payload, 0, sizeof(payload));
    
    //process the image request differntly because it is an HTTP 1.0 response
    if(requestType==2){
        //remove the header
        sprintf(payload, "%s", strtok(buffer, "\n"));
        for(i=0; i<7; i++)
            sprintf(payload, "%s", strtok(NULL, "\n"));

        memset(payload, 0, sizeof(payload));
        memset(buffer, 0, bytesRec);
        
        while ((bytesRec = recv(sock, buffer, 1024, 0)) > 0){
            totalBytes+=bytesRec;
            fwrite(buffer, 1, bytesRec, fp);
            memset(buffer, 0, bytesRec);
            //if the robot goes down, alert the client
            if(bytesRec<0) return "fail";
        }
    }
    //process the other responses by removing the header and then saving the payload
    else{
        //remove the header
        sprintf(payload, "%s", strtok(buffer, "\n"));
        sprintf(payload, "%s", strtok(NULL, " "));
        sprintf(payloadSize, "%s", strtok(NULL, "\n"));
        for(i=0; i<5; i++)
            sprintf(payload, "%s", strtok(NULL, "\n"));
        
        if(strlen(payload)>26){
            fprintf(fp,"%s", payload);
            totalBytes+=strlen(payload);
        }
        memset(payload, 0, sizeof(payload));
        memset(buffer, 0, bytesRec);
        
        while (totalBytes != atoi(payloadSize) && (bytesRec = recv(sock, buffer, 1024, 0)) > 0){
            totalBytes+=bytesRec;
            fprintf(fp,"%s", buffer);
            memset(buffer, 0, bytesRec);
            //if the robot goes down, alert the client
            if(bytesRec<0) return "fail";
        }

    }
    
    printf("\ntotal bytes read %u\n", totalBytes);
    fclose(fp);
    close(sock);
    
    char * returnString = filename;
    return returnString;
}

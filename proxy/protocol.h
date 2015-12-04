typedef struct headerData {
	int protocolID;
	int password;
	int clientRequest;
	int requestData;
	int byteOffset;
	int totalSize;
	int payloadSize;
} headerData;

#define IDENTIFIER_LOC 0 
#define PASSWORD_LOC 4
#define CLIENT_REQUEST_LOC 8 
#define REQUEST_DATA_LOC 12
#define BYTE_OFFSET_LOC 16
#define TOTAL_SIZE_LOC 20
#define PAYLOAD_SIZE_LOC 24
#define PAYLOAD_LOC 28

#define CONNECT	0
#define IMAGE	2
#define GPS	4
#define DGPS	8
#define LASERS	16
#define MOVE	32
#define TURN	64
#define STOP	128
#define QUIT	255
#define ERR_REQUEST	256
#define ERR_ROBOT	512

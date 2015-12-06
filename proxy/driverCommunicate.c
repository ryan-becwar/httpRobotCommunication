
#include "proxy.h"

char* communicate(int requestType, char* host, int robot_number, char* robot_id, int data);

int main(int argc, char *argv[]){
    char filename[15];
    sprintf(filename, "%s", communicate(2, "169.55.155.236", 1, NULL, -1));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(4, "169.55.155.236", -1, "44procal", -1));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(8, "169.55.155.236", -1, "44procal", -1));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(16, "169.55.155.236", -1, "44procal", -1));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(32, "169.55.155.236", -1, "44procal", 10));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(64, "169.55.155.236", -1, "44procal", 10));
    printf("file saved in %s\n", filename);
    
    sprintf(filename, "%s", communicate(128, "169.55.155.236", -1, "44procal", -1));
    printf("file saved in %s\n", filename);

    exit(0);
}


#include "proxy.h"

void communicate(int requestType, char* host, int robot_number, char* robot_id, int data);

int main(int argc, char *argv[]){
    communicate(2, "169.55.155.236", 1, NULL, -1);
//    communicate(4, "169.55.155.236", -1, "44procal", -1);
//    communicate(8, "169.55.155.236", -1, "44procal", -1);
//    communicate(16, "169.55.155.236", -1, "44procal", -1);
//    communicate(32, "169.55.155.236", -1, "44procal", 10);
//    communicate(64, "169.55.155.236", -1, "44procal", 10);
//    communicate(128, "169.55.155.236", -1, "44procal", -1);


    exit(0);
}
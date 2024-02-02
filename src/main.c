#include "config.h"
#include "funcs.h"

const char *serial_interface = "/dev/tnt0"; // ttyUSB0 esse ai e virtual

int sharedCounter = 0;
volatile int uiIsFinished = 0;
int sock;

int pipefd[2];  // File descriptors for the pipe

struct allData all_data;
struct inverterDataChunk all_data_with_timestamp[3];


pthread_mutex_t inverterDataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t BMSDataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t canInterfaceMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t serialInterfaceMutex = PTHREAD_MUTEX_INITIALIZER;

double allInverterDataDeltaTime = {0, 0};

struct timespec lastTelegram;

void *serialSendReceive (void* arg);

int mainFlow () {
    sock = createCANSocket(CAN_INTERFACE);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    // // Create a pipe
    // if (pipe(pipefd) == -1) {
    //     perror("Error creating pipe");
    //     exit(EXIT_FAILURE);
    // }

    pthread_t canThread, uiThread, BMSThread, logInverterThread, serverThread, carControlThread;

    // if(pthread_create(&carControlThread, NULL, sendInverterData, NULL)){
    //     perror("ERROR: send thread create");
    //     return 1;
    // }

    // if(pthread_create(&canThread, NULL, readInverterDataWithTimestamp, NULL)){
    //     perror("ERROR: can thread create");
    //     return 1;
    // }

    // if(pthread_create(&BMSThread, NULL, serialSendReceive, NULL)){
    //     perror("ERROR: BMS thread create");
    //     return 1;
    // }

    // if(pthread_create(&uiThread, NULL, windowLoop, NULL)){
    //     perror("ERROR: ui thread create");
    //     return 1;
    // }

    if(pthread_create(&uiThread, NULL, windowLoopTimestamp, NULL)){
        perror("ERROR: ui thread create");
        return 1;
    }

    // if(pthread_create(&serverThread, NULL, server, NULL)){
    //     perror("ERROR: server thread create");
    //     return 1;
    // }

    // if(pthread_create(&logInverterThread, NULL, logInverter, NULL)){
    //     perror("ERROR: inverter log create");
    //     return 1;
    // }

    while(!uiIsFinished){

    }

    if (pthread_join(uiThread, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    pthread_mutex_destroy(&inverterDataMutex);
    pthread_mutex_destroy(&serialInterfaceMutex);
    pthread_mutex_destroy(&canInterfaceMutex);

    printf("Thread has finished.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    // createETHSocket();
    // printf("starting...");
    // serialSendReceive(NULL);

    mainFlow();
    
    sock = createCANSocket(CAN_INTERFACE);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }    

    pthread_t smoeThread;
    if(pthread_create(&smoeThread, NULL, readDS4, (int *) sock)){
        perror("ERROR: controller thread create");
        return 1;
    } else {
        printf("controller thread created\n");
    }
    pthread_join(smoeThread, NULL);
    return 0;
}

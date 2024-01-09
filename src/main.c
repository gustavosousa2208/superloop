#include "config.h"
#include "funcs.h"

const char *serial_interface = "/dev/tnt0"; // ttyUSB0 esse ai e virtual

int sharedCounter = 0;
volatile int uiIsFinished = 0;

struct allData all_data;

pthread_mutex_t inverterDataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t BMSDataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t canInterfaceMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t serialInterfaceMutex = PTHREAD_MUTEX_INITIALIZER;

double allInverterDataDeltaTime = {0, 0};

const int desiredPort = 12345;

struct timespec lastTelegram;

void *serialSendReceive (void* arg);

int mainFlow () {
    int sock = createCANSocket(CAN_INTERFACE);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    pthread_t canThread, uiThread, BMSThread, logInverterThread, serverThread;
    struct canReadThreadDataStruct canThreadData;
    canThreadData.socket_descriptor = sock;

    if(pthread_create(&canThread, NULL, readInverterData, &canThreadData)){
        perror("ERROR: can thread create");
        return 1;
    }

    if(pthread_create(&BMSThread, NULL, serialSendReceive, NULL)){
        perror("ERROR: BMS thread create");
        return 1;
    }

    // if(pthread_create(&uiThread, NULL, windowLoop, NULL)){
    //     perror("ERROR: ui thread create");
    //     return 1;
    // }

    if(pthread_create(&serverThread, NULL, server, NULL)){
        perror("ERROR: server thread create");
        return 1;
    }

    if(pthread_create(&logInverterThread, NULL, logInverter, NULL)){
        perror("ERROR: inverter log create");
        return 1;
    }

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

int getCommandLineArguments(int argc, char *argv[]) {
    const char *input_speed = NULL;
    const char *mode = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            // Check if there's another argument after -i
            if (i + 1 < argc) {
                input_speed = argv[i + 1];
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Error: -s option requires an argument.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            // Check if there's another argument after -o
            if (i + 1 < argc) {
                mode = argv[i + 1];
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Error: -o option requires an argument.\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            return 1;
        }
    }
}

int main(int argc, char *argv[]) {
    // createETHSocket();
    // printf("starting...");
    // serialSendReceive(NULL);

    #ifdef DEBUG
        printf("we debug!");
    #else
        printf("we dont");
    #endif

    // mainFlow();
    // pthread_t controllerThread;
    // if(pthread_create(&controllerThread, NULL, readDS4, NULL)){
    //     perror("ERROR: controller thread create");
    //     return 1;
    // } else {
    //     printf("controller thread created\n");
    // }
    // pthread_join(controllerThread, NULL);
    return 0;
}

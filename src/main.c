#include "funcs.h"

const char *serial_interface = "/dev/tnt0"; // ttyUSB0 esse ai e virtual
const char *can_interface = "can0";

int sharedCounter = 0;
volatile int interrupted;

uint16_t sharedCommandedSpeed;
uint16_t sharedLogicalState;
uint16_t sharedInverterBatteryVoltage;
uint16_t sharedInverterMosfetTemperature1;
uint16_t sharedInverterMosfetTemperature2;
uint16_t sharedInverterAirTemperature;
uint16_t sharedMotorCurrent;
uint16_t sharedMotorVoltage;

uint16_t sharedBMSVoltage;
uint16_t sharedBMSCurrent;
uint16_t shareBMSTemperature;
uint16_t sharedBMSRemainingCapacity;
uint16_t sharedBMSTotalCapacity;

pthread_mutex_t incomingDataMutex;
pthread_mutex_t canInterfaceMutex;

int mainFlow () {
    int sock = createCANSocket(can_interface);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    pthread_t canThread, uiThread;
    struct canReadThreadDataStruct canThreadData;
    canThreadData.socket_descriptor = sock;

    if(pthread_create(&canThread, NULL, readInverterData, &canThreadData)){
        perror("can thread create");
        return 1;
    }

    if(pthread_create(&uiThread, NULL, windowLoop, NULL)){
        perror("ui thread create");
        return 1;
    }

    // int oldCounter = sharedCounter;
    // while(1){
    //     if(sharedCounter != oldCounter) {
    //         oldCounter = sharedCounter;
    //         printf("Counter: %d\n", sharedCounter);
    //     }
    // }



    if (pthread_join(canThread, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    pthread_mutex_destroy(&incomingDataMutex);

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
    mainFlow();
    return 0;
}

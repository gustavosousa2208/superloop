#include "funcs.h"
#include "shares.h"

double before, now = 0;
FILE *file = NULL;

struct allData all_data_temp2;
struct timespec lastMeasure;

void *logInverter (void* arg) {

    file = fopen("log.txt", "w");

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    while(!uiIsFinished) {
        for(int x = 0; x < 1000; x++){
            usleep(1000);
        }

        pthread_mutex_lock(&inverterDataMutex);
        all_data_temp2 = all_data;
        pthread_mutex_unlock(&inverterDataMutex);

        clock_gettime(CLOCK_REALTIME, &lastMeasure);
        fprintf(file, "%ld.%ld CAN | 0x701: %d %d %d %d, 0x702 %d %d %d, 0x703 %d %d %d %d\n", 
        lastMeasure.tv_sec, 
        lastMeasure.tv_nsec, 
        all_data_temp2.sharedCommandedSpeed, 
        all_data_temp2.logicalState, 
        all_data_temp2.encoderVelocity, 
        all_data_temp2.encoderPulseCounter,
        all_data_temp2.inverterMosfetTemperature1, 
        all_data_temp2.inverterMosfetTemperature2, 
        all_data_temp2.inverterAirTemperature, 
        all_data_temp2.motorCurrent,
        all_data_temp2.inverterBatteryVoltage,
        all_data_temp2.motorVoltage,
        all_data_temp2.vehicleSpeed
        ); 
    }
    printf("logInverter thread finished\n");
    fclose(file);

    return;
}
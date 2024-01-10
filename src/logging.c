#include "funcs.h"
#include "shares.h"

double before, now = 0;
FILE *file = NULL;

void *logInverter (void* arg) {

    file = fopen("log.txt", "w");

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    while(!uiIsFinished) {
        // now = (double) lastTelegram.tv_sec + ((double) lastTelegram.tv_nsec / 10e6);
        // if (now > before) {
        //     before = now;

        //     fprintf(file, "Timestamp: %lld, CAN ID: %u, data: ", now, thisFrame.can_id);
        //     for (int i = 0; i < thisFrame.can_dlc; i++)
        //         fprintf(file, "%02X ", thisFrame.data[i]);
        //     fprintf(file, "\n");
        // }
    }
    fclose(file);
}
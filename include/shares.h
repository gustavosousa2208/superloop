#ifndef SHARES_H
#define SHARES_H

extern const char *can_interface;

extern int sock;

extern pthread_mutex_t canInterfaceMutex;
extern pthread_mutex_t inverterDataMutex;

extern struct allData all_data;
extern struct inverterDataChunk all_data_with_timestamp[3];

extern int sharedCounter;

extern pthread_mutex_t serialInterfaceMutex;
extern pthread_mutex_t BMSDataMutex;

extern double allInverterDataDeltaTime;

extern const char *serial_interface;

extern volatile int uiIsFinished;

extern struct timespec lastTelegram;
extern struct can_frame thisFrame;

extern int pipefd[2];  // File descriptors for the pipe

#endif // SHARES_H
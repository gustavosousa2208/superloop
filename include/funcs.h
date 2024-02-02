#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <termios.h>

#include <ncurses.h>
#include <menu.h>

#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


#define CAN_RECV_TIMEOUT_USECONDS 200000
#define BUFFER_SIZE 256
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
extern char *choices[];
#define to_read 47

struct message {
    long mtype;
    char mtext[256];
};

struct BMSThreadDataStruct {
    const char *serial_interface;
};

struct canReadThreadDataStruct {
    int socket_descriptor;
};

struct canSendThreadDataStruct {
    int socket_descriptor;
    struct can_frame frame;
};

struct allData {
    uint16_t sharedCommandedSpeed;
    uint16_t logicalState;
    uint16_t encoderVelocity;
    uint16_t encoderPulseCounter;

    uint16_t inverterMosfetTemperature1;
    uint16_t inverterMosfetTemperature2;
    uint16_t inverterAirTemperature;

    uint16_t motorCurrent;
    uint16_t inverterBatteryVoltage;
    uint16_t motorVoltage;
    uint16_t vehicleSpeed;

    uint16_t sharedBMSVoltage;
    uint16_t sharedBMSCurrent;
    uint16_t sharedBMSTemperature;
    uint16_t sharedBMSRemainingCapacity;
    uint16_t sharedBMSTotalCapacity;
};

struct inverterDataChunk {
    uint16_t data[4];
    struct timespec timestamp;
};

void *sendInverterData(void * arg);
int sendTelegram(int s, struct can_frame *frame);
void *readInverterData(void * arg);
uint16_t telegramReceive2(int s);
void *windowLoop(void* arg);
void *windowLoopTimestamp(void* arg);
int createCANSocket(const char* interface_name);
void sigintHandler(int sig_num);
void *logInverter (void* arg);
void * server (void* arg);
void * readDS4(void *arg );
int sendInverterDataOnce(struct can_frame thisFrame);
void* readInverterDataWithTimestamp(void* arg);
void curses2 (void*arg);

#endif
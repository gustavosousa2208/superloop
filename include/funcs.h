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

#include <termios.h>

#include <ncurses.h>
#include <menu.h>

#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>


#define CAN_RECV_TIMEOUT_USECONDS 200000
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
    uint16_t sharedMotorCurrent;
    uint16_t sharedInverterBatteryVoltage;
    uint16_t sharedMotorVoltage;
    uint16_t sharedInverterAirTemperature;
    
    uint16_t sharedInverterMosfetTemperature1;
    uint16_t sharedInverterMosfetTemperature2;
    uint16_t sharedLogicalState;
    uint16_t sharedCommandedSpeed;

    uint16_t sharedBMSVoltage;
    uint16_t sharedBMSCurrent;
    uint16_t sharedBMSTemperature;
    uint16_t sharedBMSRemainingCapacity;
    
    uint16_t sharedBMSTotalCapacity;
    uint16_t extra1;
    uint16_t extra2;
    uint16_t extra3;
};

struct messageWithTimestamp {
    uint16_t message;
    struct timespec timestamp;
};

// isso aqui é bizarro, mas fica tudo centralizado
struct allDataWithTimestamp {
    struct messageWithTimestamp sharedMotorCurrent;
    struct messageWithTimestamp sharedInverterBatteryVoltage;
    struct messageWithTimestamp sharedMotorVoltage;
    struct messageWithTimestamp sharedInverterAirTemperature;
    
    struct messageWithTimestamp sharedInverterMosfetTemperature1;
    struct messageWithTimestamp sharedInverterMosfetTemperature2;
    struct messageWithTimestamp sharedLogicalState;
    struct messageWithTimestamp sharedCommandedSpeed;

    struct messageWithTimestamp sharedBMSVoltage;
    struct messageWithTimestamp sharedBMSCurrent;
    struct messageWithTimestamp sharedBMSTemperature;
    struct messageWithTimestamp sharedBMSRemainingCapacity;
    
    struct messageWithTimestamp sharedBMSTotalCapacity;
    struct messageWithTimestamp extra1;
    struct messageWithTimestamp extra2;
    struct messageWithTimestamp extra3;
};

void *sendInverterData(void * arg);
int sendTelegram(int s, struct can_frame *frame);
void *readInverterData(void * arg);
uint16_t telegramReceive2(int s);
void *windowLoop(void* arg);
int createCANSocket(const char* interface_name);
void sigintHandler(int sig_num);
void *logInverter (void* arg);
void * server (void* arg);
void * readDS4(void *arg );

#endif
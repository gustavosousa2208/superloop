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


#define CAN_RECV_TIMEOUT_USECONDS 200000
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
extern char *choices[];

struct message {
    long mtype;
    char mtext[256];
};

struct canReadThreadDataStruct {
    int socket_descriptor;
};

struct canSendThreadDataStruct {
    int socket_descriptor;
    struct can_frame frame;
};

void *sendInverterData(void * arg);
uint8_t serialSendReceive (const char *serial_interface, uint8_t byte);
int sendTelegram(int s, struct can_frame *frame);
void *readInverterData(void * arg);
uint16_t telegramReceive2(int s);
void *windowLoop(void* arg);
int createCANSocket(const char* interface_name);
void sigintHandler(int sig_num);

#endif
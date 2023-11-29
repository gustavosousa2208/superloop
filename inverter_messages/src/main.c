#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int s = 0;
const char *interface_name = "vcan1";

void handler(int sig) {
  printf("\nInterrupt signal received. Exiting loop.\n");
  close(s);
  exit(0);
}

int main () {
    signal(SIGINT, handler); // Register the handler for SIGINT

    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s == -1) {
        perror("socket");
        return -1; // Handle the error as needed
    }

    strcpy(ifr.ifr_name, interface_name);

    if (ioctl(s, SIOCGIFINDEX, &ifr) == -1) {
        perror("ioctl");
        close(s); // Close the socket before returning in case of an error
        return -1; // Handle the error as needed
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(s); // Close the socket before returning in case of an error
        return -1; // Handle the error as needed
    }

    struct can_frame commandedSpeed;
    commandedSpeed.can_id = 0x685;
    commandedSpeed.can_dlc = 2;
    commandedSpeed.data[0] = 0x00;
    commandedSpeed.data[1] = 0x00;
    struct can_frame logicalState;
    logicalState.can_id = 0x680;
    logicalState.can_dlc = 2;
    logicalState.data[0] = 0x01;
    logicalState.data[1] = 0x01;
    struct can_frame motorVoltage;
    motorVoltage.can_id = 0x7;
    motorVoltage.can_dlc = 2;
    motorVoltage.data[0] = 0x79;
    motorVoltage.data[1] = 0x02;
    struct can_frame batteryVoltage;
    batteryVoltage.can_id = 0x04;
    batteryVoltage.can_dlc = 2;
    batteryVoltage.data[0] = 0x79;
    batteryVoltage.data[1] = 0x02;
    struct can_frame motorCurrent;
    motorCurrent.can_id = 0x3;
    motorCurrent.can_dlc = 2;
    motorCurrent.data[0] = 0x79;
    motorCurrent.data[1] = 0x02;

    struct can_frame temp1;
    temp1.can_id = 0x30;
    temp1.can_dlc = 2;
    temp1.data[0] = 0x35;
    temp1.data[1] = 0x01;
    struct can_frame temp2;
    temp2.can_id = 0x33;
    temp2.can_dlc = 2;
    temp2.data[0] = 0x35;
    temp2.data[1] = 0x01;
    struct can_frame inverterAir;
    inverterAir.can_id = 0x34;
    inverterAir.can_dlc = 2;
    inverterAir.data[0] = 0x35;
    inverterAir.data[1] = 0x01;

    int bytes_sent = 0;
    while(1) {
        bytes_sent = send(s, &commandedSpeed, sizeof(commandedSpeed), 0);
        bytes_sent = send(s, &logicalState, sizeof(logicalState), 0);
        bytes_sent = send(s, &motorVoltage, sizeof(motorVoltage), 0);
        bytes_sent = send(s, &batteryVoltage, sizeof(batteryVoltage), 0);
        bytes_sent = send(s, &motorCurrent, sizeof(motorCurrent), 0);
        bytes_sent = send(s, &temp1, sizeof(temp1), 0);
        bytes_sent = send(s, &temp2, sizeof(temp2), 0);
        bytes_sent = send(s, &inverterAir, sizeof(inverterAir), 0);
        
        usleep(20*1000);
    }
}
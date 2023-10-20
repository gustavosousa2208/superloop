#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define CAN_RECV_TIMEOUT_USECONDS 20000

struct ThreadData {
    int socket_descriptor;
    // Add any other arguments here
};

void* telegramReceive(void* arg) {

    struct ThreadData* data = (struct ThreadData*) arg;
    int s = data->socket_descriptor;


    struct can_frame frame;
    while (1) {
        ssize_t nbytes = read(s, &frame, sizeof(struct can_frame));
        
        if (nbytes < 0) {
            perror("read");
            break;
        }

        printf("Received CAN frame ID: 0x%X, Data Length: %d, Data: ", frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n"); 
    }
}

uint16_t telegramReceive2(int s) {
    struct timeval timeout;
    struct can_frame frame;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(s, &read_fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = CAN_RECV_TIMEOUT_USECONDS;

    int ret = select(s + 1, &read_fds, NULL, NULL, &timeout);

    if (ret == -1) {
        perror("select");
    } else if (ret == 0) {
        printf("Timeout occurred. No data received within the specified time.\n");
    } else {
        ssize_t nbytes = read(s, &frame, sizeof(struct can_frame));
        if (nbytes == -1) {
            perror("read");
        } else {
            printf("Received CAN frame ID: 0x%X, Data Length: %d, Data: ", frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++) {
                printf("%02X ", frame.data[i]);
            }
            printf("\n"); 
            if(frame.can_id == 0x680){
                return (uint16_t) ((frame.data[0] << 8) | (frame.data[1]));
            } else{
                return (uint16_t) (-1);
            }
        }
    }
}

int createCANSocket(const char* interface_name) {
    int s;
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

    return s; // Return the socket descriptor
}

#endif
#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <string.h>

#define CAN_RECV_TIMEOUT_USECONDS 200000

struct ThreadData {
    int socket_descriptor;
    // Add any other arguments here
};

uint8_t serialSendReceive (const char *serial_interface, uint8_t byte) {
    int serial_port = open(serial_interface, O_RDWR);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Error from tcgetattr");
        return 1;
    }
    // Set the baud rate (in this example, 9600)
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    // 8N1 mode (8 data bits, no parity, 1 stop bit)
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~CRTSCTS;                        // Disable hardware flow control

    tty.c_cflag |= CREAD | CLOCAL;                  // Turn on the receiver and enable the serial port
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Set raw input
    tty.c_oflag &= ~OPOST;                          // Set raw output

    // Set the read() function to return immediately with no data available
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return 1;
    }

    // Write data to the serial port
    uint8_t message[] = {0xdd, 0xa5, 0x03, 0x00, 0xff, 0xfd, 0x77};
    write(serial_port, message, sizeof(message));

    // Read data from the serial port
    uint8_t buffer[47];
    int nbytes = read(serial_port, buffer, sizeof(buffer));
    if (nbytes < 0) {
        perror("Error reading from serial port");
        return 1;
    } else {
        printf("Received :");
        for (int x = 0; x < 47; x++){
            printf(" %02X", buffer[x]);
        }
        int batteryVoltage = (buffer[4] << 8) | (buffer[5]);
        printf("Battery Voltage: %d mV\n", batteryVoltage);
        int batteryCurrent = (buffer[6] << 8) | (buffer[7]);
        printf("Battery Current: %d mA\n", batteryCurrent);
        printf("Power Output: %d\n", batteryCurrent * batteryVoltage);
        
        int remainingCapacity = (buffer[8] << 8) | (buffer[9]);
        printf("Remaining Capacity: %d mAH\n", remainingCapacity*10);
        int totalCapacity = (buffer[10] << 8) | (buffer[11]);
        printf("Total Capacity: %d mA\n", totalCapacity*10);

    }

    close(serial_port);
    return 0;
}

int sendTelegram(int s, struct can_frame *frame) {
    if (write(s, frame, sizeof(struct can_frame)) == -1) {
        perror("Write error");
        return 1;
    }

    printf("CAN message sent\n");

    return 0;
}

void telegramReceivePrint(int s) {

    struct can_frame frame;
    while (1) {
        ssize_t nbytes = read(s, &frame, sizeof(struct can_frame));
        
        if (nbytes < 0) {
            perror("read");
            break;
        }

        // if (frame.can_id == 0x685) {
        //     printf("Commanded Speed (RPM) ID: 0x%X, LEN: %d, DATA: ", frame.can_id, frame.can_dlc);
        //     for (int i = 0; i < frame.can_dlc; i++) {
        //         printf("%02X ", frame.data[i]);
        //     }
        //     printf("\n"); 
        // }
        if (frame.can_id == 0x680) {
            printf("Logical State ID: 0x%X, LEN: %d, DATA: ", frame.can_id, frame.can_dlc);
            printf("\n"); 
        }
        if (frame.can_id == 0x07) {
            int temp = 0;
            temp = (frame.data[1] << 8) | (frame.data[0]);
            float tempf = ((float)temp) / 10;
            printf("Battery Voltage %.1f\n", tempf);
        }
        if (frame.can_id == 0x04) {
            int temp = 0;
            temp = (frame.data[1] << 8) | (frame.data[0]);
            float tempf = ((float)temp) / 10;
            printf("Battery Voltage %.1f\n", tempf);
        }
        if (frame.can_id == 0x03) {
            int temp = 0;
            temp = (frame.data[1] << 8) | (frame.data[0]);
            float tempf = ((float)temp) / 10;
            printf("Motor Current %.1f\n", tempf);
        }

        if (frame.can_id == 0x30) {
            int temp = 0;
            temp = (frame.data[1] << 8) | (frame.data[0]);
            float tempf = ((float)temp) / 10;
            printf("Mosfet 1 Temperature %.1f | %X %X\n", tempf);
        }
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
    return (uint16_t) (-1); 
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
#include "funcs.h"
#include "shares.h"

void *sendInverterData(void * arg) {
    char recv_data[1];
    ssize_t bytesRead;
    struct can_frame thisFrame;

    while (1) {
        bytesRead = read(pipefd[0], recv_data, 1);

        if (bytesRead == -1) {
            perror("Error reading from pipe");
        } else if (bytesRead > 0) {
            if(recv_data[0] == "S"){
                thisFrame.can_id = 0x685;
                thisFrame.can_dlc = 2;

                thisFrame.data[0] = 0x00;
                thisFrame.data[1] = 0x02;
            } else if (recv_data[0] == "P"){
                thisFrame.can_id = 0x685;
                thisFrame.can_dlc = 2;

                thisFrame.data[0] = 0x00;
                thisFrame.data[1] = 0;
            }

            pthread_mutex_lock(&canInterfaceMutex);
            if (write(sock, &thisFrame, sizeof(struct can_frame)) == -1) {
                perror("Write error");
                return NULL;
            }
            pthread_mutex_unlock(&canInterfaceMutex);
        } else {
            // vou fazer essa thread esperar um pouco pra não ficar consumindo processamento
            usleep(100000);  
        }
    }

    return NULL;
}

int sendInverterDataOnce(struct can_frame thisFrame){

    pthread_mutex_lock(&canInterfaceMutex);
    if (write(sock, &thisFrame, sizeof(struct can_frame)) == -1) {
        return -1;
    }
    pthread_mutex_unlock(&canInterfaceMutex);

    return 0;
}

void *readInverterData(void * arg) {
    struct can_frame thisFrame;

    while (1) {
        pthread_mutex_lock(&canInterfaceMutex);
        ssize_t nbytes = read(sock, &thisFrame, sizeof(struct can_frame));
        pthread_mutex_unlock(&canInterfaceMutex);

        if (nbytes < 0) {
            perror("read");
            break;
        }

        pthread_mutex_lock(&inverterDataMutex);
        switch(thisFrame.can_id){
            case 0x701:
                memcpy((uint16_t *) &all_data, thisFrame.data, 8);
                break;
            case 0x702:
                memcpy((uint16_t *) &all_data + 4, thisFrame.data, 6);
                break;
            case 0x703:
                memcpy((uint16_t *) &all_data + 7, thisFrame.data, 8);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&inverterDataMutex);
    }
    return NULL;
}

void* readInverterDataWithTimestamp(void* arg) {
    struct timespec start, end;
    struct can_frame thisFrame;

    while (1) {
        pthread_mutex_lock(&canInterfaceMutex);
        ssize_t nbytes = read(sock, &thisFrame, sizeof(struct can_frame));
        pthread_mutex_unlock(&canInterfaceMutex);

        if (nbytes < 0) {
            perror("read");
            break;
        }

        pthread_mutex_lock(&inverterDataMutex);
        switch(thisFrame.can_id){
            case 0x701:
                memcpy((uint16_t *) &all_data_with_timestamp[0].data, thisFrame.data, 8);
                clock_gettime(CLOCK_MONOTONIC, &all_data_with_timestamp->timestamp);
                break;
            case 0x702:
                memcpy((uint16_t *) &all_data_with_timestamp[1].data, thisFrame.data, 6);
                clock_gettime(CLOCK_MONOTONIC, &all_data_with_timestamp->timestamp);
                break;
            case 0x703:
                memcpy((uint16_t *) &all_data_with_timestamp[2].data, thisFrame.data, 8);
                clock_gettime(CLOCK_MONOTONIC, &all_data_with_timestamp->timestamp);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&inverterDataMutex);

        clock_gettime(CLOCK_MONOTONIC, &lastTelegram);
    }
    return NULL;
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

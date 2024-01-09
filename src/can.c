#include "funcs.h"
#include "shares.h"

struct can_frame thisFrame;

void *sendInverterData(void * arg) {
    struct canSendThreadDataStruct *args = (struct canSendThreadDataStruct *)arg;
    int s = args->socket_descriptor;

    struct can_frame thisFrame = args->frame;
    thisFrame = args->frame;

    pthread_mutex_lock(&canInterfaceMutex);
    if (write(s, &thisFrame, sizeof(struct can_frame)) == -1) {
        perror("Write error");
        return NULL;
    }
    pthread_mutex_unlock(&canInterfaceMutex);

    return NULL;
}

void *readInverterData(void * arg) {
    int *s = (int *)arg;

    int allReceived = 0;
    struct timespec start, end;

    while (1) {
        pthread_mutex_lock(&canInterfaceMutex);
        ssize_t nbytes = read(s, &thisFrame, sizeof(struct can_frame));
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

        clock_gettime(CLOCK_MONOTONIC, &lastTelegram);
    }
    return NULL;
}

// void* readInverterDataWithTimestamp(void* arg) {

// }
    

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

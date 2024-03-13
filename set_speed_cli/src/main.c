#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#include <time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>

// int createCANSocket(const char* interface_name);

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

int modoRemoto(int socket) {
    struct can_frame frame;
    frame.can_id = 0x222;
    frame.can_dlc = 1;
    frame.data[0] = 0x06;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    frame.can_id = 0x220;
    frame.can_dlc = 1;
    frame.data[0] = 0x01;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    frame.can_id = 0x226;
    frame.can_dlc = 1;
    frame.data[0] = 0x00;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    frame.can_id = 0x227;
    frame.can_dlc = 1;
    frame.data[0] = 0x03;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    frame.can_id = 0x228;
    frame.can_dlc = 1;
    frame.data[0] = 0x04;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    frame.can_id = 0x684;
    frame.can_dlc = 1;
    frame.data[0] = 0x00;

    if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int main(int argc, const char * argv[]) {

    int remoto = 0;
    int number = 0;

    if (argc < 2) {
        printf("Por favor, forneça uma letra.\n");
        printf("Exemplo: ./set_speed_cli v 100\n");

        return 1;
    }

    char letter = argv[1][0];
    // printf("Letter: %c\n", letter);

    if (argc >= 3) {
        number = atoi(argv[2]);
        // printf("Number: %d\n", number);
    }

    int socket = createCANSocket("can0");

    if (letter == 'v'){
        time_t current_time;
        char* c_time_string;

        current_time = time(NULL);
        c_time_string = ctime(&current_time);

        printf("INFO %s Socket: %d\n", c_time_string, socket);

        float temp_number = (float) 8192*number/(2878*1.04);
        int final;
        final = floor(temp_number);

        struct can_frame frame;
        frame.can_id = 0x685;
        frame.can_dlc = 2;
        frame.data[0] = (final) & 0xFF;
        frame.data[1] = (final >> 8) & 0xFF;

        printf("INFO sending: %d %X %X\n", final, frame.data[1], frame.data[0]);

        if (send(socket, &frame, sizeof(struct can_frame), 0) == -1) {
            perror("send");
            return -1;
        }        
        printf("INFO speed set sucessfuly");
    } 
    else if (letter == 'c') {
        if(modoRemoto(socket) == -1){
            printf("ERRO: Não foi possível entrar em modo remoto.\n");
            return -1;
        }
        remoto = 1;
    }
    else {
        printf("ERRO: Argumento inválido.\n");
    }

    return 0;
}

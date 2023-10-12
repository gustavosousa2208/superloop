#include "funcs.h"

void mainFlow () {
    const char* can_interface = "vcan0"; // Replace with your CAN interface name

    struct ThreadData data;
    data.socket_descriptor = createCANSocket(can_interface);
    
    if (data.socket_descriptor == -1) {
        // Handle the error
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, telegramReceive, &data) != 0) {
        perror("pthread_create");
        return 1; // Handle the error
    }

    // Continue with other tasks or interactions
    while(1) {
        printf("hey\n");
        usleep(100000);
    }
    // When done, join the thread (optional) and close the socket
    pthread_join(thread, NULL);
    close(data.socket_descriptor);

    return 0;
}

int main() {
    mainFlow();
}

#include "funcs.h"

int mainFlow () {
    const char* can_interface = "vcan0"; // Replace with your CAN interface name

    int sock = createCANSocket(can_interface);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    int recvStatus = telegramReceive2(sock);

    return 0;
}

int main() {
    mainFlow();
    // ui();
}

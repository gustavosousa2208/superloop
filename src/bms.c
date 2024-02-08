#include "funcs.h"
#include "shares.h"


void *serialSendReceive (void* arg) {
    int serial_port = open(serial_interface, O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if ((serial_port, &tty) != 0) {
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

    tty.c_cc[VMIN] = to_read;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return 1;
    }

    uint8_t buffer[to_read] = {0};
    uint8_t message[] = {0xdd, 0xa5, 0x03, 0x00, 0xff, 0xfd, 0x77};
    uint8_t dummy[] = {0xff};
    while(1) {
        for(int k = 0; k < to_read; k++) {
            buffer[k] = 0;
        }
        pthread_mutex_lock(&serialInterfaceMutex);
        write(serial_port, message, 7);
        int nbytes = read(serial_port, buffer, to_read);
        pthread_mutex_unlock(&serialInterfaceMutex);
        if (nbytes < 0) {
            perror("Error reading from serial port");
            return 1;
        } else {
            pthread_mutex_lock(&BMSDataMutex);
            bms_data.sharedBMSVoltage = (buffer[4] << 8) | (buffer[5]);
            bms_data.sharedBMSCurrent = (buffer[6] << 8) | (buffer[7]);
            bms_data.sharedBMSRemainingCapacity = (buffer[8] << 8) | (buffer[9]);
            bms_data.sharedBMSTotalCapacity = (buffer[10] << 8) | (buffer[11]);
            pthread_mutex_unlock(&BMSDataMutex);
        }
        write(serial_port, dummy, 1);
    }
    close(serial_port);
    return;
}
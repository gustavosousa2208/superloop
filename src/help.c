#include "funcs.h"


extern const char *serial_interface;
extern const char *can_interface;

extern volatile int interrupted;

char *choices[] = {
    "Go Remote",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Exit",
    (char *)NULL,
};

extern int sharedCounter;

extern pthread_mutex_t incomingDataMutex;
extern pthread_mutex_t canInterfaceMutex;
extern pthread_mutex_t serialInterfaceMutex;

extern uint16_t sharedCommandedSpeed;
extern uint16_t sharedLogicalState;
extern uint16_t sharedInverterBatteryVoltage;
extern uint16_t sharedInverterMosfetTemperature1;
extern uint16_t sharedInverterMosfetTemperature2;
extern uint16_t sharedInverterAirTemperature;
extern uint16_t sharedMotorCurrent;
extern uint16_t sharedMotorVoltage;
extern uint16_t sharedBMSVoltage;
extern uint16_t sharedBMSCurrent;
extern uint16_t shareBMSTemperature;
extern uint16_t sharedBMSRemainingCapacity;
extern uint16_t sharedBMSTotalCapacity;

extern const int desiredPort;

int createETHSocket () {
    // Step 1: Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Step 2: Set up the server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface
    server_address.sin_port = htons(desiredPort); // Port number (you can choose a different port)

    // Step 3: Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Step 4: Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", desiredPort);

    // Step 5: Accept incoming connections
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Step 6: Handle communication with the client
    // (You can add your communication logic here)

    // Step 7: Close the sockets
    close(client_socket);
    close(server_socket);

    return 0;
}

void sigintHandler(int sig_num) {
    interrupted = 1;
}

void *serialSendReceive (void* arg) {
    int serial_port = open(serial_interface, O_RDWR | O_NOCTTY);
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

    tty.c_cc[VMIN] = 34;
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
        usleep(1000);
        int nbytes = read(serial_port, buffer, to_read);
        pthread_mutex_unlock(&serialInterfaceMutex);
        if (nbytes < 0) {
            perror("Error reading from serial port");
            return 1;
        } else {
            printf("Received :");
            for (int x = 0; x < to_read; x++){
                printf(" %02X", buffer[x]);
            }
            printf("\n");
            pthread_mutex_lock(&incomingDataMutex);
            sharedBMSVoltage = (buffer[4] << 8) | (buffer[5]);
            sharedBMSCurrent = (buffer[6] << 8) | (buffer[7]);
            sharedBMSRemainingCapacity = (buffer[8] << 8) | (buffer[9]);
            sharedBMSTotalCapacity = (buffer[10] << 8) | (buffer[11]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        write(serial_port, dummy, 1);
        usleep(1000000);
    }
    close(serial_port);
    return;
}

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
    struct canReadThreadDataStruct *args = (struct canReadThreadDataStruct *)arg;
    int s = args->socket_descriptor;

    struct can_frame frame;
    while (1) {
        pthread_mutex_lock(&canInterfaceMutex);
        ssize_t nbytes = read(s, &frame, sizeof(struct can_frame));
        pthread_mutex_unlock(&canInterfaceMutex);
        if (nbytes < 0) {
            perror("read");
            break;
        }

        if (frame.can_id == 0x685) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedCommandedSpeed = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x680) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedLogicalState = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x07) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedMotorVoltage = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x04) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedInverterBatteryVoltage = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x03) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedMotorCurrent = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }

        if (frame.can_id == 0x30) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedInverterMosfetTemperature1 = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x33) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedInverterMosfetTemperature2 = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
        if (frame.can_id == 0x33) {
            pthread_mutex_lock(&incomingDataMutex);
            sharedInverterAirTemperature = (frame.data[1] << 8) | (frame.data[0]);
            pthread_mutex_unlock(&incomingDataMutex);
        }
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

void *windowLoop(void* arg) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    refresh();

    struct timespec start, end;
    double elapsed_time = 0;

    int row, col, c, i;
    getmaxyx(stdscr, row, col);

    WINDOW *win = newwin(row - 2, col - 2, 1, 1);
    WINDOW *my_menu_win;
    nodelay(win, TRUE);
    nodelay(stdscr, TRUE);
    nodelay(my_menu_win, TRUE);

    bkgd(COLOR_PAIR(2));

    int n_choices = ARRAY_SIZE(choices);
    ITEM **my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    MENU *my_menu;

    for (int i = 0; i < n_choices; i++) {
        my_items[i] = new_item(choices[i], choices[i]);
    }
    my_items[n_choices] = (ITEM *)NULL;
    my_menu = new_menu((ITEM **)my_items);
    set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
    my_menu_win = newwin(10, 40, 4, 1);
    keypad(my_menu_win, TRUE);

    set_menu_win(my_menu, win);
    set_menu_sub(my_menu, derwin(win, 6, 24, 3, 120));

    set_menu_mark(my_menu, " * ");
    /* Print a border around the main window and print a title */
    mvwhline(win, 9, 0, ACS_HLINE, 41);
    mvwvline(win, 0, 40, ACS_VLINE, 38);
    mvwvline(win, 0, 40 + 37 + 1, ACS_VLINE, 38);

    box(win, 0, 0);

    mvprintw(LINES - 2, 0, "F1 to exit");
    mvwprintw(win, 1, 1, " * Inverter Parameters:");
    mvwprintw(win, 1, 41, " * BMS  Parameters:");

    mvwprintw(win, 10, 1, "* Logical State:");
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 0, 1, "Car Dashboard and Control Panel");
    wattroff(win, COLOR_PAIR(1));

    wbkgd(win, COLOR_PAIR(1));
    set_menu_back(my_menu, COLOR_PAIR(1));
    wrefresh(win);                                                      
    int index;
    double oldTime, meanTime, maxTime = 0;
    char ok;
    char str[100];
    
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        sprintf(str, "Latency: %e", oldTime);
        mvwprintw(win, row - 4, 1, str);

        sprintf(str, "Max. Latency: %e", maxTime);
        mvwprintw(win, row - 4, 41, str);
        c = getch();                                                                                                                             
        switch (c) {
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                wbkgd(win, COLOR_PAIR(1));
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
        }

        pthread_mutex_lock(&incomingDataMutex);
        sprintf(str, " Battery Voltage (inverter): %0.1fV", (float)sharedInverterBatteryVoltage/10);
        pthread_mutex_unlock(&incomingDataMutex);
        mvwprintw(win, 3, 1, str);
        sprintf(str, " Motor Current:              %0.1fA", (float)sharedMotorCurrent/10);
        mvwprintw(win, 4, 1, str);
        sprintf(str, " Motor Voltage:              %0.1fV", (float)sharedMotorVoltage/10);
        mvwprintw(win, 5, 1, str);
        sprintf(str, " Mosfet 1 Temperature:       %0.1fC", (float)sharedInverterMosfetTemperature2/10);
        mvwprintw(win, 6, 1, str);
        sprintf(str, " Mosfet 2 Temperature:       %0.1fC", (float)sharedInverterMosfetTemperature1/10);
        mvwprintw(win, 7, 1, str);
        sprintf(str, " Internal Air Temperature    %0.1fC", (float)sharedInverterMosfetTemperature1/10);
        mvwprintw(win, 8, 1, str);

        uint16_t temp;
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Voltage (BMS):      %0.1fV", (float)sharedBMSVoltage/10);
        temp = sharedBMSVoltage;
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 3, 41, str);

        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Current:            %0.1fA", (float)sharedBMSCurrent/10);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 4, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Temperature:        %0.1fC", (float)shareBMSTemperature/10);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 5, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Remaining Capacity:         %0.1fAh", (float)sharedBMSRemainingCapacity/10);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 6, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Total Capacity:             %0.1fAh", (float)sharedBMSTotalCapacity/10);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 7, 41, str);

        ok = (sharedLogicalState & 8) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Runnning: ", ok);
        mvwprintw(win, 12, 2, str);
        ok = (sharedLogicalState & 9) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Enabled: ", ok);
        mvwprintw(win, 13, 2, str);
        ok = (sharedLogicalState & 10) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Clockwise: ", ok);
        mvwprintw(win, 14, 2, str);
        ok = (sharedLogicalState & 11) > 0 ? '*' : ' ';
        sprintf(str, "[%c] JOG: ", ok);
        mvwprintw(win, 15, 2, str);
        ok = (sharedLogicalState & 12) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Remote: ", ok);
        mvwprintw(win, 16, 2, str);
        ok = (sharedLogicalState & 13) > 0 ? '*' : ' ';
        sprintf(str, "[%c] SUB: ", ok);
        mvwprintw(win, 17, 2, str);
        ok = (sharedLogicalState & 15) > 0 ? '*' : ' ';
        sprintf(str, "[%c] FAULT: ", ok);
        

        if(c == 10) {
            mvwprintw(win, 0, 30, "Item selected is : %s", item_name(current_item(my_menu)));
            break;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        meanTime = (meanTime + elapsed_time) / 2;
        oldTime = elapsed_time;
        if(meanTime > maxTime) {
            maxTime = meanTime;
        }
        wrefresh(win);
    }
    wrefresh(win);
    refresh();

    getch();

    unpost_menu(my_menu);
    free_menu(my_menu);
    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    endwin();

    return 0;
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

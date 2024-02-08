#include "funcs.h"
#include "shares.h"

struct allData all_data_temp;
struct onlyBMSData bms_data_temp;
extern volatile sig_atomic_t ctrlCPressed;

char *choices[] = {
    "Go Remote",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Exit",
    (char *)NULL,
};
const char *menuItems[] = {
    "Modo Remoto",
    "Modo Local",
    "Setar velocidade",
    "Parar Total",
    "JOG ON/OFF"
};
enum {
    LEFT_TEXT_PAIR = 1,
    RIGHT_TEXT_PAIR,
    MENU_PAIR
};  
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
    double oldTime, meanTime, maxTime, lastReceivedTime, lastLastReceivedTime, lat = 0;
    char ok;
    char str[100];
    
    while (!ctrlCPressed) {

        lastReceivedTime = (double) ((double)lastTelegram.tv_sec + (double)lastTelegram.tv_nsec / 10e6);
        if (lastReceivedTime > lastLastReceivedTime) {
            lat = lastReceivedTime - lastLastReceivedTime;
            lastLastReceivedTime = lastReceivedTime;
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        sprintf(str, "Latency (lat): %e", lat);
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
        pthread_mutex_lock(&inverterDataMutex);
        all_data_temp = all_data;
        pthread_mutex_unlock(&inverterDataMutex);
    
        pthread_mutex_lock(&BMSDataMutex);
        bms_data_temp = bms_data;
        pthread_mutex_unlock(&BMSDataMutex);


        sprintf(str, " Battery Voltage (inverter): %0.1fV", (float) all_data_temp.inverterBatteryVoltage/10);
        mvwprintw(win, 3, 1, str);
        sprintf(str, " Motor Current:              %0.1fA", (float) all_data_temp.motorCurrent/10);
        mvwprintw(win, 4, 1, str);
        sprintf(str, " Motor Voltage:              %0.1fV", (float) all_data_temp.motorVoltage/10);
        mvwprintw(win, 5, 1, str);
        sprintf(str, " Mosfet 1 Temperature:       %0.1fC", (float) all_data_temp.inverterMosfetTemperature2/10);
        mvwprintw(win, 6, 1, str);
        sprintf(str, " Mosfet 2 Temperature:       %0.1fC", (float) all_data_temp.inverterMosfetTemperature1/10);
        mvwprintw(win, 7, 1, str);
        sprintf(str, " Internal Air Temperature    %0.1fC", (float) all_data_temp.inverterAirTemperature/10);
        mvwprintw(win, 8, 1, str);

        
        sprintf(str, " Battery Voltage (BMS):      %0.1fV", (float) all_data_temp.sharedBMSVoltage/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 3, 41, str);

        
        sprintf(str, " Battery Current:            %0.1fA", (float) all_data_temp.sharedBMSCurrent/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 4, 41, str);
        
        
        sprintf(str, " Battery Temperature:        %0.1fC", (float)all_data_temp.sharedBMSTemperature/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 5, 41, str);
        
        
        sprintf(str, " Remaining Capacity:         %0dAh", all_data_temp.sharedBMSRemainingCapacity);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 6, 41, str);
        
        
        sprintf(str, " Total Capacity:             %dAh", all_data_temp.sharedBMSTotalCapacity);

        mvwprintw(win, 7, 41, str);

        ok = (all_data_temp.logicalState & 256) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Runnning: ", ok);
        mvwprintw(win, 12, 2, str);
        ok = (all_data_temp.logicalState & 512) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Enabled: ", ok);
        mvwprintw(win, 13, 2, str);
        ok = (all_data_temp.logicalState & 1024) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Clockwise: ", ok);
        mvwprintw(win, 14, 2, str);
        ok = (all_data_temp.logicalState & 2048) > 0 ? '*' : ' ';
        sprintf(str, "[%c] JOG: ", ok);
        mvwprintw(win, 15, 2, str);
        ok = (all_data_temp.logicalState & 4096) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Remote: ", ok);
        mvwprintw(win, 16, 2, str);
        ok = (all_data_temp.logicalState & 8192) > 0 ? '*' : ' ';
        sprintf(str, "[%c] SUB: ", ok);
        mvwprintw(win, 17, 2, str);
        ok = (all_data_temp.logicalState & 32768) > 0 ? '*' : ' ';
        sprintf(str, "[%c] FAULT: ", ok);
        

        if(c == 10) {
            mvwprintw(win, 0, 30, "Item selected is : %s", item_name(current_item(my_menu)));
            break;
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

    uiIsFinished = 1;
    return 0;
}

void *windowLoopTimestamp(void* arg) {
    initscr();
    // raw();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
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
    double oldTime, meanTime, maxTime, lastReceivedTime, lastLastReceivedTime, lat = 0;
    char ok;
    char str[100];
    
    while (1) {
        
        while((c = getch())) {   
            switch(c) {
            	case KEY_DOWN:
                    menu_driver(my_menu, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(my_menu, REQ_UP_ITEM);
                    break;
            }
        }	


        pthread_mutex_lock(&inverterDataMutex);
        all_data_temp = all_data;
        all_data_temp.inverterBatteryVoltage = all_data_with_timestamp[2].data[1];
        pthread_mutex_unlock(&inverterDataMutex);

        sprintf(str, " Battery Voltage (inverter): %0.1fV", (float) all_data_temp.inverterBatteryVoltage/10);
        mvwprintw(win, 3, 1, str);
        sprintf(str, " Motor Current:              %0.1fA", (float) all_data_temp.motorCurrent/10);
        mvwprintw(win, 4, 1, str);
        sprintf(str, " Motor Voltage:              %0.1fV", (float) all_data_temp.motorVoltage/10);
        mvwprintw(win, 5, 1, str);
        sprintf(str, " Mosfet 1 Temperature:       %0.1fC", (float) all_data_temp.inverterMosfetTemperature2/10);
        mvwprintw(win, 6, 1, str);
        sprintf(str, " Mosfet 2 Temperature:       %0.1fC", (float) all_data_temp.inverterMosfetTemperature1/10);
        mvwprintw(win, 7, 1, str);
        sprintf(str, " Internal Air Temperature    %0.1fC", (float) all_data_temp.inverterMosfetTemperature1/10);
        mvwprintw(win, 8, 1, str);
        sprintf(str, " Battery Voltage (BMS):      %0.1fV", (float) all_data_temp.sharedBMSVoltage/100);
        mvwprintw(win, 3, 41, str);
        sprintf(str, " Battery Current:            %0.1fA", (float) all_data_temp.sharedBMSCurrent/100);
        mvwprintw(win, 4, 41, str);
        sprintf(str, " Battery Temperature:        %0.1fC", (float)all_data_temp.sharedBMSTemperature/100);
        mvwprintw(win, 5, 41, str);
        sprintf(str, " Remaining Capacity:         %0dAh", all_data_temp.sharedBMSRemainingCapacity);
        mvwprintw(win, 6, 41, str);
        sprintf(str, " Total Capacity:             %dAh", all_data_temp.sharedBMSTotalCapacity);
        mvwprintw(win, 7, 41, str);

        ok = (all_data_temp.logicalState & 256) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Runnning: ", ok);
        mvwprintw(win, 12, 2, str);
        ok = (all_data_temp.logicalState & 512) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Enabled: ", ok);
        mvwprintw(win, 13, 2, str);
        ok = (all_data_temp.logicalState & 1024) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Clockwise: ", ok);
        mvwprintw(win, 14, 2, str);
        ok = (all_data_temp.logicalState & 2048) > 0 ? '*' : ' ';
        sprintf(str, "[%c] JOG: ", ok);
        mvwprintw(win, 15, 2, str);
        ok = (all_data_temp.logicalState & 4096) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Remote: ", ok);
        mvwprintw(win, 16, 2, str);
        ok = (all_data_temp.logicalState & 8192) > 0 ? '*' : ' ';
        sprintf(str, "[%c] SUB: ", ok);
        mvwprintw(win, 17, 2, str);
        ok = (all_data_temp.logicalState & 32768) > 0 ? '*' : ' ';
        sprintf(str, "[%c] FAULT: ", ok);
        

        if(c == 10) {
            mvwprintw(win, 0, 30, "Item selected is : %s", item_name(current_item(my_menu)));
            break;
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

    uiIsFinished = 1;
    return 0;
}

#define WIDTH 40

void curses2(void *arg) {
    initscr(); // Initialize curses

    start_color();
    // init_pair(LEFT_TEXT_PAIR, COLOR_WHITE, COLOR_BLUE);
    // init_pair(RIGHT_TEXT_PAIR, COLOR_WHITE, COLOR_GREEN);
    // init_pair(MENU_PAIR, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(LEFT_TEXT_PAIR, COLOR_BLACK, COLOR_WHITE);
    init_pair(RIGHT_TEXT_PAIR, COLOR_BLACK, COLOR_WHITE);
    init_pair(MENU_PAIR, COLOR_BLACK, COLOR_WHITE);

    noecho(); // Do not display input characters
    keypad(stdscr, TRUE); // Enable special key input
    
    int height, width;
    getmaxyx(stdscr, height, width); // Get the screen size

    WINDOW *leftTextWin = newwin(height, WIDTH, 0, 0); // Create left text window
    wbkgd(leftTextWin, COLOR_PAIR(LEFT_TEXT_PAIR)); // Set background color
    WINDOW *rightTextWin = newwin(height, WIDTH, 0, WIDTH + 1); // Create right text window
    wbkgd(rightTextWin, COLOR_PAIR(RIGHT_TEXT_PAIR)); // Set background color
    WINDOW *menuWin = newwin(height, WIDTH, 0, 2 * (WIDTH + 1)); // Create menu window
    wbkgd(menuWin, COLOR_PAIR(MENU_PAIR)); // Set background color


    int leftTextVariable = 0; // Example variable for left text window
    int rightTextVariable = 0; // Example variable for right text window
    int menuVariable = 0; // Example variable for menu window

    int menuIndex = 0; // Index of the selected menu item

    int ch;
    int currentWindow = 2; 
    char ok;
    char str[100] = {0};


    halfdelay(1); // Set a maximum time for waiting for a keypress (1/10th of a second)

    while ((ch = getch()) != 'q') {
        // Check for user input
        if (ch != ERR) {
            // User pressed a key
            switch (ch) {
                case KEY_LEFT:
                    currentWindow = (currentWindow + 2) % 3; // Move left through windows
                    break;
                case KEY_RIGHT:
                    currentWindow = (currentWindow + 1) % 3; // Move right through windows
                    break;
                case KEY_UP:
                    if (currentWindow == 2) {
                        menuIndex = (menuIndex - 1 + sizeof(menuItems) / sizeof(menuItems[0])) % (sizeof(menuItems) / sizeof(menuItems[0]));
                    }
                    break;
                case KEY_DOWN:
                    if (currentWindow == 2) {
                        menuIndex = (menuIndex + 1) % (sizeof(menuItems) / sizeof(menuItems[0]));
                    }
                    break;
                case ' ':
                    // Switch to the next window
                    currentWindow = (currentWindow + 1) % 3;
                    break;
                case 10: // Enter key code
                    if (currentWindow == 2) {
                        // Handle selection of the menu item (for example, print it)
                        mvprintw(height - 2, 0, "Selected: %s", menuItems[menuIndex]);
                        if(menuIndex == 1) {
                            
                        }
                        refresh();
                        getch(); // Wait for user input
                    }
                    break;
            }
        } else {
            // No user input, update variables or other non-input logic here
            // leftTextVariable++;
            // rightTextVariable--;
            menuVariable += 2;
        }

        //create horizontal line in the first row
        // for (int i = 0; i < width; i++) {
        //     mvwaddch(leftTextWin, 0, i, ACS_HLINE);
        //     mvwaddch(rightTextWin, 0, i, ACS_HLINE);
        //     mvwaddch(menuWin, 0, i, ACS_HLINE);
        // }
        mvwhline(leftTextWin, 0, 0, ACS_HLINE, width - 2);
        mvwhline(leftTextWin, 9, 0, ACS_HLINE, width - 2);
        mvwhline(leftTextWin, 16, 0, ACS_HLINE, width - 2);
        mvwhline(leftTextWin, 18, 0, ACS_HLINE, width - 2);
        mvwhline(rightTextWin, 0, 0, ACS_HLINE, width - 2);
        mvwhline(rightTextWin, 9, 0, ACS_HLINE, width - 2);
        mvwhline(rightTextWin, 16, 0, ACS_HLINE, width - 2);
        mvwhline(rightTextWin, 18, 0, ACS_HLINE, width - 2);
        mvwhline(menuWin, 0, 0, ACS_HLINE, width - 2);
        mvwhline(menuWin, 9, 0, ACS_HLINE, width - 2);
        mvwhline(menuWin, 16, 0, ACS_HLINE, width - 2);
        mvwhline(menuWin, 18, 0, ACS_HLINE, width - 2);

        pthread_mutex_lock(&inverterDataMutex);
        all_data_temp = all_data;
        pthread_mutex_unlock(&inverterDataMutex);

        mvwprintw(leftTextWin, 1, 1, "Dados do Inversor");
        sprintf(str, "* Tensao da Bateria:           %0.1fV", (float) all_data_temp.inverterBatteryVoltage/10);
        mvwprintw(leftTextWin, 3, 1, str);
        sprintf(str, "* Corrente do Motor:           %0.1fA", (float) all_data_temp.motorCurrent/10);
        mvwprintw(leftTextWin, 4, 1, str);
        sprintf(str, "* Tensao do Motor:             %0.1fV", (float) all_data_temp.motorVoltage/10);
        mvwprintw(leftTextWin, 5, 1, str);
        sprintf(str, "* Temperatura do Mosfet 1:     %0.1fC", (float) all_data_temp.inverterMosfetTemperature1/10);
        mvwprintw(leftTextWin, 6, 1, str);
        sprintf(str, "* Temperatura do Mosfet 2:     %0.1fC", (float) all_data_temp.inverterMosfetTemperature2/10);
        mvwprintw(leftTextWin, 7, 1, str);
        sprintf(str, "* Temperatura Interna do Ar:   %0.1fC", (float) all_data_temp.inverterAirTemperature/10);
        mvwprintw(leftTextWin, 8, 1, str);

        pthread_mutex_lock(&BMSDataMutex);
        bms_data_temp = bms_data;
        pthread_mutex_unlock(&BMSDataMutex);

        mvwprintw(rightTextWin, 1, 1, "Dados do BMS");
        sprintf(str, "* Battery Voltage (BMS):      %0.1fV", (float) bms_data_temp.sharedBMSVoltage/100);
        mvwprintw(rightTextWin, 3, 1, str);
        sprintf(str, "* Battery Current:            %0.1fA", (float) bms_data_temp.sharedBMSCurrent/100);
        mvwprintw(rightTextWin, 4, 1, str);
        sprintf(str, "* Battery Temperature:        %0.1fC", (float)bms_data_temp.sharedBMSTemperature/100);
        mvwprintw(rightTextWin, 5, 1, str);
        sprintf(str, "* Remaining Capacity:         %0dAh", bms_data_temp.sharedBMSRemainingCapacity);
        mvwprintw(rightTextWin, 6, 1, str);
        sprintf(str, "* Total Capacity:             %dAh", bms_data_temp.sharedBMSTotalCapacity);
        mvwprintw(rightTextWin, 7, 1, str);

        ok = (all_data_temp.logicalState & 256) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Girando: ", ok);
        mvwprintw(leftTextWin, 10, 2, str);
        ok = (all_data_temp.logicalState & 512) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Habilitado: ", ok);
        mvwprintw(leftTextWin, 11, 2, str);
        ok = (all_data_temp.logicalState & 1024) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Sent. Horario: ", ok);
        mvwprintw(leftTextWin, 12, 2, str);
        ok = (all_data_temp.logicalState & 2048) > 0 ? '*' : ' ';
        sprintf(str, "[%c] JOG: ", ok);
        mvwprintw(leftTextWin, 13, 2, str);
        ok = (all_data_temp.logicalState & 4096) > 0 ? '*' : ' ';
        sprintf(str, "[%c] Remoto: ", ok);
        mvwprintw(leftTextWin, 14, 2, str);
        ok = (all_data_temp.logicalState & 8192) > 0 ? '*' : ' ';
        sprintf(str, "[%c] SUB: ", ok);
        mvwprintw(leftTextWin, 15, 2, str);
        ok = (all_data_temp.logicalState & 32768) > 0 ? '*' : ' ';
        sprintf(str, "[%c] FALTA: ", ok);

        mvwprintw(leftTextWin, 17, 2, "Latencia: %d ms", 80);
        mvwprintw(rightTextWin, 17, 2, "Ultimo telegrama: %d ms", 23);

        mvwprintw(menuWin, 1, 1, "Menu:");
        for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); ++i) {
            if (i == menuIndex) {
                wattron(menuWin, A_REVERSE);
            }
            mvwprintw(menuWin, i + 3, 2, "%s", menuItems[i]);
            wattroff(menuWin, A_REVERSE);
        }

        // Highlight the active window
        // wattron(leftTextWin, A_NORMAL);
        // wattron(rightTextWin, A_NORMAL);
        // wattron(menuWin, A_NORMAL);

        // switch (currentWindow) {
        //     case 0:
        //         wattron(leftTextWin, A_REVERSE);
        //         break;
        //     case 1:
        //         wattron(rightTextWin, A_REVERSE);
        //         break;
        //     case 2:
        //         wattron(menuWin, A_REVERSE);
        //         break;
        // }

        // Refresh each window
        wrefresh(leftTextWin);
        wrefresh(rightTextWin);
        wrefresh(menuWin);
    }

    // End curses
    endwin();
    uiIsFinished = 1;

    return 0;
}

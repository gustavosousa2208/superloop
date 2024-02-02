#include "funcs.h"
#include "shares.h"

struct allData all_data_temp;

char *choices[] = {
    "Go Remote",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Exit",
    (char *)NULL,
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
    
    while (1) {

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

        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Voltage (BMS):      %0.1fV", (float) all_data_temp.sharedBMSVoltage/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 3, 41, str);

        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Current:            %0.1fA", (float) all_data_temp.sharedBMSCurrent/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 4, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Temperature:        %0.1fC", (float)all_data_temp.sharedBMSTemperature/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 5, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Remaining Capacity:         %0dAh", all_data_temp.sharedBMSRemainingCapacity);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 6, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Total Capacity:             %dAh", all_data_temp.sharedBMSTotalCapacity);
        pthread_mutex_unlock(&serialInterfaceMutex);
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
    
    while (1) {

        // lastReceivedTime = (double) ((double)lastTelegram.tv_sec + (double)lastTelegram.tv_nsec / 10e6);
        // if (lastReceivedTime > lastLastReceivedTime) {
        //     lat = lastReceivedTime - lastLastReceivedTime;
        //     lastLastReceivedTime = lastReceivedTime;
        // }

        // clock_gettime(CLOCK_MONOTONIC, &start);
        // sprintf(str, "Latency (lat): %e", lat);
        // mvwprintw(win, row - 4, 1, str);

        // sprintf(str, "Max. Latency: %e", maxTime);
        // mvwprintw(win, row - 4, 41, str);
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

        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Voltage (BMS):      %0.1fV", (float) all_data_temp.sharedBMSVoltage/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 3, 41, str);

        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Current:            %0.1fA", (float) all_data_temp.sharedBMSCurrent/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 4, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Battery Temperature:        %0.1fC", (float)all_data_temp.sharedBMSTemperature/100);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 5, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Remaining Capacity:         %0dAh", all_data_temp.sharedBMSRemainingCapacity);
        pthread_mutex_unlock(&serialInterfaceMutex);
        mvwprintw(win, 6, 41, str);
        
        pthread_mutex_lock(&serialInterfaceMutex);
        sprintf(str, " Total Capacity:             %dAh", all_data_temp.sharedBMSTotalCapacity);
        pthread_mutex_unlock(&serialInterfaceMutex);
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

#include "funcs.h"

const char *serial_interface = "/dev/ttyUSB0";
const char *can_interface = "can0";
char input_text[5] = {0};
WINDOW *win;

// Enumeration for button options
enum ButtonOption {
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_COUNT
};

int selected_button = BUTTON_1;

void draw_buttons() {
    mvwprintw(win, 3, 2, (selected_button == BUTTON_1) ? "[Button 1]" : " Button 1 ");
    mvwprintw(win, 3, 15, (selected_button == BUTTON_2) ? "[Button 2]" : " Button 2 ");
    mvwprintw(win, 3, 28, (selected_button == BUTTON_3) ? "[Button 3]" : " Button 3 ");
    wrefresh(win);
}

void* getInput(void* arg) {
    mvwgetnstr(win, 1, 14, input_text, sizeof(input_text) - 1);
    return NULL;
}

int windowLoop() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide the cursor

    int height = 10;
    int width = 40;
    int y = (LINES - height) / 2;
    int x = (COLS - width) / 2;

    win = newwin(height, width, y, x);
    box(win, 0, 0);

    wrefresh(win);
    mvwprintw(win, 1, 2, "Enter text:");
    wrefresh(win);

    selected_button = BUTTON_1;

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, getInput, NULL);

    while (1) {
        // Display buttons and handle user input
        draw_buttons();
        wrefresh(win);
        // refresh();
        
        int key = getch();

        // Handle arrow key presses for button selection
        switch (key) {
            case KEY_LEFT:
                selected_button = (selected_button == BUTTON_1) ? BUTTON_3 : (selected_button - 1);
                break;
            case KEY_RIGHT:
                selected_button = (selected_button == BUTTON_3) ? BUTTON_1 : (selected_button + 1);
                break;
            case '\n':
                break;
        }
        if (key == '\n'){
            break;
        }
    }

    // Wait for the input thread to finish
    pthread_join(input_thread, NULL);

    // Display text alongside
    mvwprintw(win, 5, 2, "Text alongside:");
    mvwprintw(win, 6, 2, input_text);
    wrefresh(win);

    // Wait for a key press to exit
    getch();

    endwin();

    return 0;
}

int mainFlow () {
    int sock = createCANSocket(can_interface);
    
    if (sock == -1) {
        printf("Failed to create the CAN socket.\n");
        return 1;
    }

    // serialSendReceive(serial_interface, 0x55);
    // int recvStatus = telegramReceive2(sock);
    telegramReceivePrint(sock);

    return 0;
}

void whichKey() {
    int ch = getch(); endwin(); printf("KEY NAME : %s - %d\n", keyname(ch),ch);
    return;
}

void getCommandLineArguments(int argc, char *argv[]) {
    const char *input_speed = NULL;
    const char *mode = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            // Check if there's another argument after -i
            if (i + 1 < argc) {
                input_speed = argv[i + 1];
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Error: -s option requires an argument.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            // Check if there's another argument after -o
            if (i + 1 < argc) {
                mode = argv[i + 1];
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Error: -o option requires an argument.\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            return 1;
        }
    }
}

int main(int argc, char *argv[]) {
    
    // windowLoop();

    mainFlow();

    return 0;
}

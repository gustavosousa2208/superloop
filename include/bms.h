#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>

#include <termios.h>

#include <ncurses.h>
#include <menu.h>

#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

void *serialSendReceive (void* arg);
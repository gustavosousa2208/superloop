#ifndef CONFIG_H
#define CONFIG_H
#endif


#ifdef DEBUG
    #define CAN_INTERFACE "vcan0"
#else
    #define CAN_INTERFACE "can0"
#endif

#define SERVER_PORT 12345

#define CONFIG_H_VERSION 1
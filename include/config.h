#ifndef CONFIG_H
#define CONFIG_H
#endif

#ifdef EXPERIMENTAL
    #define TIMESTAMP_VER1 false
#else
    #define TIMESTAMP_VER1 true
#endif

#ifdef DEBUG
    #define CAN_INTERFACE "vcan0"
#else
    #define CAN_INTERFACE "can0"
#endif

#define CONFIG_H_VERSION 1
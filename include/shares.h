#ifndef SHARES_H
#define SHARES_H

extern const char *can_interface;

extern pthread_mutex_t canInterfaceMutex;
extern pthread_mutex_t inverterDataMutex;

extern uint16_t sharedCommandedSpeed;
extern uint16_t sharedLogicalState;
extern uint16_t sharedInverterBatteryVoltage;
extern uint16_t sharedInverterMosfetTemperature1;
extern uint16_t sharedInverterMosfetTemperature2;
extern uint16_t sharedInverterAirTemperature;
extern uint16_t sharedMotorCurrent;
extern uint16_t sharedMotorVoltage;

extern int sharedCounter;

extern pthread_mutex_t serialInterfaceMutex;
extern pthread_mutex_t BMSDataMutex;

extern uint16_t sharedBMSVoltage;
extern uint16_t sharedBMSCurrent;
extern uint16_t shareBMSTemperature;
extern uint16_t sharedBMSRemainingCapacity;
extern uint16_t sharedBMSTotalCapacity;

extern double allInverterDataDeltaTime;

extern const int desiredPort;

extern const char *serial_interface;

extern volatile int uiIsFinished;

extern struct timespec lastTelegram;
extern struct can_frame thisFrame;

#endif // SHARES_H
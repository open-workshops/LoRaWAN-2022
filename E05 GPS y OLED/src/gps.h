#ifndef GPS_H_
#define GPS_H_

#include <HardwareSerial.h>
#include "TinyGPS++.h" //https://github.com/mikalhart/TinyGPSPlus (V1.0.2)

//GPS config
#define GPS_RX_PIN   12
#define GPS_TX_PIN   13 
#define GPS_BAUDRATE 9600

void setupGPS(void);
void updateGPS(void);
void printGPSdata(void);
void smartGPSdelay(unsigned long ms);

#endif
/**********************************************
 * DriverSensorData.h
 * Header for sensor data driver.
 **********************************************/

#ifndef _DRIVERSENSORDATA_H_
#define _DRIVERSENSORDATA_H_

// Structs

typedef struct _SENSOR_DATA
{
    double power;
    long   time;
} sensor_data;

// Function Prototypes

extern int    Driver_SensorDataInit(void);
extern double Driver_SensorDataGetEnergyKwh(long startTime, long endTime);
extern double Driver_SensorDataGetEnergyKwhFromString(char * inputString, int size);

#endif
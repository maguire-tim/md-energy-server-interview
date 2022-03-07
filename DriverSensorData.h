/**********************************************
 * DriverSensorData.h
 * Header for sensor data driver.
 **********************************************/

#ifndef _DRIVERSENSORDATA_H_
#define _DRIVERSENSORDATA_H_

extern void Driver_SocketInit(void);
extern int  Driver_SocketStart(void);
extern int  Driver_SocketAwaitConnection(int socketDescriptor);
extern int  Driver_SocketSendMessage(int socketDescriptor, char * data, int dataSize);
extern int  Driver_SocketReceiveMessage(int socketDescriptor, char * buffer, int bufferSize);

#endif
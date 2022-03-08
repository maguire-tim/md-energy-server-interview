/**********************************************
 * DriverSocket.h
 * Header for socket driver.
 **********************************************/

#ifndef _DRIVERSOCKET_H_
#define _DRIVERSOCKET_H_

// Function Prototypes
extern void Driver_SocketInit(void);
extern int  Driver_SocketStart(void);
extern int  Driver_SocketAwaitConnection(int socketDescriptor);
extern int  Driver_SocketSendMessage(int socketDescriptor, char * data, int dataSize);
extern int  Driver_SocketReceiveMessage(int socketDescriptor, char * buffer, int bufferSize);

#endif
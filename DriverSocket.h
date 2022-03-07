/**********************************************
 * DriverSocket.h
 * Header for socket driver.
 **********************************************/

#ifndef _DRIVERSOCKET_H_
#define _DRIVERSOCKET_H_

extern void Driver_SocketInit(void);
extern int  Driver_SocketStart(void);
extern int  Driver_SocketAwaitConnection(int socketDescriptor);
//extern void Driver_SocketClose(void);

#endif
/**********************************************
 * Main.c
 * Entry point of the program.
 **********************************************/

// Includes
#include <stdio.h>
#include <string.h>

#include "DriverSocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Defines
#define BUFFER_SIZE 1024

// Program entry point
int main(void)
{
	//printf("entry\n");
	Driver_SocketInit();
	//printf("init\n");
	int socketDesc = Driver_SocketStart();
	//printf("start, socket desc: %d\n", socketDesc);
	//char str[20];
	//sprintf(str, "%d", socketDesc);
	//printf("%s", str);
	int connectedSocketDesc = Driver_SocketAwaitConnection(socketDesc);
	//printf("connected to: %d\n", connectedSocketDesc);
	//sprintf(str, "%d", connectedSocketDesc);
	//printf("%s", str);
	char buffer[BUFFER_SIZE];
	char response[] = "test response";
	int valread = recv(connectedSocketDesc, buffer, BUFFER_SIZE, 0);
	//printf("VALREAD: %d\nBUFFER DATA: %s\n", valread, buffer);
	send(connectedSocketDesc, response, strlen(response), 0);
	//printf("sent reponse\n");
	shutdown(connectedSocketDesc, 2);
	shutdown(socketDesc, 2);
}

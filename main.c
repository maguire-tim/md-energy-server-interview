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
#define RX_BUFFER_SIZE 1024

// Program entry point
int main(void)
{
    Driver_SocketInit();

    int socketDesc = Driver_SocketStart();

    int connectedSocketDesc = Driver_SocketAwaitConnection(socketDesc);

    char buffer[RX_BUFFER_SIZE];
    Driver_SocketReceiveMessage(connectedSocketDesc, buffer, RX_BUFFER_SIZE);

    char response[] = "test response";
    Driver_SocketSendMessage(connectedSocketDesc, response, strlen(response));
}

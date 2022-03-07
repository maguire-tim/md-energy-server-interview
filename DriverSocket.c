/**********************************************
 * DriverSocket.c
 * Driver functions for socket.
 **********************************************/

// Includes

#include "DriverSocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include <stdio.h>
#include <errno.h>

// Defines

#define COMM_PORT       8080
#define COMM_DOMAIN     AF_INET     // IPv4
#define COMM_TYPE       SOCK_STREAM // TCP
#define ADDRESS         INADDR_ANY
#define BACKLOG_SIZE    100

// Variables

static struct sockaddr_in Address, Client;

// Functions

// Initializes this module. Must be called before any other functions in this file.
void Driver_SocketInit(void)
{
    Address.sin_family      = COMM_DOMAIN;
    Address.sin_addr.s_addr = ADDRESS;
    Address.sin_port        = htons(COMM_PORT);
}

// Creates the socket attached to the specified port. Returns socket descriptor (0 if any error occurs).
int Driver_SocketStart(void)
{
    // Create socket
    //printf("    Creating socket...\n");
    int socketDesc = socket(COMM_DOMAIN, COMM_TYPE, 0);
    // Return error if creating socket fails
    if (socketDesc == 0) return -1;

    // Allow reuse of address
    int set = 1;
    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set)) < 0) return -1;

    //printf("    Binding socket...\n");
    // Bind socket to specified port. Return error if bind fails
    if (bind(socketDesc, (struct sockaddr *) &Address, sizeof(Address)) < 0)
    {
        printf("errno = %d\n", errno);
        return -1;
    }
    //printf("    Address: %d, %d, %d\n", Address.sin_family, Address.sin_addr.s_addr, Address.sin_port);

    //printf("    Entering listen...\n");
    // Enter listen mode. Return false if this fails
    if (listen(socketDesc, BACKLOG_SIZE) < 0) return -1;
    else return socketDesc;
}

// Waits for a connection and returns the new connected socket's descriptor. This is a blocking function.
// The specified socket must be in the listening state.
int Driver_SocketAwaitConnection(int socketDescriptor)
{
    char httpHeader[] = "HTTP/1.1 200 OK\r\n\n";
    int addrSize = sizeof(Client);
    int connectedSocketDesc = accept(socketDescriptor, (struct sockaddr *) &Client, (socklen_t *) &addrSize);
    if (connectedSocketDesc == -1)
    {
        printf("errno = %d\n", errno);
    }
    send(connectedSocketDesc, httpHeader, sizeof(httpHeader), 0);

    return connectedSocketDesc;
}

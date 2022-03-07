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
#include <string.h>

#include <stdio.h>
#include <errno.h>

// Defines

#define COMM_PORT        8080
#define COMM_DOMAIN      AF_INET     // IPv4
#define COMM_TYPE        SOCK_STREAM // TCP
#define ADDRESS          INADDR_ANY
#define BACKLOG_SIZE     100

#define HTTP_HEADER_SIZE 64 // Allows for 3-digit content length
#define MAX_MESSAGE_DATA_SIZE 128
#define MAX_MESSAGE_SIZE (HTTP_HEADER_SIZE + MAX_MESSAGE_DATA_SIZE)

// Variables

static struct sockaddr_in Address, Client;

// Local Function Declarations

static void CreateHttpMessage(char * data, int dataSize, char * message);

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
    int socketDesc = socket(COMM_DOMAIN, COMM_TYPE, 0);
    // Return error if creating socket fails
    if (socketDesc == 0) return -1;

    // Allow reuse of address
    int set = 1;
    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set)) < 0) return -1;

    // Bind socket to specified port. Return error if bind fails
    if (bind(socketDesc, (struct sockaddr *) &Address, sizeof(Address)) < 0)
    {
        printf("errno = %d\n", errno);
        return -1;
    }

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

    return connectedSocketDesc;
}

// Appends HTTP header to the data to send and sends the message. Returns operation status.
int Driver_SocketSendMessage(int socketDescriptor, char * data, int dataSize)
{
    char message[MAX_MESSAGE_SIZE];
    CreateHttpMessage(data, dataSize, message);
    return write(socketDescriptor, message, strlen(message));
}

// Receives incoming message into the specified buffer, up to bufferSize. Returns operation status.
int Driver_SocketReceiveMessage(int socketDescriptor, char * buffer, int bufferSize)
{
    return recv(socketDescriptor, buffer, bufferSize, 0);
}

// Adds appropriate HTTP header to a data string and stores it in message.
static void CreateHttpMessage(char * data, int dataSize, char * message)
{
    // Put header
    sprintf(message, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: %d\n\n", dataSize);
    strcat(message, data);
}

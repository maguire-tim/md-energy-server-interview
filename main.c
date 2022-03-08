/**********************************************
 * Main.c
 * Entry point of the program.
 **********************************************/

/*
    List of assumptions about user input:
    - The input from the client should be in the format of "starttime=x&endtime=y".
    - If the client sends any input not in the specified format, the result returned will be -1 kWh.
    - If the client sends start/end times that are invalid (end before start, or either time out of
        bounds of the data), the result returnd will be -1 kWh.
    - Only one client will attempt to communicate with the server at a time.
    (Assumptions about data in sensors.json are located in DriverSensorData.c)
*/

// Includes
#include <stdio.h>
#include <string.h>

#include "DriverSocket.h"
#include "DriverSensorData.h"

// Defines
#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

// Program entry point
int main(void)
{
    // Load sensor data into internal buffer
    Driver_SensorDataInit();
    // Initialize socket driver
    Driver_SocketInit();
    // Start socket
    int socketDesc = Driver_SocketStart();
    if (socketDesc < 0)
    {
        printf("\nProgram ended due to error starting socket.\nEnsure no other instances of this program are running and try again.\n");
        return -1;
    }

    while (1)
    {
        // Wait for connection from client
        int connectedSocketDesc = Driver_SocketAwaitConnection(socketDesc);
        if (connectedSocketDesc < 0)
        {
            printf("Error connecting to client.\n");
        }
        // Receive and store incoming message
        char rxBuffer[RX_BUFFER_SIZE];
        Driver_SocketReceiveMessage(connectedSocketDesc, rxBuffer, RX_BUFFER_SIZE);

        // Parse the incoming message and calculate the result
        double result = Driver_SensorDataGetEnergyKwhFromString(rxBuffer, strlen(rxBuffer));

        // Format the output message and send it
        char txBuffer[TX_BUFFER_SIZE];
        sprintf(txBuffer, "{\"results\":{\"energy\":%lf,\"units\":\"kWh\"}}\n", result);
        Driver_SocketSendMessage(connectedSocketDesc, txBuffer, strlen(txBuffer));
    }
    Driver_SocketStop(socketDesc);
    return 0;
}

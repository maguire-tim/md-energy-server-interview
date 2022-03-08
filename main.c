/**********************************************
 * Main.c
 * Entry point of the program.
 **********************************************/

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
            printf("\nProgram ended due to error connecting to client.\n");
            return -1;
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
    return 0;
}

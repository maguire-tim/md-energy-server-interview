/**********************************************
 * DriverSensorData.c
 * Driver functions for retrieving sensor data.
 **********************************************/

/*
    List of assumptions about data in sensors.json:
    - Follows the format of [{"current": i.i, "voltage": v.v, "time": t}, ... ]
    - Current is in amps, voltage is in volts, time is a timestamp in seconds
    - The number of datapoints is less than the value defined as MAX_DATAPOINT_COUNT
    - Current and voltage values will fit in a double, time values will fit in a long int
    - The current and voltage values provided at each datapoint are assumed constant until the time of the next datapoint.
    - The datapoints are sorted in order of increasing time
*/

// Includes

#include "DriverSensorData.h"

#include <stdio.h>
#include <string.h>

#include <errno.h>

// Defines

#define MAX_LABEL_SIZE          16    // Larger than the length of a label (e.g., "voltage":)
#define MAX_DATAPOINT_COUNT     16192 // Larger than the number of datapoints in the provided file
#define FILENAME                "sensors.json"

#define CURRENT_LABEL           "current"
#define VOLTAGE_LABEL           "voltage"
#define TIME_LABEL              "time"

#define SECONDS_PER_HOUR        3600.0
#define W_PER_KW                1000.0

// Variables

static sensor_data SensorDataList[MAX_DATAPOINT_COUNT];
static int NumDatapoints;

// Local Function Declarations

// Functions

// Loads data from sensors.json into RAM. Returns 0 if successful, -1 otherwise.
int Driver_SensorDataInit(void)
{
    FILE * file = fopen(FILENAME, "r");
    if (file == NULL) return -1;

    // Check first character
    char firstChar = getc(file);
    if (firstChar != '[') return -1;   // Format error (incorrect first character)

    // Scan file for data
    NumDatapoints = 0;
    for (int i = 0; i < MAX_DATAPOINT_COUNT; ++i)
    {
        char    currentLabel[MAX_LABEL_SIZE];
        char    voltageLabel[MAX_LABEL_SIZE];
        char    timeLabel[MAX_LABEL_SIZE];
        double  currentVal;
        double  voltageVal;
        long    timeVal;

        if (fscanf(file, "{%s %lf, %s %lf, %s %ld}, ", currentLabel, &currentVal, voltageLabel, &voltageVal, timeLabel, &timeVal) == 0)
        {
            // Finished scanning (or format error)
            break;
        }

        // Check if labels are present in the right place. Ignore case just in case
        if (strstr(currentLabel, CURRENT_LABEL) == NULL)
        {
            // Incorrect label
            return -1;
        }
        if (strstr(voltageLabel, VOLTAGE_LABEL) == NULL)
        {
            // Incorrect label
            return -1;
        }
        if (strstr(timeLabel, TIME_LABEL) == NULL)
        {
            // Incorrect label
            return -1;
        }
        // Labels all match, store the values for this datapoint
        SensorDataList[i].power = voltageVal * currentVal;
        SensorDataList[i].time  = timeVal;
        ++NumDatapoints;
    }
    return 0;
}

// Search for the start and end datapoints, then calculate and return the energy consumption between them.
double Driver_SensorDataGetEnergyKwh(long startTime, long endTime)
{
    if (NumDatapoints < 2) return -1; // Data wasn't loaded or not enough datapoints

    int startDatapointIndex;
    int endDatapointIndex;
    // Find starting datapoint, binary search
    int lowerBound = 0;
    int upperBound = NumDatapoints;
    int prevIndex  = 0;
    while (1)
    {
        int index = (upperBound + lowerBound) / 2;
        if (SensorDataList[index].time == startTime)
        {
            // Start datapoint found
            startDatapointIndex = index;
            break;
        }

        if (prevIndex == index)
        {
            // Exact time match not found
            if (SensorDataList[index].time > startTime)
            {
                if (index == 0) return -1; // Requested start time is before first datapoint
                
                // Start with previous datapoint's power value
                startDatapointIndex = index - 1;
                break;
            }
            else
            {
                // Start with this datapoint's power value
                startDatapointIndex = index;
                break;
            }
        }
        else
        {
            // Continue searching
            if (SensorDataList[index].time > startTime)
            {
                upperBound = index;
            }
            else
            {
                lowerBound = index;
            }
        }
        prevIndex = index;
    }

    // Repeat process for end datapoint
    lowerBound = startDatapointIndex;
    upperBound = NumDatapoints;
    prevIndex = 0;
    while (1)
    {
        int index = (upperBound + lowerBound) / 2;
        if (SensorDataList[index].time == endTime)
        {
            // End datapoint found
            endDatapointIndex = index;
            break;
        }

        if (prevIndex == index)
        {
            // Exact time match not found
            if (SensorDataList[index].time > endTime)
            {
                if (index == 0) return -1; // Requested end time is before start time
                
                // Start with previous datapoint's power value
                endDatapointIndex = index - 1;
                break;
            }
            else
            {
                // Start with this datapoint's power value
                endDatapointIndex = index;
                break;
            }
        }
        else
        {
            // Continue searching
            if (SensorDataList[index].time > endTime)
            {
                upperBound = index;
            }
            else
            {
                lowerBound = index;
            }
        }
        prevIndex = index;
    }

    // Calculate the final energy sum
    double energySum = 0;
    for (int index = startDatapointIndex; index < endDatapointIndex; ++index)
    {
        double thisPower = SensorDataList[index].power;
        long thisTime = SensorDataList[index].time;

        if ((index == startDatapointIndex) && (thisTime != startTime))
        {
            // Edge case if startTime is between 2 datapoints
            energySum += thisPower * (SensorDataList[index + 1].time - startTime);
        }
        else if ((index == endDatapointIndex - 1) && (SensorDataList[endDatapointIndex].time != endTime))
        {
            // Edge case if endTime is between 2 datapoints
            energySum += thisPower * (SensorDataList[index + 1].time - thisTime); // Add the normal energy between last 2 datapoints first
            energySum += SensorDataList[endDatapointIndex].power * (endTime - SensorDataList[endDatapointIndex].time); // Add remainder
        }
        else
        {
            energySum += (thisPower + SensorDataList[index+1].power) / 2 * (SensorDataList[index + 1].time - thisTime);
        }
    }

    // Convert to correct units
    double energySumKwh = energySum / SECONDS_PER_HOUR / W_PER_KW;
    return energySumKwh;
}

// Parses user input calls Driver_SensorDataGetEnergyKwh() and returns the same value.
double Driver_SensorDataGetEnergyKwhFromString(char * inputString, int size)
{
    long startTime;
    long endTime;

    // Remove HTTP header or any other leading data if present
    char * inputData = strstr(inputString, "starttime=");
    if (inputData == NULL)
    {
        // Improperly formatted input
        return -1;
    }

    if (sscanf(inputData, "starttime=%ld&endtime=%ld", &startTime, &endTime) == 0)
    {
        // Improperly formatted input
        return -1;
    }
    else
    {
        return Driver_SensorDataGetEnergyKwh(startTime, endTime);
    }
}

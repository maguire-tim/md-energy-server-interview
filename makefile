energy-server: main.c DriverSocket.c DriverSensorData.c
	gcc -o energy-server main.c DriverSocket.c DriverSensorData.c -I.
//
// maple-reset.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "windows.h"
#include <conio.h>
#include <SetupAPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tchar.h"
#include <ctime>

bool findDfuDevice()
{
	unsigned index;
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	TCHAR HardwareID[1024];

	// Get all the connected USB devices
	hDevInfo = SetupDiGetClassDevs(NULL, TEXT("USB"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);

	// Iterate over the devices looking for the one we care about
	for (index = 0; ; index++) {
		DeviceInfoData.cbSize = sizeof(DeviceInfoData);
		if (!SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData)) {
			return false;
		}

		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (BYTE*)HardwareID, sizeof(HardwareID), NULL);
		if (_tcsstr(HardwareID, _T("VID_1EAF&PID_0003"))) {
			// Found it
			return true;
		}
	}
}

int main(int argc, char* argv[])
{
	HANDLE hComm;
	
	printf("maple-reset 0.5\n");
	printf("This program is Free Sofware and has NO WARRANTY\n\n");
	printf("https://github.com/benlye/maple-reset\n\n");

	if (argc < 2 || argc > 3)
	{
		fprintf(stdout, "Usage: maple-reset.exe [serial port] [optional timeout in milliseconds]\n\n");
		fprintf(stdout, "The optional timeout sets the maximum number of milliseconds the process will wait for a DFU device.\n");
		fprintf(stdout, " - If a DFU device appears before the timeout expires the process will return successfully immediately.\n");
		fprintf(stdout, " - If no timeout is specified the process will return successfully immediately after sending the reset pulse.\n");
		fprintf(stdout, " - If a timeout is specified and a DFU device does not appear before it elapses, the process will return a failure.\n");
		fprintf(stdout, "\nExample: maple-reset.exe COM3\n");
		fprintf(stdout, "         Resets the device on COM3 and returns immediately\n");
		fprintf(stdout, "\nExample: maple-reset.exe COM3 5000\n");
		fprintf(stdout, "         Resets the device on COM3 then waits up to 5s for the DFU device before returning\n");
		return -1;
	}

	std::string comPort = "\\\\.\\";
	comPort += argv[1];

	hComm = CreateFileA(comPort.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		NULL,
		0);

	if (hComm == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "ERROR: Failed to open %s\n", argv[1]);
		return -1;
	}

	char bytes_to_send[4];
	bytes_to_send[0] = 0x31;	// 1
	bytes_to_send[1] = 0x45;	// E
	bytes_to_send[2] = 0x41;	// A
	bytes_to_send[3] = 0x46;	// F

	DWORD bytes_written, total_bytes_written = 0;

	if (EscapeCommFunction(hComm, SETDTR) == 0) {
		fprintf(stdout, "ERROR: Failed to set DTR\n");
		return 1;
	}
	
	if (EscapeCommFunction(hComm, CLRDTR) == 0) {
		fprintf(stdout, "ERROR: Failed to clear DTR\n");
		return 2;
	}

	if (!WriteFile(hComm, bytes_to_send, 4, &bytes_written, NULL))
	{
		fprintf(stdout, "ERROR: Failed to send string\n");
		CloseHandle(hComm);
		return 3;
	}

	fprintf(stdout, "Reset sequence sent to %s\n", argv[1]);

	// Get the timeout from the arguments or default to 2s
	if (argc == 3 and atol(argv[2]) > 0)
	{
		fprintf(stdout, "Waiting for DFU device ...");

		// Get the start time of the wait loop
		std::clock_t start;
		double duration = 0;
		start = std::clock();

		// Loop until the DFU device is found or the timeout expires
		while (findDfuDevice() == false && duration <= atol(argv[2]))
		{
			duration = ((double)std::clock() - (double)start);
			fprintf(stdout, ".");
			Sleep(50);
		}

		if (findDfuDevice() == true)
		{
			printf(" got it.\n\n");
			printf("Device reset successful in %.0fms.\n", duration);
		}
		else
		{
			printf(" failed.\n\n");
			printf("ERROR: Device reset timed out.\n");
			return 4;
		}
	}

	return 0;
}

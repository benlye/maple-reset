//
// maple-reset.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "windows.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char* argv[])
{
	HANDLE hComm;
	
	printf("maple-reset 0.3\n");
	printf("This program is Free Sofware and has NO WARRANTY\n\n");
	printf("https://github.com/benlye/maple-reset\n\n");

	if (argc == 1) 
	{
		fprintf(stderr, "ERROR: Port not specified\n");
		fprintf(stderr, "\nUsage: maple-reset.exe COM3\n");
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
		fprintf(stderr, "ERROR: Failed to open %s\n", argv[1]);
		return -1;
	}

	char bytes_to_send[4];
	bytes_to_send[0] = 0x31;	// 1
	bytes_to_send[1] = 0x45;	// E
	bytes_to_send[2] = 0x41;	// A
	bytes_to_send[3] = 0x46;	// F

	DWORD bytes_written, total_bytes_written = 0;

	if (EscapeCommFunction(hComm, SETDTR) == 0) {
		fprintf(stderr, "ERROR: Failed to set DTR\n");
		return 1;
	}
	
	if (EscapeCommFunction(hComm, CLRDTR) == 0) {
		fprintf(stderr, "ERROR: Failed to clear DTR\n");
		return 2;
	}

	if (!WriteFile(hComm, bytes_to_send, 4, &bytes_written, NULL))
	{
		fprintf(stderr, "ERROR: Failed to send string.\n");
		CloseHandle(hComm);
		return 3;
	}

	fprintf(stdout, "Reset sequence sent to %s.\n", argv[1]);

	return 0;
}

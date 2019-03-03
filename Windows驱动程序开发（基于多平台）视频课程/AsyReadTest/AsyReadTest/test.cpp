#include <windows.h>
#include <stdio.h>

DWORD WINAPI ThreadProc(PVOID Context)
{
	DWORD dwret;
	HANDLE hDevice = *(PHANDLE)Context;
	OVERLAPPED ol = { NULL };
	ol.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	ReadFile(hDevice, NULL, 0, &dwret, &ol);

	WaitForSingleObject(hDevice, INFINITE);

	return 0;
}

int main()
{
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("打开设备失败%d\n", GetLastError());
		return -1;
	}

	
	for (int i = 0; i < 100;i++)
	{
		CreateThread(NULL, 0, ThreadProc, &hDevice, 0, NULL);
	}

	while (TRUE)
	{
		Sleep(2000);
	}

	CloseHandle(hDevice);


	return 0;
}
#include <windows.h>
#include <stdio.h>

#define IOCTL_SEND_EVENT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)

DWORD WINAPI ThreadProc(PVOID Context)
{
	HANDLE hEvent = *(PHANDLE)Context;

	printf("进入子线程\n");
	WaitForSingleObject(hEvent, INFINITE);
	printf("离开子线程\n"); 
	return 0;

}

int main()
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEvent == NULL)
	{
		return -1;
	}

	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("设备打开失败\n");
		return -2;
	}

	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, &hEvent, 0, NULL);
	DWORD dwret;
	DeviceIoControl(hDevice, IOCTL_SEND_EVENT, &hEvent, sizeof(HANDLE), NULL, 0, &dwret, NULL);

	WaitForSingleObject(hThread, INFINITE);
	printf("主线程结束\n");
	CloseHandle(hDevice);
	CloseHandle(hEvent);

	return 0;
}
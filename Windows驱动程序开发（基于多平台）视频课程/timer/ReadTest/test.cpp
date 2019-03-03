#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("设备打开失败\n");
		return -1;
	}
	OVERLAPPED ol = { NULL };
	DWORD dwRet;
	ol.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ReadFile(hDevice, NULL, 0, &dwRet, &ol);

	WaitForSingleObject(ol.hEvent, INFINITE);
	CloseHandle(hDevice);

	return 0;
}
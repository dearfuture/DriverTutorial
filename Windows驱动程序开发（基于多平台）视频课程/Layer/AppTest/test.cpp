#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("设备打开失败%d\n", GetLastError());
		return -1;
	}
	DWORD dwRet;
	ReadFile(hDevice, NULL, 0, &dwRet, NULL);
	CloseHandle(hDevice);
	return 0;
}
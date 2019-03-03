#include <windows.h>
#include <stdio.h>
#include <winioctl.h>

#define IOTEST CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTLBUFFERED CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTLDIRECT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)
#define CTLNEITHER	CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_NEITHER,FILE_ANY_ACCESS)
int main()
{
	
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("设备打开失败%d\n",GetLastError());
		return -1;
	}
	DWORD dwRet;
	CHAR Buffer[1024] = { 0 };
	WriteFile(hDevice, "这是一段来自用户层的数据", strlen("这是一段来自用户层的数据"), &dwRet, NULL);
	printf("%p\n", Buffer);
	ReadFile(hDevice, Buffer, sizeof(Buffer), &dwRet, NULL);
	printf("读回的数据%s\n", Buffer);
	printf("读取数据的个数%d\n", dwRet);
	
	DWORD dwSize = GetFileSize(hDevice, NULL);
	printf("文件长度%d\n", dwSize);
	char IoBuffer[1024] = { 0 };
	DeviceIoControl(hDevice, CTLBUFFERED, "这是缓冲区方式", strlen("这是缓冲区方式") + 1, IoBuffer, sizeof(IoBuffer), &dwRet, NULL);
	printf("%s\n", IoBuffer);

	RtlZeroMemory(IoBuffer, sizeof(Buffer));
	DeviceIoControl(hDevice, CTLDIRECT, "这是直接区方式", strlen("这是直接区方式") + 1, IoBuffer, sizeof(IoBuffer), &dwRet, NULL);
	printf("%s\n", IoBuffer);

	RtlZeroMemory(IoBuffer, sizeof(Buffer));
	DeviceIoControl(hDevice, CTLNEITHER, "这是其它区方式", strlen("这是其它区方式") + 1, IoBuffer, sizeof(IoBuffer), &dwRet, NULL);
	printf("%s\n", IoBuffer);

	CloseHandle(hDevice);

	return 0;
}
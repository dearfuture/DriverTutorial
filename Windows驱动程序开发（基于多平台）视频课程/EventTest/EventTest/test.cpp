#include <windows.h>
#include <stdio.h>

DWORD WINAPI Threadproc(PVOID Context)
{
	PHANDLE phEvent = (PHANDLE)Context;
	printf("进入到线程函数里\n");
	SetEvent(*phEvent);
	printf("线程函数结束\n");

	return 0;
}

int main()
{
	HANDLE hEvent=CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == hEvent)
	{
		printf("创建事件失败%d\n", GetLastError());
		return -1;
	}

	CreateThread(NULL, 0, Threadproc, &hEvent, 0, NULL);

	WaitForSingleObject(hEvent, INFINITE);
	printf("进程结束\n");
	return 0;
}
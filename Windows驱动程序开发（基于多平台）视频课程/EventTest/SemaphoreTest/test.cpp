#include <windows.h>
#include <stdio.h>

DWORD WINAPI ThreadProc(PVOID Context)
{
	PHANDLE phSemaphore = (PHANDLE)Context;
	printf("进入线程函数\n");
	Sleep(5000);
	ReleaseSemaphore(*phSemaphore, 1, NULL);

	printf("离开线程函数\n");
	return 0;


}

int main()
{
	HANDLE hSemaphore = CreateSemaphore(NULL, 2, 2, NULL);
	if (NULL == hSemaphore)
	{
		printf("信号量创建失败%d\n", GetLastError());
		return -1;
	}

	WaitForSingleObject(hSemaphore, INFINITE);
	WaitForSingleObject(hSemaphore, INFINITE);

	CreateThread(NULL, 0, ThreadProc, &hSemaphore, 0, NULL);

	WaitForSingleObject(hSemaphore, INFINITE);
	printf("主线程结束\n");
	return 0;
}
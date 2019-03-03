#include <windows.h>
#include <stdio.h>


DWORD WINAPI ThreadProc1(PVOID Context)
{
	PHANDLE phMutex = (PHANDLE)Context;
	WaitForSingleObject(*phMutex, INFINITE);
	printf("进入线程1\n");
	Sleep(2000);
	printf("离开线程1\n"); 
	ReleaseMutex(*phMutex);
	return 0;
}

DWORD WINAPI ThreadProc2(PVOID Context)
{
	PHANDLE phMutex = (PHANDLE)Context;
	WaitForSingleObject(*phMutex, INFINITE);
	printf("进入线程2\n");
	Sleep(2000);
	printf("离开线程2\n");
	ReleaseMutex(*phMutex);
	return 0;
}
int main()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
	{
		printf("创建事件失败%d\n",GetLastError());
		return -1;
	}
	HANDLE hThread[2] = { NULL };
	hThread[0] = CreateThread(NULL, 0, ThreadProc1, &hMutex, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ThreadProc2, &hMutex, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	printf("结束主线程\n");
	return 0;
}
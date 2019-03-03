#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("驱动卸载\n"));
}

VOID ThreadProc1(PVOID Context)
{
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);
	PKMUTEX pMutex = (PKMUTEX)Context;
	KeWaitForMutexObject(pMutex, Executive, KernelMode, FALSE, NULL);
	KdPrint(("进入线程1\n"));

	KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	KdPrint(("离开线程1\n"));
	KeReleaseMutex(pMutex, FALSE);
	
	PsTerminateSystemThread(0);

}

VOID ThreadProc2(PVOID Context)
{

	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);
	PKMUTEX pMutex = (PKMUTEX)Context;
	KeWaitForMutexObject(pMutex, Executive, KernelMode, FALSE, NULL);
	KdPrint(("进入线程2\n"));

	KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	KdPrint(("离开线程2\n"));
	KeReleaseMutex(pMutex, FALSE);

	PsTerminateSystemThread(0);

}

VOID MutexTest()
{
	KMUTEX Mutex;
	HANDLE hThread[2] = { NULL };
	PETHREAD Thread[2] = { NULL };
	KeInitializeMutex(&Mutex, 0);

	PsCreateSystemThread(&hThread[0], THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc1, &Mutex);
	PsCreateSystemThread(&hThread[1], THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc2, &Mutex);

	ObReferenceObjectByHandle(hThread[0], THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &Thread[0], NULL);
	ObReferenceObjectByHandle(hThread[1], THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &Thread[1], NULL);

	KeWaitForMultipleObjects(2, Thread, WaitAll, Executive, KernelMode, FALSE, NULL, NULL);


	ObDereferenceObject(Thread[0]);
	ObDereferenceObject(Thread[1]);
	KdPrint(("主线程结束\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	MutexTest();
	return STATUS_SUCCESS;
}
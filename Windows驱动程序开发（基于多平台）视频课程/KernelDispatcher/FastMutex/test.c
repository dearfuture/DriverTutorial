#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("驱动卸载\n"));
}

VOID ThreadProc1(PVOID Context)
{
	PFAST_MUTEX pFastMutex = (PFAST_MUTEX)Context;

	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);

	ExAcquireFastMutex(pFastMutex);
	KdPrint(("进入线程1\n"));
	KeDelayExecutionThread(KernelMode, FALSE, &timeout);


	KdPrint(("离开线程1\n"));
	ExReleaseFastMutex(pFastMutex);
	
	PsTerminateSystemThread(0);
}

VOID ThreadProc2(PVOID Context)
{
	PFAST_MUTEX pFastMutex = (PFAST_MUTEX)Context;

	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);

	ExAcquireFastMutex(pFastMutex);
	KdPrint(("进入线程2\n"));
	KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	KdPrint(("离开线程2\n"));
	ExReleaseFastMutex(pFastMutex);
	
	PsTerminateSystemThread(0);

}

VOID FastMutexTest()
{
	FAST_MUTEX FastMutex;
	HANDLE hThread1, hThread2;
	PETHREAD Thread[2] = { NULL };
	ExInitializeFastMutex(&FastMutex);
	PsCreateSystemThread(&hThread1, THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc1, &FastMutex);
	PsCreateSystemThread(&hThread2, THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc2, &FastMutex);
	ObReferenceObjectByHandle(hThread1, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &Thread[0], NULL);
	ObReferenceObjectByHandle(hThread2, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &Thread[1], NULL);

	KeWaitForMultipleObjects(2, Thread, WaitAll, Executive, KernelMode, FALSE, NULL, NULL);
	KdPrint(("主线程终止\n"));

	ObDereferenceObject(Thread[0]);
	ObDereferenceObject(Thread[1]); 
	
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	LONG Value = 10;
	KSPIN_LOCK spinLock;
	LONGLONG value1 = 1, value2 = 2, value3 = 1;
	LARGE_INTEGER liValue = RtlConvertLongToLargeInteger(10);
	LARGE_INTEGER Inc = RtlConvertLongToLargeInteger(90);
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	//FastMutexTest();

	InterlockedCompareExchange(&Value, 100, 10);
	KdPrint(("Value:%d\n", Value));

	InterlockedDecrement(&Value);
	KdPrint(("%d\n", Value));

	InterlockedExchange(&Value, 88);
	KdPrint(("%d\n", Value));

	InterlockedExchangeAdd(&Value, 1);
	KdPrint(("%d\n", Value));

	InterlockedIncrement(&Value);
	KdPrint(("%d\n", Value));


	KeInitializeSpinLock(&spinLock);
	ExInterlockedAddLargeInteger(&liValue, Inc, &spinLock);
	KdPrint(("%d\n", liValue.LowPart));

	ExInterlockedAddLargeStatistic(&liValue, 9);
	KdPrint(("%d\n", liValue.LowPart));

	ExInterlockedAddUlong(&Value, 10, &spinLock);
	KdPrint(("%d\n", Value));

	ExInterlockedCompareExchange64(&value1, &value2, &value3, &spinLock);
	KdPrint(("%d\n", value1));
	return STATUS_SUCCESS;
}
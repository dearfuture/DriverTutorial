#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("驱动卸载\n"));
}

VOID ThreadProc(IN PVOID Context)
{
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	while (TRUE)
	{
		KdPrint(("驱动正在运行...\n"));
		KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	}
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	HANDLE hThread;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc, NULL);
	return STATUS_SUCCESS;
}
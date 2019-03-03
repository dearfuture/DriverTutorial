#include <ntifs.h>
#include <ntddk.h>

PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);
HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
PPEB PsGetProcessPeb(PEPROCESS Process);
NTSTATUS PsSuspendProcess(PEPROCESS Process);
NTSTATUS PsResumeProcess(PEPROCESS Process);

typedef NTSTATUS (*pfnPsSuspendProcess)(PEPROCESS Process);
typedef NTSTATUS(*pfnPsResumeProcess)(PEPROCESS Process);

pfnPsSuspendProcess xppfnPsSuspendProcess = (pfnPsSuspendProcess)0x805d585e;
pfnPsResumeProcess xppfnPsResumeProcess = (pfnPsResumeProcess)0x805d5710;

VOID ResumeProcess(HANDLE ProcessId);

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//ResumeProcess((HANDLE)1036);
	KdPrint(("驱动卸载\n"));
}

VOID EnumThread(PEPROCESS Process)
{
	PETHREAD Thread =  NULL;
	NTSTATUS status;
	for (int i = 4; i < 100000;i+=4)
	{
		status  = PsLookupThreadByThreadId((HANDLE)i, &Thread);
		if (NT_SUCCESS(status))
		{
			if (IoThreadToProcess(Thread) == Process)
			{
				KdPrint(("%s\t%d:%d\n", PsGetProcessImageFileName(Process), PsGetProcessId(Process), i));
			}
			
		}


	}




}


VOID EnumProcess()
{
	ULONG i = 0;
	PEPROCESS Process;
	NTSTATUS status;
	for (i = 4; i < 20000; i += 4)
	{
		status = PsLookupProcessByProcessId((HANDLE)i, &Process);
		if (NT_SUCCESS(status))
		{
			KdPrint(("parentid:%d\t%d\t%s\n", PsGetProcessInheritedFromUniqueProcessId(Process), i, PsGetProcessImageFileName(Process)));
			EnumThread(Process);
		
		}
	}

}

VOID SuspendProcess(HANDLE ProcessId)
{
	PEPROCESS Process;
	NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &Process);
	if (NT_SUCCESS(status))
	{
		if (SharedUserData->NtMajorVersion == 5)
		{
			xppfnPsSuspendProcess(Process);
		}
// 		else
// 		{
// 			PsSuspendProcess(Process);
// 		}
		
	}


}

VOID ResumeProcess(HANDLE ProcessId)
{
	PEPROCESS Process;
	NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &Process);
	if (NT_SUCCESS(status))
	{
		if (SharedUserData->NtMajorVersion == 5)
		{
			xppfnPsResumeProcess(Process);
		}
// 		else
// 		{
// 			PsResumeProcess(Process);
// 		}
		
	}


}


VOID MyTerminateProcess(HANDLE ProcessId)
{
	CLIENT_ID Client = { 0 };
	HANDLE hProcess = NULL;
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;

	Client.UniqueProcess = ProcessId;
	InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &Client);
	if (NT_SUCCESS(status))
	{
		status = ZwTerminateProcess(hProcess, 0);
		if (NT_SUCCESS(status))
		{
			KdPrint(("进程终止\n"));
		}
	}
	

}



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	EnumProcess();
	//SuspendProcess((HANDLE)1036);
	MyTerminateProcess((HANDLE)1608);
	return STATUS_SUCCESS;
}

#include <ntifs.h>
#include <ntddk.h>

PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);
HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
PPEB PsGetProcessPeb(PEPROCESS Process);
NTSTATUS PsSuspendProcess(PEPROCESS Process);
NTSTATUS PsResumeProcess(PEPROCESS Process);


VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));
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
			KdPrint(("%d\t%s\n", i, PsGetProcessImageFileName(Process)));
		}
	}

}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
	DriverObject->DriverUnload = Unload;

	EnumProcess();

	return STATUS_SUCCESS;
}

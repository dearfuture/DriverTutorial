#include <ntifs.h>
#include <ntddk.h>

VOID
ProcessNotify(
IN HANDLE  ParentId,
IN HANDLE  ProcessId,
IN BOOLEAN  Create
);

VOID CreateProcessNotifyEx(
	_Inout_  PEPROCESS              Process,
	_In_     HANDLE                 ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	);

PCHAR PsGetProcessImageFileName(PEPROCESS Process);
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	PsSetCreateProcessNotifyRoutine(ProcessNotify, TRUE);
	PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, TRUE);
}


VOID
ProcessNotify(
IN HANDLE  ParentId,
IN HANDLE  ProcessId,
IN BOOLEAN  Create
)
{
	PEPROCESS Process;
	PsLookupProcessByProcessId(ProcessId, &Process);
	if (Create)
	{
		
		KdPrint(("有进程创建%d:%s:%d\n",ParentId, PsGetProcessImageFileName(Process),ProcessId));
		
	}
	else
	{
		KdPrint(("有进程退出%s:%d\n",PsGetProcessImageFileName(Process),ProcessId));
	}

}

VOID CreateProcessNotifyEx(
	_Inout_  PEPROCESS              Process,
	_In_     HANDLE                 ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	)
{
	if (CreateInfo!=NULL)
	{
		KdPrint(("创建ppid:%d pid=%d,%wZ\n", CreateInfo->ParentProcessId, ProcessId, CreateInfo->ImageFileName));
		if (_stricmp(PsGetProcessImageFileName(Process), "calc.exe") == 0)
		{
			CreateInfo->CreationStatus = STATUS_UNSUCCESSFUL;
		}
	}
	else
	{
		KdPrint(("退出pid=%d,%s\n",ProcessId,PsGetProcessImageFileName(Process)));
	}


}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PsSetCreateProcessNotifyRoutine(ProcessNotify, FALSE);
	status = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, FALSE);
	if (NT_SUCCESS(status))
	{
		KdPrint(("注册成功\n"));
	}
	else
	{
		KdPrint(("注册失败%x\n", status));
	}
	DriverObject->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
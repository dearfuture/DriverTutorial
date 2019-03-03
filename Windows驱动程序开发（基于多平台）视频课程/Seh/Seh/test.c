#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("驱动卸载\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	PCHAR Buffer = NULL;
	LONG f1, f2, f3;
	DriverObject->DriverUnload = Unload;
	KdPrint(("驱动加载\n"));

// 	__try
// 	{
// 		//KdPrint(("%d\n", *Buffer));
// 		//ProbeForRead(Buffer, 4, 4);
// 		//ProbeForWrite(Buffer, 4, 4);
// 		//ExRaiseStatus(STATUS_ACCESS_VIOLATION);
// 		//ExRaiseStatus(STATUS_DATATYPE_MISALIGNMENT);
// 		//ExRaiseAccessViolation();
// 		ExRaiseDatatypeMisalignment();
// 	}
// 	__except (EXCEPTION_EXECUTE_HANDLER)
// 	{
// 		KdPrint(("异常码%x\n", GetExceptionCode()));
// 	}
	f1 = 0;
	f2 = 1;
	f3 = 2;
	__try
	{
		if (f1 == 0)
		{
			KdPrint(("在f1\n"));
			__leave;
		}

		if (f2 == 1)
		{
			KdPrint(("在f2\n"));
			__leave;
		}

		if (f3 == 2)
		{
			KdPrint(("在f3\n"));
			__leave;
		}
	
	}
	__finally
	{
		KdPrint(("在finally\n"));
	}

	KdPrint(("DriverEntry结束\n"));
	
	return STATUS_SUCCESS;
}
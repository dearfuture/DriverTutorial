#include <ntddk.h>

HANDLE hCallback;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	if (hCallback != NULL)
	{
		ExUnregisterCallback(hCallback);
	}
	KdPrint(("驱动卸载\n"));
}

//DISPATCH_LEVEL
VOID
callbackroutine(
IN PVOID CallbackContext,
IN PVOID Argument1,
IN PVOID Argument2
)
{
	KdPrint(("%d:时间发生了修改\n",KeGetCurrentIrql()));
	

}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING CbName = RTL_CONSTANT_STRING(L"\\Callback\\SetSystemTime");
	OBJECT_ATTRIBUTES oa;
	NTSTATUS status;
	PCALLBACK_OBJECT CallBack;

	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	InitializeObjectAttributes(&oa, &CbName, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ExCreateCallback(&CallBack, &oa, TRUE, FALSE);
	if (NT_SUCCESS(status))
	{
		KdPrint(("成功\n"));
		hCallback = ExRegisterCallback(CallBack, callbackroutine, NULL);
		ObDereferenceObject(CallBack);
	}
	else
	{
		KdPrint(("失败\n"));
	}

	return STATUS_SUCCESS;
}
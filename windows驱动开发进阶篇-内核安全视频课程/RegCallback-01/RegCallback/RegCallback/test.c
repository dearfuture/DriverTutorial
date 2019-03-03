#include <ntddk.h>

LARGE_INTEGER Cookie;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	CmUnRegisterCallback(Cookie);
	KdPrint(("驱动卸载\n"));
}

NTSTATUS RegistryCallback(
	_In_      PVOID CallbackContext,
	_In_opt_  PVOID Argument1,
	_In_opt_  PVOID Argument2
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	KdPrint(("有注册表的操作\n"));

	switch ((REG_NOTIFY_CLASS)Argument1)
	{
	case RegNtPreOpenKey:
	case RegNtPreOpenKeyEx:
	case RegNtPreCreateKey:
	case RegNtPreCreateKeyEx:
		break;
	default:
		break;
	}


	return status;

}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	status = CmRegisterCallback(RegistryCallback, NULL, &Cookie);
	if (NT_SUCCESS(status))
	{
		KdPrint(("注册成功\n"));
		return status;
	}

	return status;
}
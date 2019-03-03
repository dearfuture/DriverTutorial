#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("驱动卸载\n"));
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	LARGE_INTEGER systemtime;
	LARGE_INTEGER localtime;
	TIME_FIELDS timefields;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	KeQuerySystemTime(&systemtime);
	ExSystemTimeToLocalTime(&systemtime, &localtime);
	RtlTimeToTimeFields(&localtime, &timefields);

	KdPrint(("%d年%d月%d日 %d:%d:%d.%.3d\n", 
		timefields.Year, 
		timefields.Month, 
		timefields.Day, 
		timefields.Hour, 
		timefields.Minute, 
		timefields.Second,
		timefields.Milliseconds));


	return STATUS_SUCCESS;
}
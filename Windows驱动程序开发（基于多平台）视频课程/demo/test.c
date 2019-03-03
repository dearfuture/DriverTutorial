#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));
	
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
	DriverObject->DriverUnload=Unload;
	
	return STATUS_SUCCESS;
}
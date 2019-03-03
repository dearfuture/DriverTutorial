#include <ntddk.h>

#define MUL(A,B)  ((A)*(B))

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("«˝∂Ø–∂‘ÿ\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	
	KdPrint(("«˝∂Øº”‘ÿ\n"));
	KdPrint(("≤‚ ‘\n"));
	KdPrint(("%wZ\n", &us));

	//ASSERT(FALSE);
	
	return STATUS_SUCCESS; 
}
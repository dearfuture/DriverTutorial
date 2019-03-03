#include <ntifs.h>
#include <ntddk.h>

NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
IN PUNICODE_STRING ObjectName,
IN ULONG Attributes,
IN PACCESS_STATE PassedAccessState OPTIONAL,
IN ACCESS_MASK DesiredAccess OPTIONAL,
IN POBJECT_TYPE ObjectType,
IN KPROCESSOR_MODE AccessMode,
IN OUT PVOID ParseContext OPTIONAL,
OUT PVOID *Object
);


extern POBJECT_TYPE *IoDriverObjectType;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{



}

VOID GetDeviceInfo(PDEVICE_OBJECT DeviceObject)
{
	PDEVICE_OBJECT CurrentDevice = DeviceObject;
	POBJECT_NAME_INFORMATION pNameInfo = (POBJECT_NAME_INFORMATION)ExAllocatePool(PagedPool, 1024);
	ULONG Length;
	NTSTATUS status;
	while (CurrentDevice != NULL)
	{
		RtlZeroMemory(pNameInfo, 1024);
		status = ObQueryNameString(CurrentDevice, pNameInfo, 1024, &Length);
		if (NT_SUCCESS(status))
		{
			KdPrint(("驱动对象%p:%wZ\t设备对象%p:%wZ\n",
				CurrentDevice->DriverObject,
				&CurrentDevice->DriverObject->DriverName,
				CurrentDevice, 
				&pNameInfo->Name));
		}

		CurrentDevice = CurrentDevice->AttachedDevice;
	}

	ExFreePool(pNameInfo);
}

VOID EnumDriver()
{
	UNICODE_STRING DriverName = RTL_CONSTANT_STRING(L"\\Driver\\ACPI");
	NTSTATUS status;
	PDRIVER_OBJECT DriverObject;
	PDEVICE_OBJECT CurrentDevice;


	status = ObReferenceObjectByName(&DriverName, OBJ_CASE_INSENSITIVE,
		NULL, FILE_ANY_ACCESS, *IoDriverObjectType, KernelMode, NULL, &DriverObject);
	if (NT_SUCCESS(status))
	{
		CurrentDevice = DriverObject->DeviceObject;
		while (CurrentDevice != NULL)
		{
			GetDeviceInfo(CurrentDevice);
			CurrentDevice = CurrentDevice->NextDevice;
		}
	}

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{

	DriverObject->DriverUnload = Unload;

	EnumDriver();

	return STATUS_SUCCESS;
}
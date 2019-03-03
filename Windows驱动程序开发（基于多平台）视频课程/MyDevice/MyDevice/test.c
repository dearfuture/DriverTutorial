#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\Global??\\HelloDDK");
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);
	KdPrint(("驱动卸载\n"));

}

VOID EnumDriver(PDRIVER_OBJECT DriverObjet)
{

	KdPrint(("Driver:%p\n", DriverObjet));
	KdPrint(("Device:%p\n", DriverObjet->DeviceObject));
	KdPrint(("驱动对象名:%wZ\n", &DriverObjet->DriverName));
	KdPrint(("服务名:%wZ\n", &DriverObjet->DriverExtension->ServiceKeyName));
	KdPrint(("%wZ\n", DriverObjet->HardwareDatabase));
	

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备创建失败\n"));
		return status;
	}
	KdPrint(("设备创建成功\n"));
	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建符号链接失败%x\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	KdPrint(("符号链接创建成功\n"));
	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	EnumDriver(DriverObject);




	return STATUS_SUCCESS;
}


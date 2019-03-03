#include <ntddk.h>

KDPC dpc;
KTIMER timer;

LARGE_INTEGER timeout;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");

	//IoStopTimer(DriverObject->DeviceObject);
	KeCancelTimer(&timer);
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymbolicLinkName);
	KdPrint(("驱动卸载\n"));

}
void IoTimerRoutine(
	PDEVICE_OBJECT DeviceObject,
	PVOID Context
	)
{
	KdPrint(("io定时器\n"));
}



VOID DpcRoutine(
	_In_     struct _KDPC *Dpc,
	_In_opt_ PVOID        DeferredContext,
	_In_opt_ PVOID        SystemArgument1,
	_In_opt_ PVOID        SystemArgument2
	)
{
	PIRP Irp = (PIRP)DeferredContext;
	KdPrint(("进入DriverA:dpc定时函数\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("离开DriverA:dpc定时器函数\n"));
	//KeSetTimer(&timer, timeout, &dpc);
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("进入DriverA:DispatchRoutine\n"));
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("离开DriverA:DispatchRoutine\n"));
	return STATUS_SUCCESS;
}


NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint((("进入DrvierA:Read\n")));
	timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);
	IoMarkIrpPending(Irp);


	KeInitializeTimer(&timer);
	KeInitializeDpc(&dpc, DpcRoutine, Irp);

	KeSetTimer(&timer, timeout, &dpc);
	KdPrint(("离开DriverA:Read\n"));
	return STATUS_PENDING;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	NTSTATUS status;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchRoutine;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchRoutine;
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备创建失败%d\n", status));
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("符号链接创建失败%d\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	// 	IoInitializeTimer(DeviceObject, IoTimerRoutine, NULL);
	// 	IoStartTimer(DeviceObject);


	return STATUS_SUCCESS;
}
#include <ntddk.h>


LIST_ENTRY ListHead;
KDEVICE_QUEUE DeviceQueue;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymbolicLinkName);

	KdPrint(("驱动卸载\n"));
}

NTSTATUS Create(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

VOID OnCancelIrp(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
	)
{
	KdPrint(("进入取消函数\n"));
	Irp->IoStatus.Status = STATUS_CANCELLED;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("离开取消函数\n"));

}

VOID MyStartIo(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIRP CurrentIrp = Irp;
	PKDEVICE_QUEUE_ENTRY QueueEntry = NULL;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 2000 * 1000);
	KdPrint(("进入MMyStartIo\n"));


	while (TRUE)
	{

		CurrentIrp->IoStatus.Status = STATUS_SUCCESS;
		CurrentIrp->IoStatus.Information = 0;
		IoCompleteRequest(CurrentIrp, IO_NO_INCREMENT);

		QueueEntry = KeRemoveDeviceQueue(&DeviceQueue);
		if (QueueEntry == NULL)
		{
			break;
		}
		
		CurrentIrp = CONTAINING_RECORD(QueueEntry, IRP, Tail.Overlay.DeviceQueueEntry);
		KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	}
	

	KdPrint(("离开MMyStartIo\n"));

}


NTSTATUS Read(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("进入read\n"));

	//IoSetCancelRoutine(Irp, OnCancelIrp);
	IoMarkIrpPending(Irp);

	if (!KeInsertDeviceQueue(&DeviceQueue, &Irp->Tail.Overlay.DeviceQueueEntry))
	{
		MyStartIo(DeviceObject, Irp);
	}
	

	//IoStartPacket(DeviceObject, Irp, NULL, OnCancelIrp);

	KdPrint(("离开读函数\n")); 
	return STATUS_PENDING;

}


NTSTATUS Cleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("进入cleanup\n"));
	while (!IsListEmpty(&ListHead))
	{

		PLIST_ENTRY pEntry = RemoveHeadList(&ListHead);
		PIRP pendingirp = CONTAINING_RECORD(pEntry, IRP, Tail.Overlay.ListEntry);

		pendingirp->IoStatus.Status = STATUS_SUCCESS;
		pendingirp->IoStatus.Information = 0;
		IoCompleteRequest(pendingirp, IO_NO_INCREMENT);
		KdPrint(("完成irp操作\n"));
	}
	

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("离开cleanup\n"));
	return STATUS_SUCCESS;

}

NTSTATUS Close(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("进入Close\n"));
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("离开close\n"));
	return STATUS_SUCCESS;

}


VOID StartIo(
	_Inout_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP           *Irp
	)
{
	KIRQL oldIrql;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 100 * 1000);
	
	IoAcquireCancelSpinLock(&oldIrql);
	if (DeviceObject->CurrentIrp != Irp)
	{

		IoReleaseCancelSpinLock(oldIrql);
		return;
	}
	IoReleaseCancelSpinLock(oldIrql);

	KdPrint(("进入startio"));
	KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	IoStartNextPacket(DeviceObject, TRUE);

	KdPrint(("离开startio\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	KdPrint(("驱动加载\n"));

	DriverObject->DriverUnload = Unload;
	DriverObject->DriverStartIo = StartIo;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	DriverObject->MajorFunction[IRP_MJ_READ] = Read;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = Cleanup;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建设备失败%x\n", status));
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("符号链接创建失败%x\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	InitializeListHead(&ListHead);
	KeInitializeDeviceQueue(&DeviceQueue);
	return STATUS_SUCCESS;
}
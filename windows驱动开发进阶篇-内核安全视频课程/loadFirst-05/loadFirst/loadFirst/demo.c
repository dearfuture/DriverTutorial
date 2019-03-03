#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{


	KdPrint(("驱动卸载\n"));
}

VOID ThreadProc(IN PVOID Context)
{
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	while (TRUE)
	{
		KdPrint(("驱动正在运行...\n"));
		KeDelayExecutionThread(KernelMode, FALSE, &timeout);
	}
}

VOID Reinitialize(
	_In_     struct _DRIVER_OBJECT *DriverObject,
	_In_opt_ PVOID                 Context,
	_In_     ULONG                 Count
	)
{
	KdPrint(("驱动重新初始化完成\n"));
	if (Count < 10)
	{
		IoRegisterDriverReinitialization(DriverObject, Reinitialize, NULL);
	}
	else
	{
		KdPrint(("重新初始化次数过多\n"));
	}

	


}


VOID BootReinitialize(
	_In_     struct _DRIVER_OBJECT *DriverObject,
	_In_opt_ PVOID                 Context,
	_In_     ULONG                 Count
	)
{
	KdBreakPoint();
	KdPrint(("boot类型重新初始化函数\n"));
//	IoRegisterBootDriverReinitialization(DriverObject, BootReinitialize, NULL);
}

NTSTATUS lfShutDown(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdBreakPoint();
	KdPrint(("关机通知\n"));

	//关机可以做的事,比如恢复文件，刷新磁盘文件等，修改注册表等
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	HANDLE hThread;
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status;
	PFILE_OBJECT FileObject;
	PDEVICE_OBJECT NullDevice = NULL;
	UNICODE_STRING	DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Null");
	KdPrint(("驱动加载\n"));

	DriverObject->DriverUnload = Unload;
	//PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadProc, NULL);

	DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = lfShutDown;
	status = IoGetDeviceObjectPointer(&DeviceName, FILE_ANY_ACCESS, &FileObject, &NullDevice);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("没有找到对应的设备\n"));
		return status;
	}
	ObDereferenceObject(FileObject);
	NullDevice->DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = lfShutDown;
	


// 	status = IoCreateDevice(DriverObject, 0, NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
// 	if (!NT_SUCCESS(status))
// 	{
// 		KdPrint(("设备创建失败%x\n",status));
// 	}
	IoRegisterShutdownNotification(NullDevice);
// 	IoRegisterDriverReinitialization(DriverObject, Reinitialize, NULL);
// 	IoRegisterBootDriverReinitialization(DriverObject, BootReinitialize, NULL);

	//IoRegisterShutdownNotification()

	return STATUS_SUCCESS;
}
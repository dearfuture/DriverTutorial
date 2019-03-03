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
extern POBJECT_TYPE *IoDeviceObjectType;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{



}
VOID CallDrivertest1()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwCreateFile(&hDevice, 
		GENERIC_ALL, 
		&oa, 
		&IoStatus, 
		NULL, 
		FILE_ATTRIBUTE_NORMAL, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN, 
		FILE_SYNCHRONOUS_IO_NONALERT, 
		NULL, 
		0
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备打开失败%x\n", status));
		return;
	}
	ZwReadFile(hDevice, NULL, NULL, NULL, &IoStatus, NULL, 0, NULL, NULL);
	KdPrint(("DriverB:读结束\n"));
	ZwClose(hDevice);
	
}


VOID
APCRoutine(
_In_ PVOID ApcContext,
_In_ PIO_STATUS_BLOCK IoStatusBlock,
_In_ ULONG Reserved
)
{
	PKEVENT pEvent = (PKEVENT)ApcContext;
	KeSetEvent(pEvent, IO_NO_INCREMENT, FALSE);




}

VOID CallDrivertest2()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	KEVENT Event;
	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);
	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwCreateFile(&hDevice,
		GENERIC_ALL,
		&oa,
		&IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		0,
		NULL,
		0
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备打开失败%x\n", status));
		return;
	}

	KeInitializeEvent(&Event, NotificationEvent, FALSE);
	
	status = ZwReadFile(hDevice, NULL, APCRoutine, &Event, &IoStatus, NULL, 0, &offset, NULL);

	if (NT_SUCCESS(status))
	{
		if (status == STATUS_PENDING)
		{
			KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		}
	}
	else
	{
		KdPrint(("%x\n", status));
	}
	

	KdPrint(("DriverB:读结束\n"));
	ZwClose(hDevice);

}


VOID CallDrivertest3()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	KEVENT Event;
	PDEVICE_OBJECT DeviceObject;
	PFILE_OBJECT FileObject;
	PIRP Irp;
	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);
	PIO_STACK_LOCATION stack;
	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	
	status = IoGetDeviceObjectPointer(&DeviceName, FILE_ANY_ACCESS, &FileObject, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("获取设备对象指针出错%x\n", status));
		return;
	}
	KdPrint(("设备对象:%p", DeviceObject));
	KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

	Irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, DeviceObject, NULL, 0, &offset, &Event, &IoStatus);

	stack = IoGetNextIrpStackLocation(Irp);
	stack->FileObject = FileObject;
	status = IoCallDriver(DeviceObject, Irp);
	if (status==STATUS_PENDING)
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		KdPrint(("完成读操作\n"));
	}

	ObDereferenceObject(FileObject);


}

VOID CallDrivertest4()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	KEVENT Event;
	PDEVICE_OBJECT DeviceObject;
	PFILE_OBJECT FileObject;
	PIRP Irp;
	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);
	PIO_STACK_LOCATION stack;
	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = IoGetDeviceObjectPointer(&DeviceName, FILE_ANY_ACCESS, &FileObject, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("获取设备对象指针出错%x\n", status));
		return;
	}
	KdPrint(("设备对象:%p", DeviceObject));
	KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

//	Irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, DeviceObject, NULL, 0, &offset, &Event, &IoStatus);
	Irp = IoBuildAsynchronousFsdRequest(IRP_MJ_READ, DeviceObject, NULL, 0, &offset, &IoStatus);
	Irp->UserEvent = &Event;
	stack = IoGetNextIrpStackLocation(Irp);
	stack->FileObject = FileObject;
	status = IoCallDriver(DeviceObject, Irp);

	if (status == STATUS_PENDING)
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		KdPrint(("完成读操作\n"));
	}

	ObDereferenceObject(FileObject);


}

VOID CallDrivertest5()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	KEVENT Event;
	PDEVICE_OBJECT DeviceObject;
	PFILE_OBJECT FileObject;
	PIRP Irp;
	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);
	PIO_STACK_LOCATION stack;
	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = IoGetDeviceObjectPointer(&DeviceName, FILE_ANY_ACCESS, &FileObject, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("获取设备对象指针出错%x\n", status));
		return;
	}
	KdPrint(("设备对象:%p", DeviceObject));
	KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

	Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
	if (Irp != NULL)
	{
		Irp->UserIosb = &IoStatus;
		Irp->UserEvent = &Event;
		Irp->AssociatedIrp.SystemBuffer = NULL;
		Irp->Tail.Overlay.Thread = PsGetCurrentThread();
		stack = IoGetNextIrpStackLocation(Irp);
		stack->FileObject = FileObject;
		stack->DeviceObject = DeviceObject;
		stack->MajorFunction = IRP_MJ_READ;
		stack->MinorFunction = IRP_MN_NORMAL;
		status = IoCallDriver(DeviceObject, Irp);

		if (status == STATUS_PENDING)
		{
			KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
			KdPrint(("完成读操作\n"));
		}

	}


	ObDereferenceObject(FileObject);


}

VOID MyGetDeviceObject()
{
	NTSTATUS status;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	PDEVICE_OBJECT DeviceObject;
// 	OBJECT_ATTRIBUTES oa;
// 	InitializeObjectAttributes(&oa, &DeviceName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ObReferenceObjectByName(&DeviceName, OBJ_CASE_INSENSITIVE, NULL, FILE_ANY_ACCESS, *IoDeviceObjectType, KernelMode, NULL, &DeviceObject);
	if (NT_SUCCESS(status))
	{
		KdPrint(("设备对象指针对获取成功\n"));
		KdPrint(("%p", DeviceObject));
		ObDereferenceObject(DeviceObject);
	}
	else
	{
		KdPrint(("设备对象获取失败%x", status));
	}
	

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{

	DriverObject->DriverUnload = Unload;
	//CallDrivertest1();
	//CallDrivertest2();
	//CallDrivertest3();
	//CallDrivertest5();
	MyGetDeviceObject();
	return STATUS_SUCCESS;
}
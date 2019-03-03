#include <ntddk.h>

#define CTLBUFFERED	CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTLDIRECT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)
#define CTLNEITHER	CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_NEITHER,FILE_ANY_ACCESS)
#define IOCTL_SEND_EVENT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
typedef struct _DEVICE_EXTENSION 
{
	PCHAR Buffer;
	ULONG Length;
#define MAX_FILE_LEN	4096
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");

	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);

	KdPrint(("驱动卸载\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	static char* irpname[] = {				
		"IRP_MJ_CREATE					",
		"IRP_MJ_CREATE_NAMED_PIPE		",
		"IRP_MJ_CLOSE					",
		"IRP_MJ_READ					",
		"IRP_MJ_WRITE					",
		"IRP_MJ_QUERY_INFORMATION		",
		"IRP_MJ_SET_INFORMATION			",
		"IRP_MJ_QUERY_EA				",
		"IRP_MJ_SET_EA					",
		"IRP_MJ_FLUSH_BUFFERS			",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION	",
		"IRP_MJ_DIRECTORY_CONTROL		",
		"IRP_MJ_FILE_SYSTEM_CONTROL		",
		"IRP_MJ_DEVICE_CONTROL			",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL	",
		"IRP_MJ_SHUTDOWN				",
		"IRP_MJ_LOCK_CONTROL			",
		"IRP_MJ_CLEANUP					",
		"IRP_MJ_CREATE_MAILSLOT			",
		"IRP_MJ_QUERY_SECURITY			",
		"IRP_MJ_SET_SECURITY			",
		"IRP_MJ_POWER					",
		"IRP_MJ_SYSTEM_CONTROL			",
		"IRP_MJ_DEVICE_CHANGE			",
		"IRP_MJ_QUERY_QUOTA				",
		"IRP_MJ_SET_QUOTA				",
		"IRP_MJ_PNP						"
	};
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("%s\n", irpname[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	ULONG Length = 0;
	ULONG offset;
	ULONG ReadLength;
	__try
	{
		Length = stack->Parameters.Read.Length;
		offset = stack->Parameters.Read.ByteOffset.LowPart;
		if (Length+offset<MAX_FILE_LEN)
		{
			if (Length+offset>pdx->Length)
			{

				ReadLength = pdx->Length - offset;
				KdPrint(("1：%d\n", ReadLength));
			}
			else
			{
				ReadLength = Length;
				KdPrint(("2：%d\n", ReadLength));
			}
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, pdx->Buffer + offset, ReadLength);
			status = STATUS_SUCCESS;

		}
		else
		{
			status = STATUS_BUFFER_TOO_SMALL;
			ReadLength = 0;
		}
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		ReadLength = 0;
	}
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = ReadLength;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	PDEVICE_EXTENSION pdx = DeviceObject->DeviceExtension;
	ULONG offset;
	ULONG Length;
	ULONG WriteLength;
	KdPrint(("写操作\n"));
	__try
	{
		Length = stack->Parameters.Write.Length;
		offset = stack->Parameters.Write.ByteOffset.LowPart;
		if (Length+offset <= MAX_FILE_LEN)
		{
			if (Length+offset>pdx->Length)
			{
				pdx->Length = Length + offset;
			}
			KdPrint(("写入%d\n", pdx->Length));
			
			RtlCopyMemory(pdx->Buffer + offset, Irp->AssociatedIrp.SystemBuffer, Length);
			WriteLength = Length;
			KdPrint(("写操作完成%s\n", Irp->AssociatedIrp.SystemBuffer));
		}
		else
		{
			status = STATUS_BUFFER_TOO_SMALL;
			WriteLength = 0;
			KdPrint(("写操作出现问题\n"));
		}
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("写操作出错异常\n"));
		status = GetExceptionCode();
		Length = 0;
	}
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = WriteLength;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchQueryInfo(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	ULONG Length;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	__try
	{
		Length = stack->Parameters.QueryFile.Length;
		if (stack->Parameters.QueryFile.FileInformationClass=FileStandardInformation&&
			Length>=sizeof(FILE_STANDARD_INFORMATION))
		{
			PFILE_STANDARD_INFORMATION pfsi = (PFILE_STANDARD_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
			pfsi->EndOfFile.LowPart = pdx->Length;
			status = STATUS_SUCCESS;
			Length = sizeof(FILE_STANDARD_INFORMATION);
		}
		else
		{
			status = STATUS_BUFFER_TOO_SMALL;
			Length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		Length = 0;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = Length;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;

}

NTSTATUS DirectRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("这是直接方式\n"));
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(Irp);
	PCHAR Buffer;
	ULONG ByteOffset;
	ULONG ByteCount;
	PVOID Va;
	ULONG Length;
	__try
	{
		ByteOffset = MmGetMdlByteCount(Irp->MdlAddress);
		ByteCount = MmGetMdlByteCount(Irp->MdlAddress);
		Va = MmGetMdlVirtualAddress(Irp->MdlAddress);
		KdPrint(("Byteoffset:%d\n", ByteOffset));
		KdPrint(("ByteCount:%d\n", ByteCount));
		KdPrint(("%p\n", Va));

		Buffer = (PCHAR)MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		if (stack->Parameters.Read.Length <= ByteCount)
		{
			RtlCopyMemory(Buffer, "这是一段来自内核地址空间的数据", strlen("这是一段来自内核地址空间的数据"));
			Length = strlen("这是一段来自内核地址空间的数据");
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
			Length = 0;

		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Length = 0;
		status = GetExceptionCode();

	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS NeitherRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint(("进入NeitherRead\n"));
	NTSTATUS status;
	ULONG Length;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	__try
	{
		ProbeForWrite(Irp->UserBuffer, stack->Parameters.Read.Length, 4);
		RtlCopyMemory(Irp->UserBuffer, "这是来自内核的数据", strlen("这是来自内核的数据"));
		status = STATUS_SUCCESS;
		Length = strlen("这是来自内核的数据");


	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

		status = GetExceptionCode();
		Length = 0;
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = Length;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);


	return status;
}

NTSTATUS DispatchControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG IoCode= stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG InputLength;
	ULONG OutputLength;
	PCHAR InputBuffer;
	PCHAR OutputBuffer;
	ULONG Length;
	__try
	{
		switch (IoCode)
		{
		case IOCTL_SEND_EVENT:
		{
			HANDLE hEvent = *(PHANDLE)Irp->AssociatedIrp.SystemBuffer;
			PKEVENT pEvent;
			status = ObReferenceObjectByHandle(hEvent, EVENT_MODIFY_STATE, *ExEventObjectType, UserMode, &pEvent, NULL);
			if (NT_SUCCESS(status))
			{

				KeSetEvent(pEvent, IO_NO_INCREMENT, FALSE);

				ObDereferenceObject(pEvent);
			}
			status = STATUS_SUCCESS;
			Length = 0;
		}
			break;
		case CTLBUFFERED:
		{
			InputBuffer = Irp->AssociatedIrp.SystemBuffer;
			InputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = Irp->AssociatedIrp.SystemBuffer;
			OutputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;
			KdPrint(("inputBuffer:%s\n", InputBuffer));
			KdPrint(("InputLength:%d\n", InputLength));

			RtlCopyMemory(OutputBuffer, "这是缓冲区io方式", strlen("这是缓冲区io方式"));
			Length = strlen("这是缓冲区io方式");
			status = STATUS_SUCCESS;
		}

		break;

		case CTLDIRECT:
		{
			InputBuffer = Irp->AssociatedIrp.SystemBuffer;
			InputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
			OutputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;

			KdPrint(("InputBuffer:%s", InputBuffer));
			KdPrint(("输入缓冲区的长度:%d\n", InputLength));

			RtlCopyMemory(OutputBuffer, "直接方式io控制操作", strlen("直接方式io控制操作"));
			status = STATUS_SUCCESS;
			Length = strlen("直接方式io控制操作");


		}
		break;
		case CTLNEITHER:
		{
			InputBuffer = stack->Parameters.DeviceIoControl.Type3InputBuffer;
			InputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
			OutputBuffer = Irp->UserBuffer;
			OutputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;

			KdPrint(("inputBuffer:%s\n", InputBuffer));
			KdPrint(("输入缓冲区长度%d\n", InputLength));
			RtlCopyMemory(OutputBuffer, "其它方式io控制操作", strlen("其它方式io控制操作"));
			status = STATUS_SUCCESS;
			Length = strlen("其它方式io控制操作");

		}
			break;
		default:
		{
			status = STATUS_SUCCESS;
			Length = 0;
		}
		break;
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		Length = 0;
	}
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = Length;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	PDEVICE_EXTENSION pdx;
	ULONG i = 0;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION;i++)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	
	DriverObject->MajorFunction[IRP_MJ_READ] = NeitherRead;
//	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	DriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = DispatchQueryInfo;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;
	status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建设备失败%x\n", status));
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("符号链接创建失败%d\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}
	DeviceObject->Flags |= 0;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	pdx = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	pdx->Buffer = (PCHAR)ExAllocatePool(PagedPool, 4096);
	pdx->Length = 0;


	return STATUS_SUCCESS;
}
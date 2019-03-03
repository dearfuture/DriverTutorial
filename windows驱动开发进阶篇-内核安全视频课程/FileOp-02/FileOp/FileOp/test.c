#include <ntifs.h>
#include <ntddk.h>
BOOLEAN MyDeleteFile(WCHAR FileName[]);
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	MyDeleteFile(L"\\??\\C:\\acpi.sys");
	KdPrint(("驱动加载\n"));
}

VOID MyCopyFile(WCHAR SrcFileName[], WCHAR DestFileName[])
{
	UNICODE_STRING usSrcFileName;
	UNICODE_STRING usDestFileName;
	OBJECT_ATTRIBUTES oa;
	NTSTATUS status;
	HANDLE hFile;
	IO_STATUS_BLOCK IoStatus;
	PCHAR Buffer = NULL;
	FILE_STANDARD_INFORMATION fsi = { 0 };

	if (SrcFileName==NULL||
		DestFileName== NULL
		)
	{
		return;
	}



	RtlInitUnicodeString(&usSrcFileName, SrcFileName);
	RtlInitUnicodeString(&usDestFileName, DestFileName);

	InitializeObjectAttributes(&oa, &usSrcFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hFile, 
		GENERIC_ALL,
		&oa, 
		&IoStatus, 
		NULL, 
		FILE_ATTRIBUTE_NORMAL, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		FILE_OPEN, 
		FILE_SYNCHRONOUS_IO_NONALERT, 
		NULL, 
		0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件打开失败%d\n", status));
		return;
	}
	status = ZwQueryInformationFile(hFile, &IoStatus, &fsi, sizeof(fsi), FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hFile);
		return;
	}

	if (fsi.EndOfFile.LowPart > 0)
	{
		Buffer = (PCHAR)ExAllocatePool(PagedPool, fsi.EndOfFile.LowPart);
	}
	if (Buffer != NULL)
	{
		status = ZwReadFile(hFile, NULL, NULL, NULL, &IoStatus, Buffer, fsi.EndOfFile.LowPart, NULL, NULL);
		if (!NT_SUCCESS(status))
		{
			ZwClose(hFile);
			return;
		}
		
	}
	ZwClose(hFile);

	RtlZeroMemory(&oa, sizeof(oa));
	InitializeObjectAttributes(&oa, &usDestFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hFile, 
		GENERIC_ALL, 
		&oa,
		&IoStatus,
		NULL, 
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		FILE_OVERWRITE_IF, 
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 
		0);
	if (!NT_SUCCESS(status))
	{

		return;
	}
	status = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatus, Buffer, fsi.EndOfFile.LowPart, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("写入出错%x\n", status));
		ZwClose(hFile);
		return;
	}

	ZwClose(hFile);
	if (Buffer != NULL)
	{
		ExFreePool(Buffer);
	}


}

BOOLEAN MyDeleteFile(WCHAR FileName[])
{
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING usFileName;
	NTSTATUS status;
	if (FileName == NULL)
	{
		return FALSE;
	}
	
	RtlInitUnicodeString(&usFileName, FileName);
	InitializeObjectAttributes(&oa, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwDeleteFile(&oa);




	return NT_SUCCESS(status);
}



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动卸载\n"));
	DriverObject->DriverUnload = Unload;
	//KdBreakPoint();
	MyCopyFile(L"\\??\\C:\\windows\\system32\\drivers\\acpi.sys",
		L"\\??\\C:\\acpi.sys");


	return STATUS_SUCCESS;
}
#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("驱动卸载\n"));
}

VOID CreateFileTest1()
{
	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test.log");
	OBJECT_ATTRIBUTES oa;

	InitializeObjectAttributes(&oa, &FilePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hFile, 
		GENERIC_ALL, 
		&oa,
		&IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件创建失败%x\n",status));
		return;
	}
	KdPrint(("文件创建成功\n"));
	ZwClose(hFile);



}

VOID CreateFileTest2()
{
	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test2.log");

	InitializeObjectAttributes(&oa, &FilePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = IoCreateFile(&hFile,
		GENERIC_ALL,
		&oa,
		&IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		CreateFileTypeNone,
		NULL,
		IO_NO_PARAMETER_CHECKING
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件创建2失败%x\n", status));
		return;
	}
	KdPrint(("文件创建2成功\n"));
	ZwClose(hFile);

}


VOID QueryFileAttribute()
{
	HANDLE hFile;
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test.sys");
	IO_STATUS_BLOCK IoStatus;
	FILE_STANDARD_INFORMATION fsi;
	InitializeObjectAttributes(&oa, &FilePath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenFile(&hFile, FILE_ALL_ACCESS, &oa, &IoStatus, FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件打开失败\n"));
		return;
	}
	status = ZwQueryInformationFile(hFile, &IoStatus, &fsi, sizeof(fsi), FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件信息查询失败%x\n", status));
		ZwClose(hFile);
		return;

	}

	KdPrint(("文件的大小%d字节\n", fsi.EndOfFile.LowPart));
	ZwClose(hFile);


}

VOID WriteFileTest()
{
	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test.log");
	InitializeObjectAttributes(&oa, &FilePath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwCreateFile(&hFile, 
		GENERIC_ALL, 
		&oa, 
		&IoStatus, 
		NULL, 
		FILE_ATTRIBUTE_NORMAL, 
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件创建失败%x\n", status));
		return;
	}
	ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatus, "这是写入的文件内容", strlen("这是写入的文件内容"), NULL, NULL);
	ZwClose(hFile);


}

VOID ReadFileTest()
{

	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	OBJECT_ATTRIBUTES oa;
	CHAR Buffer[1024] = { 0 };
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test.log");
	InitializeObjectAttributes(&oa, &FilePath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwCreateFile(&hFile,
		GENERIC_ALL,
		&oa,
		&IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("文件创建失败%x\n", status));
		return;
	}
	ZwReadFile(hFile, NULL, NULL, NULL, &IoStatus, Buffer, sizeof(Buffer), NULL, NULL);
	KdPrint(("读取的文件内容:%s\n", Buffer));
	ZwClose(hFile);



}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	
// 	CreateFileTest1(); 
// 	CreateFileTest2();

//	QueryFileAttribute();
	WriteFileTest();
	ReadFileTest();

	return STATUS_SUCCESS;
}
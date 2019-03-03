#include <ntifs.h>
#include <ntddk.h>
#include <stdio.h>
#include <ntimage.h>

PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);

VOID
LoadImageNotify(
_In_ PUNICODE_STRING FullImageName,
_In_ HANDLE ProcessId,                // pid into which image is being mapped
_In_ PIMAGE_INFO ImageInfo
);

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	PsRemoveLoadImageNotifyRoutine(LoadImageNotify);
	KdPrint(("驱动卸载\n"));
}


KIRQL WPOFF()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	ULONG_PTR cr0 = __readcr0();
#ifdef _X86_
	cr0 &= 0xfffeffff;
#else
	cr0 &= 0xfffffffffffeffff;
#endif
	_disable();
	__writecr0(cr0);

	return irql;
}


VOID WPON(KIRQL irql)
{

	ULONG_PTR cr0 = __readcr0();
	cr0 |= 0x10000;
	__writecr0(cr0);
	_enable();

	KeLowerIrql(irql);
}


VOID DenyDriver(PCHAR ImageBase)
{
	char Shellcode[6] = "\xB8\x22\x00\x00\xC0\xC3";
	PCHAR AddressEntry;
	KIRQL irql;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS NtHeader = (PIMAGE_NT_HEADERS)(ImageBase + DosHeader->e_lfanew);
	AddressEntry = ImageBase + NtHeader->OptionalHeader.AddressOfEntryPoint;
	irql = WPOFF();
	RtlCopyMemory(AddressEntry, Shellcode, sizeof(Shellcode));
	WPON(irql);

}

//
VOID
LoadImageNotify(
_In_ PUNICODE_STRING FullImageName,
_In_ HANDLE ProcessId,                // pid into which image is being mapped
_In_ PIMAGE_INFO ImageInfo
)
{
	PEPROCESS Process;
	CHAR ModuleName[256] = { 0 };
	if (ProcessId == 0)
	{

		sprintf(ModuleName, "%wZ", FullImageName);
		if (strstr(_strlwr(ModuleName),"test.sys")!=NULL)
		{
			DenyDriver(ImageInfo->ImageBase);
		}
		KdPrint(("模块加载%wZ\n", FullImageName));
	}
	else
	{
		PsLookupProcessByProcessId(ProcessId, &Process);
		KdPrint(("%d:%s,%wZ\n",ProcessId, PsGetProcessImageFileName(Process), FullImageName));
	}

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	PsSetLoadImageNotifyRoutine(LoadImageNotify);
	
	return STATUS_SUCCESS;
}
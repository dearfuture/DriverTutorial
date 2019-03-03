#include <ntifs.h>
#include <ntddk.h>


VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));
}


PVOID Getpdt(PEPROCESS Process)
{
	if (sizeof(PVOID) == 4)
	{
		return  *(PVOID*)((PCHAR)Process + 0x18);
	}
	else
	{
		return  *(PVOID*)((PCHAR)Process + 0x28);
	}


}

VOID KReadProcessMemory2(PEPROCESS Process, PVOID Address, ULONG Length, PVOID Buffer)
{
	PVOID Pdt = Getpdt(Process);
	PVOID oldpdt;

	_disable();
	oldpdt = (PVOID)__readcr3();
	__writecr3(Pdt);
	_enable();

	if (MmIsAddressValid(Address))
	{
		RtlCopyMemory(Buffer, Address, Length);
	}
	_disable();
	__writecr3(oldpdt);
	_enable();

}

VOID KWriteProcessMemory2(PEPROCESS Process, PVOID Address, ULONG Length, PVOID Buffer)
{
	PVOID Pdt = Getpdt(Process);
	PVOID oldpdt;

	_disable();
	oldpdt = (PVOID)__readcr3();
	__writecr3(Pdt);
	_enable();

	if (MmIsAddressValid(Address))
	{
		RtlCopyMemory(Address,Buffer, Length);
	}
	_disable();
	__writecr3(oldpdt);
	_enable();

}




VOID KReadProcessMemory(PEPROCESS Process, PVOID Address, ULONG Length, PVOID Buffer)
{
	KAPC_STATE apcstate;
	KeStackAttachProcess(Process, &apcstate);

	if (MmIsAddressValid(Address))
	{
		RtlCopyMemory(Buffer, Address, Length);
	}

	KeUnstackDetachProcess(&apcstate);

}

VOID KWriteProcessMemory(PEPROCESS Process, PVOID Address, ULONG Length, PVOID Buffer)
{
	KAPC_STATE apcstate;
	KeStackAttachProcess(Process, &apcstate);

	if (MmIsAddressValid(Address))
	{
		RtlCopyMemory(Address,Buffer, Length);
	}

	KeUnstackDetachProcess(&apcstate);
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
	DriverObject->DriverUnload = Unload;

	return STATUS_SUCCESS;
}
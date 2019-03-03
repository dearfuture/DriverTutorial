#include <ntifs.h>
#include <ntddk.h>
#include "LDE64x64.h"


KIRQL WPOFF();
VOID WPON(KIRQL irql);

typedef NTSTATUS
(*PSLOOKUPPROCESSBYPROCESSID)(
_In_ HANDLE ProcessId,
_Outptr_ PEPROCESS *Process
);

PSLOOKUPPROCESSBYPROCESSID OriFun;
ULONG PachSize;

PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTSTATUS
Proxy_PsLookupProcessByProcessId(
_In_ HANDLE ProcessId,
_Outptr_ PEPROCESS *Process
)
{
	NTSTATUS status = STATUS_SUCCESS;
	status = OriFun(ProcessId, Process);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	if (strcmp(PsGetProcessImageFileName(*Process), "calc.exe") == 0)
	{
		return STATUS_ACCESS_DENIED;
	}

	return status;
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KIRQL irql;
	irql = WPOFF();
	RtlCopyMemory(PsLookupProcessByProcessId, OriFun, PachSize);


	WPON(irql);
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));
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


VOID HookKernelRoutine(PVOID ApiAddress,PVOID Proxy_Address,PVOID *Ori_address,ULONG *PatchSize)
{
	int len = 0;
	int totallen = 0;
	KIRQL irql;
	PCHAR Addr = (PCHAR)ApiAddress;
	 
	char jmp_code[14] = "\xFF\x25\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	char jmp_Oricode[14] = "\xFF\x25\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	while (totallen<14)
	{
		len = LDE(ApiAddress, 64);
		Addr += len;
		totallen += len;
	}

	*PatchSize = totallen;
	*Ori_address = ExAllocatePool(NonPagedPool, totallen + sizeof(jmp_Oricode));
	RtlCopyMemory(*Ori_address, ApiAddress, totallen);

	*(PVOID*)&jmp_Oricode[6] = (PCHAR)ApiAddress + *PatchSize;
	RtlCopyMemory((PCHAR)*Ori_address + *PatchSize, jmp_Oricode, sizeof(jmp_Oricode));

	*(PVOID*)&jmp_code[6] = Proxy_PsLookupProcessByProcessId;

	irql = WPOFF();
	RtlCopyMemory(ApiAddress, jmp_code, sizeof(jmp_code));
	WPON(irql);


}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
	DriverObject->DriverUnload = Unload;
	LDE_init();

	HookKernelRoutine(PsLookupProcessByProcessId, Proxy_PsLookupProcessByProcessId, (PVOID*)&OriFun, &PachSize);

	return STATUS_SUCCESS;
}
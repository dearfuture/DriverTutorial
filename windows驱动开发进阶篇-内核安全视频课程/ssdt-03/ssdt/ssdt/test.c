#include <ntddk.h>

typedef struct _KSYSTEM_SERVICE_TABLE
{
	PLONG  ServiceTableBase;                                       // 服务函数地址表基址  
	PLONG  ServiceCounterTableBase;
	ULONG   NumberOfService;                                        // 服务函数的个数  
	PLONG   ParamTableBase;                                        // 服务函数参数表基址   
} KSYSTEM_SERVICE_TABLE, *PKSYSTEM_SERVICE_TABLE;

#ifdef _X86_
extern PKSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;
#else
#endif


typedef NTSTATUS (*NTTERMINATEPROCESS)(HANDLE ProcessHandle, NTSTATUS ExitStatus);
NTTERMINATEPROCESS OldNtTerminateProcess;
PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);
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

NTSTATUS MyNtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus)
{
	PEPROCESS Process;
	NTSTATUS status;
	status = ObReferenceObjectByHandle(ProcessHandle, PROCESS_ALL_ACCESS, *PsProcessType, KernelMode, &Process, NULL);
	if (NT_SUCCESS(status))
	{
		if (strcmp(PsGetProcessImageFileName(Process),"calc.exe")==0)
		{
			return STATUS_ACCESS_DENIED;
		}
	}

	return OldNtTerminateProcess(ProcessHandle,ExitStatus);
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("驱动卸载\n"));
}

VOID Enumssdt32()
{
	ULONG i = 0;
#ifdef _X86_
	for (i = 0; i < KeServiceDescriptorTable->NumberOfService; i++)
	{

		KdPrint(("%d\t0x%p\n", i, KeServiceDescriptorTable->ServiceTableBase[i]));
	}
#endif
	KdPrint(("驱动加载\n"));
}

VOID Enumssdt64()
{
	ULONG i = 0;
	PKSYSTEM_SERVICE_TABLE pServiceTable = NULL;
	PCHAR Base;
	PCHAR FuncAddr;
	LONG offset;
	LONG funoffset;
	KdBreakPoint();
	Base = (PCHAR)__readmsr(0xc0000082);
	Base += 313;
	offset = *(PLONG)(Base - 4);
	pServiceTable = (PKSYSTEM_SERVICE_TABLE)(Base + offset);
	for (i = 0; i < pServiceTable->NumberOfService;i++)
	{
		funoffset = pServiceTable->ServiceTableBase[i];
		funoffset >>= 4;
		FuncAddr = (PCHAR)((PCHAR)(pServiceTable->ServiceTableBase) + funoffset);
		KdPrint(("%d\t0x%p\n", i, FuncAddr));
	}


	
}

VOID HookTerminateProcess()
{
	KIRQL irql;
	if (SharedUserData->NtMajorVersion == 5)
	{
		OldNtTerminateProcess = (NTTERMINATEPROCESS)KeServiceDescriptorTable->ServiceTableBase[257];

		irql = WPOFF();

		(NTTERMINATEPROCESS)KeServiceDescriptorTable->ServiceTableBase[257] = MyNtTerminateProcess;
		WPON(irql);
	}
	else
	{
		OldNtTerminateProcess = (NTTERMINATEPROCESS)KeServiceDescriptorTable->ServiceTableBase[370];
		irql = WPOFF();
		(NTTERMINATEPROCESS)KeServiceDescriptorTable->ServiceTableBase[370] = MyNtTerminateProcess;
		WPON(irql);
	}


}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	
	DriverObject->DriverUnload = Unload;
//	Enumssdt64();

	HookTerminateProcess();
	return STATUS_SUCCESS;
}
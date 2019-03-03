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



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	
	DriverObject->DriverUnload = Unload;
	Enumssdt64();
	return STATUS_SUCCESS;
}
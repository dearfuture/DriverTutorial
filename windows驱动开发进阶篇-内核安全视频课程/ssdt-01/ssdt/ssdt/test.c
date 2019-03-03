#include <ntddk.h>

typedef struct _KSYSTEM_SERVICE_TABLE
{
	PULONG  ServiceTableBase;                                       // 服务函数地址表基址  
	PULONG  ServiceCounterTableBase;
	ULONG   NumberOfService;                                        // 服务函数的个数  
	PULONG   ParamTableBase;                                        // 服务函数参数表基址   
} KSYSTEM_SERVICE_TABLE, *PKSYSTEM_SERVICE_TABLE;

#ifdef _X86_
extern PKSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;
#else
#endif

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("驱动卸载\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	ULONG i = 0;
	for (i = 0; i < KeServiceDescriptorTable->NumberOfService; i++)
	{

		KdPrint(("%d\t0x%p\n", i,KeServiceDescriptorTable->ServiceTableBase[i]));
	}
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	return STATUS_SUCCESS;
}
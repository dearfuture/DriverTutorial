#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("驱动卸卸\n"));
}

KIRQL WPOFF()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	ULONG_PTR cr0=__readcr0();
#ifdef _X86_
	cr0 &= 0xfffeffff;
#else
	cr0&=0xfffffffffffeffff;
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


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KIRQL irql;
	PVOID p1 = NULL;
	PVOID p2 = NULL;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;

	irql = WPOFF();
	//hook操作
	p1 = ExAllocatePool(PagedPool, 1024);
	RtlZeroMemory(p1, 1024);
	RtlCopyMemory(p1, "hello world!", strlen("hello world!"));
	KdPrint(("%s\n", p1));
	p2 = ExAllocatePool(NonPagedPool, 1024);

	RtlZeroMemory(p2, 1024);
	RtlCopyMemory(p2, p1, 1024);
	KdPrint(("%s\n", p2));

	WPON(irql);
	
	return STATUS_SUCCESS;
}
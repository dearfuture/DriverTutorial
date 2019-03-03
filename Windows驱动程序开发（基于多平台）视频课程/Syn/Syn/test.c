#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));

}

VOID SpinLockTest()
{
	KIRQL irql;
	KSPIN_LOCK SpinLock;
	KeInitializeSpinLock(&SpinLock);
	switch (KeGetCurrentIrql())
	{
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("APC_LEVEL\n"));

		break;
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	default:
		break;
	}

	KeAcquireSpinLock(&SpinLock, &irql);
	switch (KeGetCurrentIrql())
	{
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("APC_LEVEL\n"));

		break;
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	default:
		break;
	}

	KeReleaseSpinLock(&SpinLock, irql);
	switch (KeGetCurrentIrql())
	{
	case DISPATCH_LEVEL:
		KdPrint(("DISPATCH_LEVEL\n"));
		break;
	case APC_LEVEL:
		KdPrint(("APC_LEVEL\n"));

		break;
	case PASSIVE_LEVEL:
		KdPrint(("PASSIVE_LEVEL\n"));
		break;
	default:
		break;
	}



}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KIRQL irql;
	DriverObject->DriverUnload = Unload;
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
// 	switch (KeGetCurrentIrql())
// 	{
// 	case DISPATCH_LEVEL:
// 		KdPrint(("DISPATCH_LEVEL\n"));
// 		break;
// 	case APC_LEVEL:
// 		KdPrint(("APC_LEVEL\n"));
// 
// 		break;
// 	case PASSIVE_LEVEL:
// 		KdPrint(("PASSIVE_LEVEL\n"));
// 		break;
// 	default:
//		break;
// 	}
// 
// 	KeRaiseIrql(DISPATCH_LEVEL, &irql);
// 	switch (KeGetCurrentIrql())
// 	{
// 	case DISPATCH_LEVEL:
// 		KdPrint(("DISPATCH_LEVEL\n"));
// 		break;
// 	case APC_LEVEL:
// 		KdPrint(("APC_LEVEL\n"));
// 
// 		break;
// 	case PASSIVE_LEVEL:
// 		KdPrint(("PASSIVE_LEVEL\n"));
// 		break;
// 	default:
//		break;
// 	}
// 
// 	KeLowerIrql(irql);
// 	switch (KeGetCurrentIrql())
// 	{
// 	case DISPATCH_LEVEL:
// 		KdPrint(("DISPATCH_LEVEL\n"));
// 		break;
// 	case APC_LEVEL:
// 		KdPrint(("APC_LEVEL\n"));
// 
// 		break;
// 	case PASSIVE_LEVEL:
// 		KdPrint(("PASSIVE_LEVEL\n"));
// 		break;
// 	default:
//		break;
// 	}

	SpinLockTest();

	return STATUS_SUCCESS;
}
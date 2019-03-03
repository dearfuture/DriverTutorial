#include <ntddk.h>

typedef struct _MYDATA
{
	int number;
}MYDATA,*PMYDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("驱动卸载\n"));
}

VOID LookAsideTest()
{
	PAGED_LOOKASIDE_LIST LookAsideList;
	PMYDATA pdata[100] = { NULL };
	int i = 0;

	KdPrint(("分页快查表测试\n"));
	ExInitializePagedLookasideList(&LookAsideList, NULL, NULL, 0, sizeof(MYDATA), 'abcd', 0);

	for (i = 0; i < 100; i++)
	{
		pdata[i] = (PMYDATA)ExAllocateFromPagedLookasideList(&LookAsideList);
		pdata[i]->number = i;
	}

	for (i = 0; i < 100; i++)
	{
		KdPrint(("%d\n", pdata[i]->number));

		ExFreeToPagedLookasideList(&LookAsideList, pdata[i]);
	}

	ExDeletePagedLookasideList(&LookAsideList);
	KdPrint(("******************\n"));


}

VOID LookAsideTest2()
{
	NPAGED_LOOKASIDE_LIST LookAsideList;
	PMYDATA pdata[100] = { NULL };
	int i = 0;

	KdPrint(("非分页快查表测试\n"));
	ExInitializeNPagedLookasideList(&LookAsideList, NULL, NULL, 0, sizeof(MYDATA), 'abcd', 0);

	for (i = 0; i < 100; i++)
	{
		pdata[i] = (PMYDATA)ExAllocateFromNPagedLookasideList(&LookAsideList);
		pdata[i]->number = i;
	}

	for (i = 0; i < 100; i++)
	{
		KdPrint(("%d\n", pdata[i]->number));

		ExFreeToNPagedLookasideList(&LookAsideList, pdata[i]);
	}

	ExDeleteNPagedLookasideList(&LookAsideList);
	KdPrint(("******************\n"));

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	LookAsideTest();
	LookAsideTest2();
	return STATUS_SUCCESS;
}
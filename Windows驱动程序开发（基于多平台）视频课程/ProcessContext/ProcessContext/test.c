#include <ntddk.h>

PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);

typedef struct _MYDATA 
{
	LIST_ENTRY ListEntry;
	int number;
}MYDATA,*PMYDATA;

typedef struct _MYDATA2
{
	int number;
}MYDATA2, *PMYDATA2;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//KdPrint(("%s\n", PsGetProcessImageFileName(PsGetCurrentProcess())));
	KdPrint(("驱动卸载\n"));

}

VOID LinkListTest()
{
	LIST_ENTRY ListHead;
	InitializeListHead(&ListHead);
	KdPrint(("链表测试\n"));
	for (int i = 0; i < 10; i++)
	{
		PMYDATA pdata = (PMYDATA)ExAllocatePool(PagedPool, sizeof(MYDATA));
		pdata->number = i;
		InsertHeadList(&ListHead, &pdata->ListEntry);
	}

	while (!IsListEmpty(&ListHead))
	{
		PLIST_ENTRY pEntry = RemoveTailList(&ListHead);
		PMYDATA pdata = CONTAINING_RECORD(pEntry, MYDATA, ListEntry);
		KdPrint(("%d\n", pdata->number));
		ExFreePool(pdata);
	}



}

VOID LookAsideTest()
{
	PMYDATA2 data[50] = { 0 };
	int i;
	PAGED_LOOKASIDE_LIST LookAsideList;
	ExInitializePagedLookasideList(&LookAsideList, NULL, NULL, 0, sizeof(MYDATA2), 'abcd', 0);;
	;

	for (i = 0; i < 50;i++)
	{
		data[i] = ExAllocateFromPagedLookasideList(&LookAsideList);
		data[i]->number = i;
	}

	for (i = 0; i < 50;i++)
	{
		KdPrint(("%d\n", data[i]->number));
		ExFreeToPagedLookasideList(&LookAsideList,data[i]);
	}
	ExDeletePagedLookasideList(&LookAsideList);

}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	PCHAR pcstr;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	pcstr = (PCHAR)ExAllocatePool(NonPagedPool, 1024);
	if (pcstr == NULL)
	{
		KdPrint(("内存分配失败\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	KdPrint(("内存分配成功\n"));
	RtlZeroMemory(pcstr, 1024);
	strcpy(pcstr, "这是一次内存分配的测试");
	KdPrint(("%s\n", pcstr));
	ExFreePool(pcstr);

	pcstr = (PCHAR)ExAllocatePoolWithTag(PagedPool, 1024, 'abcd');
	if (pcstr == NULL)
	{
		KdPrint(("内存分配失败\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	KdPrint(("带标签的内存分配成功\n"));
	RtlZeroMemory(pcstr, 1024);
	strcpy(pcstr, "这是一次带标签的内存测试\n");
	KdPrint(("%s\n", pcstr));
	ExFreePoolWithTag(pcstr,'abcd');

	//KdPrint(("%s\n", PsGetProcessImageFileName(PsGetCurrentProcess())));
	LinkListTest();
	LookAsideTest();
	return STATUS_SUCCESS;
}
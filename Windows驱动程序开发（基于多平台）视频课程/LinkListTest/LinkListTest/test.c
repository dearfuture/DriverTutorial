#include <ntddk.h>

typedef struct _MYDATA 
{
	LIST_ENTRY ListEntry;
	int number;
}MYDATA,*PMYDATA;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Çý¶¯Ð¶ÔØ\n"));
}

VOID LinkListTest()
{
	LIST_ENTRY ListHead;
	int i = 0;
	InitializeListHead(&ListHead);
	
	for (i = 0; i < 10;i++)
	{
		PMYDATA pdata = (PMYDATA)ExAllocatePool(PagedPool, sizeof(MYDATA));
		pdata->number = i;
		InsertHeadList(&ListHead, &pdata->ListEntry);
	}

	while (!IsListEmpty(&ListHead))
	{
		PLIST_ENTRY pEntry = RemoveHeadList(&ListHead);
		PMYDATA pdata = CONTAINING_RECORD(pEntry, MYDATA, ListEntry);
		KdPrint(("%d\n", pdata->number));
		ExFreePool(pdata);
	}

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Çý¶¯¼ÓÔØ\n"));
	DriverObject->DriverUnload = Unload;

	LinkListTest();

	return STATUS_SUCCESS;
}
#include <ntifs.h>
#include <ntddk.h>

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;//这个成员把系统所有加载(可能是停止没被卸载)已经读取到内存中 我们关系第一个  我们要遍历链表 双链表 不管中间哪个节点都可以遍历整个链表 本驱动的驱动对象就是一个节点
	LIST_ENTRY InMemoryOrderLinks;//系统已经启动 没有被初始化 没有调用DriverEntry这个历程的时候 通过这个链表进程串接起来
	LIST_ENTRY InInitializationOrderLinks;//已经调用DriverEntry这个函数的所有驱动程序
	PVOID DllBase;
	PVOID EntryPoint;//驱动的进入点 DriverEntry
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;//驱动的满路径
	UNICODE_STRING BaseDllName;//不带路径的驱动名字
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
NTSTATUS EnableProtect(BOOLEAN Enable);
HANDLE ObHandle;
HANDLE ObHandle1;
PCHAR PsGetProcessImageFileName(IN PEPROCESS Process);
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	EnableProtect(FALSE);
	KdPrint(("驱动卸卸\n"));
}
NTSTATUS BypassDriverCheck(PDRIVER_OBJECT DriverObject)
{
	if (DriverObject==NULL)
	{
		return STATUS_INVALID_PARAMETER;
	}
	PLDR_DATA_TABLE_ENTRY ldrData = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	ldrData->Flags |= 0x20;
	return STATUS_SUCCESS;
}


OB_PREOP_CALLBACK_STATUS
Precb1(
_In_ PVOID RegistrationContext,
_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
)
{

	PEPROCESS Process;
	PETHREAD Thread;
	if (OperationInformation->ObjectType != *PsThreadType)
	{
		goto End;
	}
	Thread = OperationInformation->Object;
	Process = IoThreadToProcess(Thread);
	if (_stricmp(PsGetProcessImageFileName(Process), "calc.exe") == 0)
	{
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)//
		{
			if (OperationInformation->Parameters->CreateHandleInformation.DesiredAccess&THREAD_TERMINATE)
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~THREAD_TERMINATE;
			}
			if (OperationInformation->Parameters->CreateHandleInformation.DesiredAccess&THREAD_SUSPEND_RESUME)
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~THREAD_SUSPEND_RESUME;
			}


		}
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
		{
			if (OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess&THREAD_TERMINATE)
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~THREAD_TERMINATE;
			}
			if (OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess&THREAD_SUSPEND_RESUME)
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~THREAD_SUSPEND_RESUME;
			}
		}

	}



End:


	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS
Precb(
_In_ PVOID RegistrationContext,
_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
#define 	PROCESS_TERMINATE 0x0001 
	PEPROCESS Process;
	if (OperationInformation->ObjectType != *PsProcessType)
	{
		goto End;
	}
	Process = OperationInformation->Object;
	if (_stricmp(PsGetProcessImageFileName(Process),"calc.exe")==0)
	{
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			if (OperationInformation->Parameters->CreateHandleInformation.DesiredAccess&PROCESS_TERMINATE)
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
			}
			
		}
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
		{
			if (OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess&PROCESS_TERMINATE)
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
			}
		}
		
	}
	


End:


	return OB_PREOP_SUCCESS;
}

NTSTATUS EnableProtect(BOOLEAN Enable)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	if (Enable)
	{
		OB_CALLBACK_REGISTRATION ObCb = { 0 };
		OB_OPERATION_REGISTRATION ObOp = { 0 };

		OB_CALLBACK_REGISTRATION ObCb1 = { 0 };
		OB_OPERATION_REGISTRATION ObOp1 = { 0 };

		ObCb.Version = OB_FLT_REGISTRATION_VERSION;
		ObCb.RegistrationContext = NULL;
		ObCb.OperationRegistrationCount = 1;
		ObCb.OperationRegistration = &ObOp;
		RtlInitUnicodeString(&ObCb.Altitude, L"123456");
		ObOp.ObjectType = PsProcessType;
		ObOp.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		ObOp.PreOperation = Precb;
		
		status = ObRegisterCallbacks(&ObCb, &ObHandle);
		if (NT_SUCCESS(status))
		{
			KdPrint(("注册成功\n"));
		}
		else
		{
			KdPrint(("进程回调注册失败%x\n", status));
		}


		ObCb1.Version = OB_FLT_REGISTRATION_VERSION;
		ObCb1.RegistrationContext = NULL;
		ObCb1.OperationRegistrationCount = 1;
		ObCb1.OperationRegistration = &ObOp1;
		RtlInitUnicodeString(&ObCb1.Altitude, L"32412");
		ObOp1.ObjectType = PsThreadType;
		ObOp1.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		ObOp1.PreOperation = Precb1;

		status = ObRegisterCallbacks(&ObCb1, &ObHandle1);
		if (NT_SUCCESS(status))
		{
			KdPrint(("线程成功\n"));
		}
		else
		{
			KdPrint(("线程回调注册失败%x\n", status));
		}




	}
	else
	{
		if (ObHandle != NULL)
		{
			ObUnRegisterCallbacks(ObHandle);
		}
		if (ObHandle1 != NULL)
		{
			ObUnRegisterCallbacks(ObHandle1);
		}
	}

	return status;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	KdPrint(("驱动加载\n"));
	DriverObject->DriverUnload = Unload;
	BypassDriverCheck(DriverObject);
	EnableProtect(TRUE);
	return STATUS_SUCCESS;
}
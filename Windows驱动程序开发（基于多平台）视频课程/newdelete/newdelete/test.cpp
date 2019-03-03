#include <ntddk.h>

void* __cdecl operator new(size_t size)
{
	KdPrint(("进入全局的new\n"));
	return ExAllocatePool(PagedPool, size);
}

void __cdecl operator delete(void *p)
{
	KdPrint(("进入全局的delete\n"));
	if (p != NULL)
	{
		ExFreePool(p);
	}
}

class Myclass
{
public:
	Myclass()
	{
		KdPrint(("构造函数\n"));
	}

	~Myclass()
	{
		KdPrint(("析构函数\n"));
	}
	void* __cdecl operator new(size_t size, POOL_TYPE PoolType = PagedPool)
	{
		KdPrint(("进入类中的new\n"));
		return ExAllocatePool(PoolType, size);
	}

	void __cdecl operator delete(void *p)
	{
		KdPrint(("进入类中的delete\n"));
		if (p!=NULL)
		{
			ExFreePool(p);
		}
	}
	
protected:
private:
};

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	char Buffer[1024] = { 0 };
	char Src[128] = "中华人民共和国";
	char Dst[1024] = "中华人民共和国";
 	DriverObject->DriverUnload = Unload;
// 	PCHAR pstr = new char[100];
// 	delete[] pstr;
// 
// 	Myclass *pmyclass = new Myclass;
// 
// 	delete pmyclass;
	NTSTATUS status;
	RtlCopyMemory(Buffer, "这是一个内存的复制测试", strlen("这是一个内存的复制测试"));
	KdPrint(("%s\n", Buffer));
	RtlMoveMemory(Buffer, "这是一个内存的复制测试2", strlen("这是一个内存的复制测试2"));
	KdPrint(("%s\n", Buffer));

	RtlFillMemory(Buffer, 64, 0xcc);
	KdPrint(("%s\n", Buffer));

	RtlZeroMemory(Buffer, sizeof(Buffer));

	if (RtlEqualMemory(Src,Dst,sizeof(Src)))
	{
		KdPrint(("两块内存相同\n"));

	}
	else
	{
		KdPrint(("两块内存不同\n"));
	}
	

	
	return STATUS_SUCCESS;
}
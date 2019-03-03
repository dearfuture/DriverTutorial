#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#ifdef _X86_
#include "driver.h"
#else
#include "driver64.h"
#endif
#pragma comment(lib,"shlwapi.lib")

//用winhex将驱动转成字节数组就可以
int ReleaseFile(TCHAR FileName[])
{
	HANDLE hFile = CreateFile(FileName,
		GENERIC_ALL,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("文件创建失败%x\n", GetLastError());

		return -1;
	}

	DWORD dwret;
	BOOL bret = WriteFile(hFile, driverdata, sizeof(driverdata), &dwret, NULL);
	if (!bret)
	{
		printf("释放文件失败%x\n", GetLastError());
		CloseHandle(hFile);
		return -2;
	}

	CloseHandle(hFile);

	return 0;
}


VOID DriverInstall1()
{

	ReleaseFile(TEXT("C:\\windows\\system32\\drivers\\loadfirst.sys"));

	SC_HANDLE hSCmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService;
	DWORD dwTag = 1;
	hService = CreateService(hSCmanager,
		TEXT("loadfirst"),
		TEXT("loadfirst"),
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_BOOT_START,
		SERVICE_ERROR_IGNORE,
		TEXT("System32\\Drivers\\loadfirst.sys"),
		TEXT("System Reserved"),
		&dwTag, NULL, NULL, NULL
		);
// 	if (hService != NULL)
// 	{
// 		StartService(hService,NULL,NULL);
// 	}
	CloseServiceHandle(hSCmanager);
	CloseServiceHandle(hService);


}

VOID DriverInstall2()
{

	
	ReleaseFile(TEXT("C:\\windows\\system32\\driver\\loadfirst.sys"));

	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\loadfirst
	DWORD dwStart = SERVICE_BOOT_START;
	DWORD dwType = SERVICE_KERNEL_DRIVER;
	DWORD dwTag = 1;
	SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\loadfirst"), TEXT("ImagePath"), REG_EXPAND_SZ, TEXT("System32\\Drivers\\loadfirst.sys"), (lstrlen(TEXT("System32\\Driver\\loadfirst.sys")) + 1)*sizeof(TCHAR));
	SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\loadfirst"), TEXT("Group"), REG_EXPAND_SZ, TEXT("System Reserved"), (lstrlen(TEXT("System Reserved")) + 1)*sizeof(TCHAR));
	SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\loadfirst"), TEXT("Start"), REG_DWORD, &dwStart, sizeof(dwStart));
	SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\loadfirst"), TEXT("Type"), REG_DWORD, &dwType, sizeof(dwType));

}

int main()
{

	DriverInstall1();
	//DriverInstall2();
	return 0;
}
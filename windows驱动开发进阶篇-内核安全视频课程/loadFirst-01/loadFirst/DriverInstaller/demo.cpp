#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

VOID DriverInstall1()
{
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
		TEXT("System32\\Drivers\\demo.sys"),
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

	//DriverInstall1();
	DriverInstall2();
	return 0;
}
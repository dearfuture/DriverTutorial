#include <windows.h>
#include <stdio.h>

int main()
{
	SC_HANDLE hScmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hScmanager == NULL)
	{
		printf("openScmanager ´íÎó%d\n", GetLastError());
		return -1;
	}
	SC_HANDLE hService = CreateService(hScmanager,
		TEXT("mydriver"),
		TEXT("mydriver"),
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		TEXT("C:\\Users\\Administrator\\Desktop\\test.sys"),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
		);

	if (hService == NULL)
	{
		if (GetLastError() == ERROR_SERVICE_EXISTS)
		{
			hService = OpenService(hScmanager, TEXT("mydriver"), SERVICE_ALL_ACCESS);
			if (hService == NULL)
			{
				return -1;
			}
		}
		else
		{
			printf("´ò¿ª´íÎó%d;\n", GetLastError());
			CloseServiceHandle(hScmanager);
		}

	}

	if (!StartService(hService, NULL, NULL))
	{
		printf("Æô¶¯Çý¶¯Ê§°Ü%d\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hScmanager);
		return -3;
	
	}
	

	getchar();

	SERVICE_STATUS Status;
	ControlService(hService, SERVICE_CONTROL_STOP, &Status);
	DeleteService(hService);

	CloseServiceHandle(hService);
	CloseServiceHandle(hScmanager);

	return 0;
}
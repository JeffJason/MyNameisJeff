#include <iostream>
#include <direct.h>
#include <Windows.h>

using namespace std;

bool IsOnlyInstance(LPCTSTR gameTitle)
{
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd)
		{
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}
	cout << "The Game is running." << endl;
	return true;
}

bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;
	_getdiskfree(drive, &diskfree);
	unsigned __int64 const neededClusters = diskSpaceNeeded / (diskfree.sectors_per_cluster*diskfree.bytes_per_sector);
	if (diskfree.avail_clusters < neededClusters)
	{
		// if you get here you don’t have enough disk space!
		cout << "CheckStorage Failure : Not enough physical storage" << endl;
		return false;
	}
	cout << "It has enough physical storage." << endl;
	return true;
}

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded) {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	if (status.ullTotalPhys < physicalRAMNeeded) {
		//	 you don’t have enough physical memory. Tell the
		//	player to go get a real computer and give this one to
		//	his mother.
		cout << "CheckMemory Failure : Not enough physical memory." << endl;
		return false;
	}
	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded) {
		// you don’t have enough virtual memory available.
		// Tell the player to shut down the copy of Visual 
		//Studio running in the background.
		cout << "CheckMemory Failure : Not enough virtual memory." << endl;
		return false;
	}

	cout << "Physical Memory: " << status.ullTotalPhys / 1000000 << "MB" << endl;
	cout << "Virtual Memory: " << status.ullTotalVirtual / 1000000 << "MB" << endl;
}

DWORD ReadCPUSpeed() {
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;
	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, reinterpret_cast<LPCTSTR>("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		// query the key:
		RegQueryValueEx(hKey, reinterpret_cast<LPCTSTR>("MHz"), NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}
	return dwMHz;
}


int main()
{
	IsOnlyInstance("GAME");
	CheckStorage(300);
	CheckMemory(1, 1);
	cout << "CPUSPEED: " << ReadCPUSpeed() << endl;
	system("pause");
	return 0;
}
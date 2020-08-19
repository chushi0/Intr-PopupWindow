#include <Windows.h>
#include "../dbms/dbms.h"
#include <tlhelp32.h>

BOOL callback(HWND hwnd, LPARAM lParam) {
	if (IsWindowVisible(hwnd)) {
		auto intrConfigs = (std::vector<IntrConfig>*) lParam;

		WCHAR szBuf[512];
		DWORD processId;
		std::wstring windowTitle, windowClass, processName;
		// 窗口标题
		GetWindowTextW(hwnd, szBuf, 512);
		windowTitle = szBuf;
		// 窗口类
		GetClassNameW(hwnd, szBuf, 512);
		windowClass = szBuf;
		// pid
		GetWindowThreadProcessId(hwnd, &processId);
		// 进程名
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
		PROCESSENTRY32W pe = { sizeof(pe) };
		BOOL ret = Process32FirstW(hSnapshot, &pe);
		while (ret) {
			if (pe.th32ProcessID == processId) {
				CloseHandle(hSnapshot);
				processName = pe.szExeFile;
				break;
			}
			ret = Process32NextW(hSnapshot, &pe);
		}

		// 依次验证
		for (auto& config : *intrConfigs) {
			bool titleEqual = windowTitle == config.windowTitle;
			bool classEqual = windowClass == config.windowClass;
			if (config.type == 3 && titleEqual) goto hide;
			if (config.type == 4 && classEqual) goto hide;
			if (processName == config.process) {
				if (config.type == 5) goto hide;
				if (config.type == 1 && titleEqual) goto hide;
				if (config.type == 2 && classEqual) goto hide;
			}
			continue;

		hide:
			ShowWindow(hwnd, SW_HIDE);
			continue;
		}
	}
	return TRUE;
}

int main() {
	HANDLE hMutex = CreateMutexW(NULL, FALSE, L"IntrPopupwindowDeamon");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return 0;
	}

	while (true) {
		auto list = readAllConfig();
		EnumWindows(callback, (LPARAM)&list);
		Sleep(500);
	}
}

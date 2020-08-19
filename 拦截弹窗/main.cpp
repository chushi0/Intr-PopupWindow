#include <Windows.h>
#include <iostream>

inline void openProcess(LPCWSTR process) {
	STARTUPINFO startupInfo = {};
	PROCESS_INFORMATION processInfo = {};
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = TRUE;
	BOOL result = CreateProcessW(process, NULL, NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &startupInfo, &processInfo);
	if (!result) {
		LPVOID lpBuf;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, (LPWSTR)&lpBuf, 0, NULL);
		MessageBoxW(NULL, (LPWSTR)lpBuf, L"错误", MB_ICONERROR | MB_OK);
		LocalFree(lpBuf);
	}
}

inline void startDeamon() {
	openProcess(L"deamon.exe");
}

inline void startMainMenu() {
	openProcess(L"settings.exe");
}

int main(int argc, char** argv) {

	int size = MultiByteToWideChar(CP_ACP, 0, argv[0], -1, NULL, 0);
	wchar_t* path = new wchar_t[size];
	MultiByteToWideChar(CP_ACP, 0, argv[0], -1, path, size);
	std::wstring curDir = path;
	int index = curDir.find_last_of(L'\\');
	curDir = curDir.substr(0, index);
	SetCurrentDirectory(curDir.c_str());

	bool bootCause = false;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "/b")) {
			bootCause = true;
		}
	}

	startDeamon();
	if (!bootCause) {
		startMainMenu();
	}

	return 0;
}

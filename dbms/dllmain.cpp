// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

extern void openDatabase();
extern void closeDatabase();

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		openDatabase();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		closeDatabase();
		break;
	}
	return TRUE;
}


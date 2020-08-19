#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

#ifdef DBMS_EXPORTS
#define DBMSAPI __declspec(dllexport)
#else
#define DBMSAPI __declspec(dllimport)
#endif
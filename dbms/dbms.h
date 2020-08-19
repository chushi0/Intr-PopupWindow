#pragma once
#include "framework.h"
#include <string>
#include <vector>

typedef DBMSAPI struct
{
	int type;
	std::wstring windowTitle;
	std::wstring windowClass;
	std::wstring process;
} IntrConfig, *LpIntrConfig;

DBMSAPI bool insertNewConfig(LpIntrConfig config);

DBMSAPI bool removeConfig(LpIntrConfig config);

DBMSAPI std::vector<IntrConfig> readAllConfig();
#pragma once

#include "../../includes.h"

class GameVirtual
{
public:
	static bool isVmWare();
	static std::string getVMname();

	static bool checkVirtualMachine();
};
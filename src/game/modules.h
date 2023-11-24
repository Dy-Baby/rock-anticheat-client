#pragma once

#include "../../includes.h"

class Modules
{
public:
	static bool inspectModules(DWORD pid);
	static long verifyMicrosoftSignature(LPCSTR path);
	static bool verifyPublisher(LPCSTR path);
	static std::string getPublisher(LPCSTR path);

	static void thread();
};
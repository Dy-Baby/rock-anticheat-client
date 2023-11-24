#pragma once

#include "../../includes.h"

struct SF_PluginsIterator;

class SampFuncs {
public:
	static bool isDisabled;
	static std::vector<std::string> plugins;

	static bool iterPlugins();

	static HMODULE getSFModule();
	static bool isSAMPinitialized();

	static bool disableConsole();

	static void thread();
};
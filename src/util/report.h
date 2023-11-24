#pragma once

#include "../../includes.h"

class Report
{
public:
	struct MemoryInfo {
		std::string base_address;
		std::string region_size;
		std::string characteristics;
		std::string name;
		std::string hash;
	};

	struct ModuleInfo {
		std::string name;
		std::string path;
		std::string hash;
		std::string lastChange;
		std::string permission;
		std::string base_address;
		std::string publisher;
		std::string size;
	};

	struct WindowInfo {
		std::string title;
		std::string hwnd;
		std::string visible;
		std::string description;
		std::string path;
		std::string hash;
		std::string lastChange;
		std::string permission;
		std::string size;
	};

	struct CleoInfo {
		std::string filename;
		std::string path;
		std::string hash;
		std::string lastChange;
		std::string size;
		std::string permissions;
	};

	struct VMInfo {
		std::string name;
	};

	struct SFInfo {
		std::string filename;
		std::string path;
		std::string hash;
		std::string lastChange;
		std::string size;
		std::string permissions;
	};

	struct DebuggerInfo {
		std::string method;
	};

	struct ErrorInfo {
		std::string error;
		std::string call;
		std::string ret;
		std::string wh;
	};

	static bool sendMemoryReport(MemoryInfo *data);
	static bool sendModuleReport(ModuleInfo *data);
	static bool sendWindowReport(WindowInfo *data);
	static bool sendCleoReport(CleoInfo *data);
	static bool sendVMReport(VMInfo *data);
	static bool sendSFReport(SFInfo* data);
	static bool sendDebuggerReport(DebuggerInfo* data);
	static bool sendErrorReport(ErrorInfo* error);
};
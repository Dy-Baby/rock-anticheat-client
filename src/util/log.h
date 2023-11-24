#pragma once

#include "../../includes.h"

class Log
{
public:
	static void openLog();
	static void write(const char* fmt, ...);
	static void closeLog();
};
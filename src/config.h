#pragma once

#include "../includes.h"

#include "util/network.h"

class Config
{
public:
	static std::string URL_REPOSITORY;
	static std::string AES_KEY;
	static std::string AES_IV;
	static std::string API_KEY;
	
	// TIMING
	static DWORD MEMORY_CHECK_SLEEP;
	static DWORD WINDOW_CHECK_SLEEP;
	static DWORD MODULES_CHECK_SLEEP;
	static DWORD SAMPFUNCS_CHECK_SLEEP;
	static DWORD DEBUGGER_CHECK_SLEEP;

	static DWORD LONGPOOL_GET_SLEEP;
	static DWORD ACTIVITY_UPDATE_SLEEP;

};
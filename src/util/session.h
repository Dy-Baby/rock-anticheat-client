#pragma once

#include "../../includes.h"

class Session
{
private:
	static std::string uniqueSesionID;
	static bool cheatAllowed;

public:
	static bool sessionStart();
	static std::string getSessionID();
	static void updateActivity();

	static void thread();
};
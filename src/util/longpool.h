#pragma once

#include "../../includes.h"

class LongPool
{
private:
	static std::string lastReceive;
	static int try_count;

public:
	static void handleData();
	static bool getData();
	static void thread();
};
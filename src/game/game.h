#pragma once

#include "../../includes.h"

class Game
{
public:
	static bool sendServiceMessage(int mode, const char* msg);
	static bool TerminateGame(int mode, std::string info);
	static unsigned int getGameState();
};
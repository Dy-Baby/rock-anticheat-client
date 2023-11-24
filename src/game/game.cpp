#include "game.h"

bool Game::sendServiceMessage(int mode, const char* msg)
{
	HANDLE hPipe;
	DWORD dwWritten;

	hPipe = LI_FN(CreateFileW).get()(XorStrW(TEXT("\\\\.\\pipe\\rockAnticheat")),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		char buf[256];
		switch (mode)
		{
			case 0:
				sprintf(buf, XorStr("detect^%s"), msg);
				break;

			case 1:
				sprintf(buf, XorStr("error^%s"), msg);
				break;
		}
		
		LI_FN(WriteFile).get()(hPipe,
			buf,
			strlen(buf),   
			&dwWritten,
			NULL);

		LI_FN(CloseHandle).get()(hPipe);
		return true;
	}
	
	return false;
}

bool Game::TerminateGame(int mode, std::string info)
{
	bool res = Game::sendServiceMessage(mode, info.c_str());
	
	// �������� samp.DLL �� ������ ���� ���� �� ���������.
	HMODULE samp_dll = LI_FN(GetModuleHandleW).get()(XorStrW(L"samp.dll"));
	if (samp_dll != NULL)
	{
		LI_FN(FreeLibrary).get()(samp_dll);
	}

	Log::closeLog();

	const auto explorer = LI_FN(OpenProcess).get()(PROCESS_TERMINATE, false, Core::getProcessID(XorStrW(L"gta_sa.exe")));
	LI_FN(TerminateProcess).get()(explorer, 1);
	LI_FN(CloseHandle).get()(explorer);

	exit(1);
}

unsigned int Game::getGameState()
{
	return *reinterpret_cast<unsigned int*>(0xC8D4C0);
}
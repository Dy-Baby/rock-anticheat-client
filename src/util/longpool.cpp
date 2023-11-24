#include "longpool.h"

std::string LongPool::lastReceive = "";

void LongPool::handleData()
{
	if (!(LongPool::lastReceive.empty()))
	{
		std::string str = LongPool::lastReceive;

		if (str == XorStr("0x000001a"))
			Log::write(XorStr("[ERROR] Called error 0x000001a (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000001b"))
			Log::write(XorStr("[ERROR] Called error 0x000001b (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000002a"))
			Log::write(XorStr("[ERROR] Called error 0x000002a (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000002b"))
			Log::write(XorStr("[ERROR] Called error 0x000002b (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000002c"))
			Log::write(XorStr("[ERROR] Called error 0x000002c (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000003a"))
			Log::write(XorStr("[ERROR] Called error 0x000003a (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000004a"))
			Log::write(XorStr("[ERROR] Called error 0x000004a (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000004b"))
			Log::write(XorStr("[ERROR] Called error 0x000004b (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x000005a"))
			Log::write(XorStr("[ERROR] Called error 0x000005a (LongPool::handleData())"));
			Game::TerminateGame(0, str);

		if (str == XorStr("0x0001488"))
			Log::write(XorStr("[ERROR] Called error 0x0001488 - User banned (LongPool::handleData())"));
			Game::TerminateGame(1, XorStr("\n\n�� �������� ���������� ���������� �� Rock Anticheat.\n\n������� ���������� ����������������� �������� ����, ����� ��������� ��� ��������� ��������� �� ������������ � �� ���� ������� ������������.\n���� ������� ��������� �������� ���������, �� ��� ������� ����� �������� ������������ ����������.\n\n"));
	}
}

bool LongPool::getData()
{
	std::string url = Config::URL_REPOSITORY;
	url = url + XorStr("/sessions/") + Session::getSessionID() + XorStr("/");
	std::string tmp = Network::string_sendRequest(url.c_str());

	if (tmp == "SESNF")
	{
		Log::write(XorStr("[ERROR] Called error 0x1000016 - Session not found (LongPool::getData())"));
		if (!Session::sessionStart())
		{
			Log::write(XorStr("[ERROR] Called error: 0x1000010 (Session::sessionStart()) lastNetworkError: %ld"), Network::lastError);
			Game::TerminateGame(1, XorStr("0x1000010"));
		}
	}
	else
	{
		LongPool::lastReceive = tmp;
	}

	return true;
}

void LongPool::thread()
{
	Log::write(XorStr("LongPool::thread() started"));
	DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
	do
	{
		LI_FN(Sleep).get()(Config::LONGPOOL_GET_SLEEP);
		if (LongPool::getData())
		{
			LongPool::handleData();
		}
	} while (pid != 0);
}
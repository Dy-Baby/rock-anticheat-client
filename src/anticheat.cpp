#include "anticheat.h"

void AntiCheat::Load()
{
	/* FOR DEBUGGING */
	//LI_FN(AllocConsole).get()();
	//freopen("CONOUT$", "w", stdout);

	Log::openLog();
	Log::write(XorStr("Log::openLog() success"));

	Core::loadDefaultLibs();
	Log::write(XorStr("Core::loadDefaultLibs() success"));

	DWORD n_init = Network::init();
	if (n_init != 0)
	{
		Log::write(XorStr("[ERROR] Called error 0x1000013 (Network::Init())\n\tGetLastError: %ld\n"), n_init);
		Game::TerminateGame(1, XorStr("0x1000013"));
	}
	Log::write(XorStr("Network::Init() success"));

	const auto explorer = LI_FN(OpenProcess).get()(PROCESS_TERMINATE, false, Core::getProcessID(XorStrW(L"rockACService.exe")));
	Log::write(XorStr("OpenProcess() success"));

	LI_FN(TerminateProcess).get()(explorer, 1);
	Log::write(XorStr("TerminateProcess() success"));

	LI_FN(CloseHandle).get()(explorer);
	Log::write(XorStr("CloseHandle() success"));

	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;

	std::wstring path = Core::UTF8ToUTF16(Core::getCurrentDirectory() + XorStr("/rockACService.exe"));
	std::wstring cmd = XorStrW(L"");

	if (!LI_FN(CreateProcessW).get()(path.c_str(), &cmd[0], nullptr, nullptr, false, DETACHED_PROCESS, nullptr, nullptr, &info, &processInfo))
	{
		Log::write(XorStr("[ERROR] Called error: 0x1000012 (CreateProcessW())"));
		Game::TerminateGame(1, XorStr("0x1000012"));
	}
	Log::write(XorStr("CreateProcessW() success"));

	if (!Hardware::getMacAddress())
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000001 (Hardware::getMacAddress())"));
		Game::TerminateGame(1, XorStr("0x7000001"));
	}
	Log::write(XorStr("Hardware::getMacAddress() success"));

	if (!Hardware::getDriveGUID())
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000002 (Hardware::getDriveGUID())"));
		Game::TerminateGame(1, XorStr("0x7000002"));
	}
	Log::write(XorStr("Hardware::getDriveGUID() success"));

	if (!Hardware::getCPUHash())
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000003 (Hardware::getCPUHash())"));
		Game::TerminateGame(1, XorStr("0x7000003"));
	}
	Log::write(XorStr("Hardware::getCPUHash() success"));

	if (!Hardware::getPCName())
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000004 (Hardware::getPCName())"));
		Game::TerminateGame(1, XorStr("0x7000004"));
	}
	Log::write(XorStr("Hardware::getPCName() success"));

	if (!Hardware::getUserGUID()) 
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000005 (Hardware::getUserGUID())"));
		Game::TerminateGame(1, XorStr("0x7000005"));
	}
	Log::write(XorStr("Hardware::getUserGUID() success"));

	if (!Hardware::getMotherboardSerial())
	{
		Log::write(XorStr("[ERROR] Called error: 0x7000006 (Hardware::getMotherboardSerial())"));
		Game::TerminateGame(1, XorStr("0x7000006"));
	}
	Log::write(XorStr("Hardware::getMotherboardSerial() success"));

	if (!Session::sessionStart())
	{
		Log::write(XorStr("[ERROR] Called error: 0x1000010 (Session::sessionStart()) lastNetworkError: %ld"), Network::lastError);
		Game::TerminateGame(1, XorStr("0x1000010"));
	}
	Log::write(XorStr("Session::sessionStart() success"));

	// print info about pc
	std::string pc_info;
	pc_info = XorStr("PC Information:");
	pc_info = pc_info + XorStr("\nGUID: ") + Hardware::user_guid;
	pc_info = pc_info + XorStr("\nPC Name: ") + Hardware::pc_name;
	pc_info = pc_info + XorStr("\n\nDrive HWID:\n");
	for (std::string hwid : Hardware::guid_drives)
		pc_info = pc_info + XorStr("\t- ") + hwid + XorStr("\n");

	pc_info = pc_info + XorStr("\n\nMac Addresses:\n");
	for (std::string mac : Hardware::mac_address)
		pc_info = pc_info + XorStr("\t- ") + mac + XorStr("\n");

	pc_info = pc_info + XorStr("\n\nMotherboard Serial: ") + Hardware::motherboard_serial + XorStr("\n");
	pc_info = pc_info + XorStr("--------------\n\n");

	Log::write(XorStr("%s"), pc_info.c_str());

	std::thread debuggerThread(AntiDebugger::thread);
	debuggerThread.detach();


	std::thread sampfuncsThread(SampFuncs::thread);
	sampfuncsThread.detach();

	// activity thread
	std::thread activityThread(Session::thread);
	activityThread.detach();

	GameVirtual::checkVirtualMachine();

	// check Memory thread
	std::thread checkMemory(GameMemory::thread);
	checkMemory.detach();

	// check window list
	std::thread windowThread(GameWindow::check);
	windowThread.detach();

	// check cleo folder
	GameCleo::checkCleo();

	std::thread checkModule(Modules::thread);
	checkModule.detach();

	// longpool thread
	std::thread longpoolThread(LongPool::thread);
	longpoolThread.detach();
}

void AntiCheat::Unload()
{
	Log::closeLog();
}
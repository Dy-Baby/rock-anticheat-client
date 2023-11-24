#include "sampfuncs.h"

bool SampFuncs::isDisabled = false;
std::vector<std::string> SampFuncs::plugins;

bool SampFuncs::iterPlugins()
{
	if(!SampFuncs::plugins.empty())
		SampFuncs::plugins.clear();

	HMODULE sf = SampFuncs::getSFModule();
	if (sf)
	{
		if (SampFuncs::isSAMPinitialized())
		{

			char path[MAX_PATH];
			SF_PluginsIterator* it = nullptr;

			typedef SF_PluginsIterator* (WINAPI* sf_GetNextLoadedPlugin)(SF_PluginsIterator* it, char* pathBuf, size_t bufSize);
			sf_GetNextLoadedPlugin SF_GetNextLoadedPlugin = (sf_GetNextLoadedPlugin)LI_FN(GetProcAddress).get()(sf, XorStr("SF_GetNextLoadedPlugin"));

			while (it = SF_GetNextLoadedPlugin(it, path, sizeof(path))) {
				SampFuncs::plugins.push_back(path);
			}

			return true;
		}
	}

	return false;
}

bool SampFuncs::isSAMPinitialized()
{
	std::string line;
	std::ifstream myfile((Core::getCurrentDirectory()+XorStr("/moonloader/moonloader.log")).c_str());
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			size_t pos = line.find(XorStr("Hooks installed."));
			if (pos++ != line.npos)
			{
				HMODULE sf = SampFuncs::getSFModule();
				if (sf)
				{
					typedef bool (WINAPI* sampInit)(void);
					sampInit sampInitialized = (sampInit)LI_FN(GetProcAddress).get()(sf, XorStr("?IsInitialized@SFSAMP@@QAE_NXZ"));

					return sampInitialized();
				}
			}
		}
		myfile.close();
	}
	return false;
}

HMODULE SampFuncs::getSFModule()
{
	HMODULE sf = 0;
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
	hProcess = LI_FN(OpenProcess).get()(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, pid);

	if (LI_FN(K32EnumProcessModules).get()(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			if (LI_FN(K32GetModuleBaseNameW).get()(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				std::wstring wszModName(szModName);
				std::string injectedmodule(wszModName.begin(), wszModName.end());

				if (injectedmodule == std::string(XorStr("SAMPFUNCS.asi")))
				{
					sf = hMods[i];
				}
			}
		}
	}

	return sf;
}

bool SampFuncs::disableConsole()
{
	if (SampFuncs::isDisabled) return false;

	HMODULE sf = SampFuncs::getSFModule();
	if (sf)
	{
		typedef void(WINAPI* disableConsole)();

		disableConsole DisableConsole = (disableConsole)LI_FN(GetProcAddress).get()(sf, XorStr("SF_DisableConsole"));

		DisableConsole();
		SampFuncs::isDisabled = true;

		return true;
	}

	return false;
}

void SampFuncs::thread()
{
	Log::write(XorStr("SampFuncs::thread() started"));
	DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
	do
	{
		pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
		LI_FN(Sleep).get()(500);
	} while (!Game::getGameState() == 9 && pid == 0);
	
	do
	{
		LI_FN(Sleep).get()(500);
	} while (!SampFuncs::isSAMPinitialized());

	Log::write(XorStr("Try to SampFuncs::disableConsole()"));
	SampFuncs::disableConsole();
	
	Log::write(XorStr("SampFuncs::check() started"));
	if (SampFuncs::isSAMPinitialized())
	{
		Sleep(Config::SAMPFUNCS_CHECK_SLEEP);
		SampFuncs::iterPlugins();

		if (!SampFuncs::plugins.empty())
		{
			for (std::string plugin : SampFuncs::plugins)
			{
				// get filename
				std::string filename;
				size_t pos = plugin.find(XorStr("\\SAMPFUNCS\\"));
				if (pos++ != plugin.npos)
				{
					filename = plugin.substr(pos);
				}

				// report
				Report::SFInfo info;
				info.filename = filename;
				info.path = plugin;
				info.hash = Core::getFileMD5(plugin);
				info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(plugin));
				info.size = std::to_string(std::filesystem::file_size(plugin));
				info.permissions = Core::getFilePermissions(std::filesystem::status(plugin).permissions());
				Report::sendSFReport(&info);
			}

			Log::write(XorStr("[ERROR] Called error 0x1000008 (SampFuncs::plugins.empty())"));
			Game::TerminateGame(0, XorStr("0x1000008"));
		}
	}
}
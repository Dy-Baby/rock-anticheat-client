#include "memory_pattern.h"

std::vector<std::string> forbiddenBytePatterns;
std::vector<std::string> forbiddenRegex;
std::vector<std::string> forbiddenNames;

bool Pattern::findProcessPatterns()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!LI_FN(K32EnumProcesses).get()(aProcesses, sizeof(aProcesses), &cbNeeded)) return false;

    cProcesses = cbNeeded / sizeof(DWORD);
    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            HANDLE hProcess = LI_FN(OpenProcess).get()(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
            if (NULL != hProcess)
            {
                TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
                HMODULE hMod;
                DWORD cbNeeded;

                if (LI_FN(K32EnumProcessModules).get()(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    LI_FN(K32GetModuleBaseNameW).get()(hProcess, hMod, szProcessName,
                        sizeof(szProcessName) / sizeof(TCHAR));
                }

                std::wstring wszModName(szProcessName);
                std::string injectedmodule(wszModName.begin(), wszModName.end());

                MODULEENTRY32W client = Core::get_module(injectedmodule.c_str(), aProcesses[i]);

                std::string bytes(client.modBaseSize, ' ');
                LI_FN(ReadProcessMemory).get()(hProcess, (void*)client.modBaseAddr, (void*)bytes.data(), client.modBaseSize, nullptr);

                for (std::string pattern : forbiddenBytePatterns)
                {
                    int index = 0;
                    auto it = std::find(forbiddenBytePatterns.begin(), forbiddenBytePatterns.end(), pattern);
                    if (it != forbiddenBytePatterns.end())
                    {
                        index = it - forbiddenBytePatterns.begin();
                    }
                    else break;

                    uintptr_t addr = Core::find_pattern(hProcess, client, bytes, pattern.c_str(), 0x1, 0x0, true);

                    if (addr != 0x0)
                    {
                        const std::regex r = std::regex(std::regex_replace(forbiddenRegex[index], std::regex("\\s+"), ".*"));

                        std::smatch m;
                        if (std::regex_search(bytes, m, r))
                        {
                            LI_FN(EnumWindows).get()(Core::EnumWindowsProcMy, aProcesses[i]);
                            std::wostringstream ss;
                            ss << std::hex << AntiCheat::gtaHwnd;
                            std::wstring strTitle = ss.str();

                            wchar_t* filename = new wchar_t[MAX_PATH];
                            HANDLE prc = LI_FN(OpenProcess).get()(PROCESS_ALL_ACCESS, NULL, aProcesses[i]);
                            LI_FN(K32GetModuleFileNameExW).get()(prc, NULL, filename, MAX_PATH);

                            // report
                            Report::WindowInfo info;

                            info.hwnd = Core::utf16ToUTF8(strTitle);
                            info.title = Core::HWNDToString(AntiCheat::gtaHwnd);
                            info.visible = LI_FN(IsWindowVisible).get()(AntiCheat::gtaHwnd) ? XorStr("true") : XorStr("false");
                            info.description = forbiddenNames[index];
                            info.path = Core::utf16ToUTF8(filename);

                            Report::sendWindowReport(&info);

                            const auto explorer = LI_FN(OpenProcess).get()(PROCESS_TERMINATE, false, aProcesses[i]);
                            LI_FN(TerminateProcess).get()(explorer, 1);
                            LI_FN(CloseHandle).get()(explorer);
                        }
                    }
                    bytes.~basic_string();
                }

            }
        }
    }

}

void Pattern::thread()
{
    Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/finder/patterns.php")).c_str(), forbiddenBytePatterns);
    Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/finder/regex.php")).c_str(), forbiddenRegex);
    Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/finder/names.php")).c_str(), forbiddenNames);

    DWORD pID = Core::getProcessID(XorStrW(L"gta_sa.exe"));
    do
    {
        Pattern::findProcessPatterns();

        LI_FN(Sleep).get()(Config::MEMORY_PATTERN_CHECK_SLEEP);
    } while (pID != 0);
}
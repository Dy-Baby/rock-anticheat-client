#include "window_check.h"

std::vector<std::string> knownWindowsRegex = 
{
    XorStr(".*([iI]nject(or|ion))+"),
    XorStr(".*(\b[aA]im\b)+"),
    XorStr(".*(\b[cC]heat+[s]*)\b")

};

std::vector<std::string> knownWindowsDescription =
{
    XorStr("Injector"),
    XorStr("AIM"),
    XorStr("Cheat")
};

static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam) { 
    int length = LI_FN(GetWindowTextLengthW).get()(hWnd);
    wchar_t* buffer = new wchar_t[length + 1];

    DWORD pid;
    LI_FN(GetWindowThreadProcessId).get()(hWnd, &pid);

    LI_FN(GetWindowTextW).get()(hWnd, buffer, length + 1);

    std::wstring ws(buffer);
    std::string windowTitle(ws.begin(), ws.end());
        
    if (length != 0) {
        std::stringstream stringstreamBuffer;
        stringstreamBuffer << hWnd;
        std::string strhwndHex = stringstreamBuffer.str();
        
        for (std::string regex : knownWindowsRegex)
        {
            int index = 0;
            auto it = std::find(knownWindowsRegex.begin(), knownWindowsRegex.end(), regex);
            if (it != knownWindowsRegex.end())
            {
                index = it - knownWindowsRegex.begin();
            }
            else break;

            wchar_t* filename = new wchar_t[MAX_PATH];
            HANDLE prc = LI_FN(OpenProcess).get()(PROCESS_ALL_ACCESS, NULL, pid);
            LI_FN(K32GetModuleFileNameExW).get()(prc, NULL, filename, MAX_PATH);

            const std::regex r = std::regex(std::regex_replace(knownWindowsRegex[index], std::regex("\\s+"), ".*"));

            std::smatch m;
            if (std::regex_search(windowTitle, m, r))
            {
                Report::WindowInfo info;
                info.title = windowTitle;
                info.hwnd = strhwndHex;
                info.visible = LI_FN(IsWindowVisible).get()(hWnd) ? XorStr("true") : XorStr("false");
                info.description = knownWindowsDescription[index];
                info.path = Core::UTF16ToUTF8(filename);

                Report::sendWindowReport(&info);
            }
        }

    }
    return TRUE;
}

int GameWindow::check() {
    Log::write(XorStr("GameWindow::check() started"));

    DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
    do
    {
        LI_FN(EnumWindows).get()(enumWindowCallback, NULL);
        LI_FN(Sleep).get()(Config::WINDOW_CHECK_SLEEP);
    } while (pid != 0);

    return 0;
}
#include "memory_integrity.h"

std::vector<GameMemory::Memory> GameMemory::cachedSections = {};

PIMAGE_DOS_HEADER GameMemory::get_dos_header(std::uintptr_t module)
{
    return reinterpret_cast<PIMAGE_DOS_HEADER>(module);
}

PIMAGE_NT_HEADERS GameMemory::get_nt_headers(std::uintptr_t module, const PIMAGE_DOS_HEADER dos_header)
{
    return reinterpret_cast<PIMAGE_NT_HEADERS>(module + dos_header->e_lfanew);
}

std::uintptr_t GameMemory::get_address_from_va(std::uintptr_t module, std::uintptr_t virtual_address)
{
    return (module + virtual_address);
}

HMODULE GameMemory::get_module_handle(LPCWSTR module)
{
    return GetModuleHandle(module);
}

std::uintptr_t GameMemory::get_module_handle_as(LPCWSTR module)
{
    return reinterpret_cast<std::uintptr_t>(get_module_handle(module));
}

std::uint32_t GameMemory::getCRC32(std::uintptr_t data, std::size_t size)
{
    std::uint32_t result = {};

    for (std::size_t index = {}; index < size; ++index)
    {
        char tmp = reinterpret_cast<char*>(data)[index];

        result = CRC::Calculate(&tmp, sizeof(tmp), CRC::CRC_32());
    }
    return result;
}

std::vector<GameMemory::Memory> GameMemory::extractSections(std::uintptr_t module)
{
    std::vector<GameMemory::Memory> result = {};

    PIMAGE_NT_HEADERS nt_headers = get_nt_headers(module, get_dos_header(module));

    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt_headers);

    for (std::uint16_t index = {}; index < nt_headers->FileHeader.NumberOfSections; index++, section++)
    {
        if ((section->Characteristics & IMAGE_SCN_MEM_WRITE) == false)
        {
            if (std::uintptr_t address = get_address_from_va(module, section->VirtualAddress))
            {
                result.push_back({
                    section->Name,
                    address,
                    section->Misc.VirtualSize,
                    section->Characteristics,
                    getCRC32(address, section->Misc.VirtualSize)
                    });
            }
        }
    }


    return result;
}

std::vector<GameMemory::Memory> GameMemory::compareSections(std::vector<Memory>& sections)
{
    std::vector<GameMemory::Memory> result = {};
    GameMemory::Memory tmp;

    for (std::size_t index = {}; index < GameMemory::cachedSections.size(); index++)
        if (!(GameMemory::cachedSections[index] == sections[index]))
        {
            tmp.address = sections[index].address;
            tmp.characteristics = sections[index].characteristics;
            tmp.checksum = sections[index].checksum;
            tmp.name = sections[index].name;
            tmp.size = sections[index].size;

            result.push_back(tmp);
        }

    return result;
}

bool GameMemory::checkModule(LPCWSTR module)
{
    int* p = 0x00000000;
    *p = 13;
    if (GameMemory::cachedSections.empty())
    {
        std::uintptr_t mod = GameMemory::get_module_handle_as(module);

        GameMemory::cachedSections = GameMemory::extractSections(mod);
        
        return true;
    }
    else
    {
        std::uintptr_t mod = GameMemory::get_module_handle_as(module);
        
        std::vector <GameMemory::Memory> tempVec = GameMemory::extractSections(mod);
        std::vector <GameMemory::Memory> returnVec = GameMemory::compareSections(tempVec);

        if(returnVec.empty())
        {
            GameMemory::cachedSections = GameMemory::extractSections(mod);
            return true;
        }
        else 
        {
            // report
            for (std::size_t index = {}; index < returnVec.size(); index++)
            {
                Report::MemoryInfo info;
                info.base_address = std::to_string(returnVec[index].address);
                info.characteristics = std::to_string(returnVec[index].characteristics);
                info.region_size = std::to_string(returnVec[index].size);
                info.hash = std::to_string(returnVec[index].checksum);
                info.name = std::string((char*)(returnVec[index].name));

                Report::sendMemoryReport(&info);
            }

            GameMemory::cachedSections.clear();
            return true;
        }
    }

    return false;
}

void GameMemory::thread()
{
    std::vector<std::wstring> modules =
    {
        XorStrW(L"gta_sa.exe"),
        XorStrW(L"samp.dll"),
        XorStrW(L"audio.dll"),
        XorStrW(L"bass.dll")
    };


    DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
    do
    {
        for (std::wstring i : modules)
        {
            GameMemory::checkModule(i.c_str());
            LI_FN(Sleep).get()(Config::MEMORY_CHECK_SLEEP);
        }
    } while (pid != 0);
}
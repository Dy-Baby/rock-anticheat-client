#pragma once

#include "../../includes.h"

class GameMemory
{
public:
    struct Memory
    {
        std::uint8_t* name = {};
        std::uintptr_t address = {};
        std::size_t size = {};
        std::size_t characteristics = {};
        std::uint32_t checksum = {};

        const bool operator==(const Memory& other) const noexcept
        {
            return this->checksum == other.checksum;
        }
    };
    static std::vector<Memory> cachedSections;

    static void thread();

    static bool checkModule(LPCWSTR module);

    static std::vector<Memory> compareSections(std::vector<Memory>& sections);
    static std::vector<Memory> extractSections(std::uintptr_t module);
    static std::uint32_t getCRC32(std::uintptr_t data, std::size_t size);

    static PIMAGE_DOS_HEADER get_dos_header(std::uintptr_t module);
    static PIMAGE_NT_HEADERS get_nt_headers(std::uintptr_t module, const PIMAGE_DOS_HEADER dos_header);
    static std::uintptr_t get_address_from_va(std::uintptr_t module, std::uintptr_t virtual_address);

    static HMODULE get_module_handle(LPCWSTR module);
    static std::uintptr_t get_module_handle_as(LPCWSTR module);
};
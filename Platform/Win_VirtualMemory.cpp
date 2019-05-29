#include "_PreHeader.hpp"
#include "VirtualMemory.hpp"

using namespace StdLib;

namespace
{
    constexpr std::array<DWORD, 8> PageProtectionMapping =
    {
        0, // 0 - Unused
        0, // 1 - Write
        PAGE_READONLY, // 2 - Read
        PAGE_READWRITE, // 3 - Write + Read
        PAGE_EXECUTE, // 4 - Execute
        0, // 5 - Execute + Write
        PAGE_EXECUTE_READ, // 6 - Execute + Read
        PAGE_EXECUTE_READWRITE // 7 - Execute + Write + Read
    };

    uiw Static_PageSize{};
}

static constexpr DWORD PageModeToWinAPI(VirtualMemory::PageModes::PageMode pageMode);

void *VirtualMemory::Reserve(uiw size)
{
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}

Error<> VirtualMemory::Commit(void *memory, uiw size, PageModes::PageMode pageMode)
{
    ASSUME(memory && size);
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (VirtualAlloc(memory, size, MEM_COMMIT, protection))
    {
        return DefaultError::Ok();
    }
    return DefaultError::UnknownError("VirtualAlloc failed");
}

void *VirtualMemory::Alloc(uiw size, PageModes::PageMode pageMode)
{
    ASSUME(size);
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return nullptr;
    }
    return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, protection);
}

bool VirtualMemory::Free(void *memory, uiw memorySize)
{
    ASSUME(memory && memorySize);
    return VirtualFree(memory, 0, MEM_RELEASE) != 0;
}

auto VirtualMemory::PageMode(const void *memory, uiw size) -> Result<PageModes::PageMode>
{
    if ((size % PageSize()) != 0)
    {
        return DefaultError::InvalidArgument("size must be divisible by PageSize()");
    }

    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T informationSize = VirtualQuery(memory, &mbi, sizeof(mbi));
    if (!informationSize)
    {
        return DefaultError::UnknownError("VirtualQuery returned 0");
    }
    if (mbi.RegionSize < size)
    {
        return InconsistentProtection();
    }
    for (uiw index = 0; index < PageProtectionMapping.size(); ++index)
    {
        if (PageProtectionMapping[index] == mbi.Protect)
        {
            return PageModes::PageMode::Create((PageModes::PageMode::valueType)index);
        }
    }
    SOFTBREAK;
    return DefaultError::UnknownError("Encountered unknown WinAPI protection mode");
}

Error<> VirtualMemory::PageMode(void *memory, uiw size, PageModes::PageMode pageMode)
{
    ASSUME(memory && size);
    DWORD oldProtect;
    DWORD protection = PageModeToWinAPI(pageMode);
    if (!protection)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (VirtualProtect(memory, size, protection, &oldProtect))
    {
        return DefaultError::Ok();
    }
    return DefaultError::UnknownError("VirtualProtect failed");
}

uiw VirtualMemory::PageSize()
{
    ASSUME(Static_PageSize);
    return Static_PageSize;
}

constexpr DWORD PageModeToWinAPI(VirtualMemory::PageModes::PageMode pageMode)
{
    return PageProtectionMapping[pageMode.AsInteger()];
}

namespace StdLib::VirtualMemory
{
    void Initialize()
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        Static_PageSize = sysinfo.dwPageSize;
    }
}
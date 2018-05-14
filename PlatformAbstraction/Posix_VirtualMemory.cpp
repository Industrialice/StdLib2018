#include "VirtualMemory.hpp"
#include <sys/mman.h>

using namespace StdLib;

namespace
{
    constexpr std::array<int, 8> PageProtectionMapping =
    {
        -1,  //  0 - Unused
        -1,  //  1 - Write
        PROT_READ,  //  2 - Read
        PROT_WRITE | PROT_READ,  //  3 - Write + Read
        PROT_EXEC,  //  4 - Execute
        -1,  //  5 - Execute + Write
        PROT_EXEC | PROT_READ,  //  6 - Execute + Read
        PROT_EXEC | PROT_WRITE | PROT_READ  //  7 - Execute + Write + Read
    };
}

static constexpr int PageModeToPosix(VirtualMemory::PageMode pageMode);

void *VirtualMemory::Reserve(uiw size)
{
    ASSUME(size);
    return mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

Error<> VirtualMemory::Commit(void *memory, uiw size, PageMode pageMode)
{
    ASSUME(memory && size);
    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (mprotect(memory, size, protection) == 0)
    {
        return DefaultError::Ok();
    }
    return DefaultError::UnknownError("mprotect failed");
}

Result<void *> VirtualMemory::Alloc(uiw size, PageMode pageMode)
{
    ASSUME(size);
    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (void *memory = mmap(0, size, protection, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); memory)
    {
        return memory;
    }
    return DefaultError::UnknownError("mmap failed");
}

bool VirtualMemory::Free(void *memory, uiw memorySize)
{
    ASSUME(memory && memorySize);
    return munmap(memory, memorySize) == 0;
}

auto VirtualMemory::PageModeGet(const void *memory, uiw size) -> Result<PageMode>
{
    return DefaultError::Unsupported();
}

Error<> VirtualMemory::PageModeSet(void *memory, uiw size, PageMode pageMode)
{
    ASSUME(memory && size);
    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return DefaultError::InvalidArgument("Such page mode isn't permitted");
    }
    if (mprotect(memory, size, protection) == 0)
    {
        return DefaultError::Ok();
    }
    return DefaultError::UnknownError("mprotect failed");
}

uiw VirtualMemory::PageSize()
{
    return 4096;
}

constexpr int PageModeToPosix(VirtualMemory::PageMode pageMode)
{
    return PageProtectionMapping[(ui32)pageMode];
}

namespace StdLib::VirtualMemory
{
    void Initialize()
    {}
}
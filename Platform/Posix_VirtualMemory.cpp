#include "_PreHeader.hpp"
#include "VirtualMemory.hpp"
#include <sys/mman.h>

using namespace StdLib;

namespace
{
    constexpr std::array<int, 8> PageProtectionMapping =
    {
        -1, // 0 - Unused
        -1, // 1 - Write
        PROT_READ, // 2 - Read
        PROT_WRITE | PROT_READ, // 3 - Write + Read
        PROT_EXEC, // 4 - Execute
        -1, // 5 - Execute + Write
        PROT_EXEC | PROT_READ, // 6 - Execute + Read
        PROT_EXEC | PROT_WRITE | PROT_READ // 7 - Execute + Write + Read
    };

#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
	uiw PageSizeValue;
#endif
}

static constexpr int PageModeToPosix(VirtualMemory::PageModes::PageMode pageMode);

void *VirtualMemory::Reserve(uiw size, bool isTopDown)
{
    ASSUME(size);
    return mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

Error<> VirtualMemory::Commit(void *memory, uiw size, PageModes::PageMode pageMode)
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

void *VirtualMemory::Alloc(uiw size, bool isTopDown, PageModes::PageMode pageMode)
{
    ASSUME(size);
    int protection = PageModeToPosix(pageMode);
    if (protection < 0)
    {
        SOFTBREAK;
        return nullptr;
    }
    return mmap(0, size, protection, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

bool VirtualMemory::Free(void *memory, uiw memorySize)
{
    ASSUME(memory && memorySize);
    return munmap(memory, memorySize) == 0;
}

auto VirtualMemory::PageModeRequest(const void *memory, uiw size) -> Result<PageModes::PageMode>
{
    return DefaultError::Unsupported();
}

Error<> VirtualMemory::PageModeChange(void *memory, uiw size, PageModes::PageMode pageMode)
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

#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
uiw VirtualMemory::PageSize()
{
	ASSUME(PageSizeValue > 0);
	return PageSizeValue;
}
#endif

constexpr int PageModeToPosix(VirtualMemory::PageModes::PageMode pageMode)
{
    return PageProtectionMapping[pageMode.AsInteger()];
}

namespace StdLib::VirtualMemory
{
    void Initialize()
    {
		#ifdef STDLIB_DONT_ASSUME_PAGE_SIZE
			int pageSize = sysconf(_SC_PAGESIZE);
			if (pageSize == -1)
			{
				SOFTBREAK;
				PageSizeValue = 4096;
			}
			else
			{
				PageSizeValue = static_cast<uiw>(pageSize);
			}
		#endif
	}
}
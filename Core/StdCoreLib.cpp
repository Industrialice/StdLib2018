#include "_PreHeader.hpp"
#include "_CoreInitialization.hpp"

using namespace StdLib;

namespace
{
    std::function<void(const char *, i32, i32)> SoftBreakCallback{};
}

namespace StdLib
{
    f32 DefaultF32Epsilon{};
}

#if defined(PLATFORM_ANDROID) && __ANDROID_API__ < 17
	#include <dlfcn.h>

	uiw (*MallocUsableSize)(const void *ptr);
#endif

static void DefaultSoftBreakCallback(const char *file, i32 line, i32 counter);

void _Private::SoftBreak(const char *file, i32 line, i32 counter)
{
    ASSUME(SoftBreakCallback);
    SoftBreakCallback(file, line, counter);
}

void Initialization::Initialize(const CoreParameters &parameters)
{
    SoftBreakCallback = parameters.softBreakCallback;
    if (!SoftBreakCallback)
    {
        SoftBreakCallback = DefaultSoftBreakCallback;
    }

    DefaultF32Epsilon = parameters.defaultF32Epsilon;

	#if defined(PLATFORM_ANDROID) && __ANDROID_API__ < 17
		void *libc = dlopen("libc.so", RTLD_NOW);
		ASSUME(libc);
		MallocUsableSize = reinterpret_cast<decltype(MallocUsableSize)>(dlsym(libc, "malloc_usable_size"));
		if (MallocUsableSize == nullptr)
		{
			MallocUsableSize = [](const void *ptr) -> uiw
			{
			    if (ptr == nullptr)
			    {
			        return 0;
			    }
				ui32 size = static_cast<const ui32 *>(ptr)[-1];
				return size - 6;
			};
			__android_log_print(ANDROID_LOG_WARN, "StdLib info", "Didn't find malloc_usable_size, using a hack");
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "StdLib info", "Found malloc_usable_size, using it");
		}
	#endif
}

void DefaultSoftBreakCallback(const char *file, i32 line, i32 counter)
{
#ifdef PLATFORM_WINDOWS
    class BreakInfo
    {
        i32 _line{};
        i32 _counter{};
        std::string _file{};

    public:
        BreakInfo(std::string_view file, i32 line, i32 counter) : _file{file}, _line{line}, _counter{counter}
        {}

        bool operator < (const BreakInfo &other) const
        {
            return std::make_tuple(_line, _counter, _file) < std::make_tuple(other._line, other._counter, other._file);
        }
    };

	static std::mutex mutex;
    static std::set<BreakInfo> breaksToIgnore;

	std::scoped_lock lock{mutex};

    BreakInfo newBreak{file, line, counter};
    auto it = breaksToIgnore.find(newBreak);
    if (it != breaksToIgnore.end())
    {
        return;
    }

    std::string text = "Soft break occured in file "s + file + " at line " + std::to_string(line) + "\nAbort - abort program execution\nRetry - break\nIgnore - ignore this break and stop reporting it";

	CURSORINFO cursorInfo{sizeof(CURSORINFO)};
	BOOL winApiResult = GetCursorInfo(&cursorInfo);
	ASSUME(winApiResult == TRUE);

	bool isCursorHidden = cursorInfo.flags == 0;
	if (isCursorHidden)
	{
		ShowCursor(TRUE);
	}

    int result = MessageBoxA(0, text.c_str(), "Soft break has occured", MB_ABORTRETRYIGNORE);
    switch (result)
    {
    case IDABORT:
        exit(1);
        break;
    case IDRETRY:
        __debugbreak();
        break;
    case IDIGNORE:
        breaksToIgnore.insert(newBreak);
        break;
    }
#elif defined(PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_ERROR, "soft break", "%s %i", file, line);
#else
    fprintf(stderr, "soft break %s %i\n", file, line);
#endif

	if (isCursorHidden)
	{
		ShowCursor(FALSE);
	}
}
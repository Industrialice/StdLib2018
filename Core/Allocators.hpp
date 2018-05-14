#pragma once

namespace StdLib::Allocator
{
    // TODO: alignment
    struct MallocBased
    {
        template <typename T = ui8> [[nodiscard]] UNIQUEPTRRETURN static T *Allocate(uiw count)
        {
            if (count)
            {
                if constexpr (!std::is_same_v<T, void>)
                {
                    count *= sizeof(T);
                }
                return (T *)malloc(count);
            }
            return nullptr;
        }

        template <typename T> [[nodiscard]] static T *Reallocate(T *memory, uiw count)
        {
            if (memory || count)
            {
                if (count == 0)
                {
                    count = 1;
                }
                if constexpr (!std::is_same_v<T, void>)
                {
                    count *= sizeof(T);
                }
                return (T *)realloc(memory, count);
            }
            return nullptr;
        }

        template <typename T> [[nodiscard]] static bool ReallocateInplace(T *memory, uiw count)
        {
            if constexpr (!std::is_same_v<T, void>)
            {
                count *= sizeof(T);
            }
        #ifdef PLATFORM_WINDOWS
            return _expand(memory, count) != nullptr;
        #else
            return false;
        #endif
        }

        template <typename T> [[nodiscard]] static uiw MemorySize(const T *memory)
        {
        #ifdef PLATFORM_WINDOWS
            return _msize((void *)memory);
        #elif defined(PLATFORM_ANDROID) || defined(PLATFORM_LINUX) || defined(PLATFORM_EMSCRIPTEN)
            return malloc_usable_size(memory);
        #else
            return malloc_size(memory);
        #endif
        }

        template <typename T> static void Free(T *memory)
        {
            free(memory);
        }
    };
}
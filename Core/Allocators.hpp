#pragma once

#if defined(PLATFORM_ANDROID) && __ANDROID_API__ < 17
	extern StdLib::uiw (*MallocUsableSize)(const void *ptr); // from StdCoreLib.cpp
#endif

namespace StdLib::Allocator
{
	#ifdef PLATFORM_WINDOWS
		#ifdef _WIN64
			static constexpr uiw MinimalGuaranteedAlignment = 16;
		#else
			static constexpr uiw MinimalGuaranteedAlignment = 8;
		#endif
	#else
		static constexpr uiw MinimalGuaranteedAlignment = 8;
	#endif

    struct Malloc
    {
        template <typename T = ui8> [[nodiscard]] UNIQUEPTRRETURN static T *Allocate(uiw count)
        {
            if constexpr (!std::is_same_v<T, void>)
            {
                count *= sizeof(T);
            }
            return (T *)malloc(count);
        }

        template <typename T> [[nodiscard]] static T *Reallocate(T *memory, uiw count)
        {
            if constexpr (!std::is_same_v<T, void>)
            {
                count *= sizeof(T);
            }
            return (T *)realloc(memory, count);
        }

        template <typename T> [[nodiscard]] static bool ReallocateInplace(T *memory, uiw count)
        {
            if constexpr (!std::is_same_v<T, void>)
            {
                count *= sizeof(T);
            }

        #ifdef PLATFORM_WINDOWS

            if (memory == nullptr)
            {
                return count == 0;
            }
            else
            {
                return _expand(memory, count) != nullptr;
            }

        #else

            return false; // TODO: solutions for other platforms?

        #endif
        }

        template <typename T> [[nodiscard]] static uiw MemorySize(const T *memory)
        {
        #ifdef PLATFORM_WINDOWS
            return memory ? _msize((void *)memory) : 0;
        #elif defined(PLATFORM_ANDROID)
			#if __ANDROID_API__ >= 17
				return malloc_usable_size(memory);
			#else
				return MallocUsableSize(memory);
			#endif
        #elif defined(PLATFORM_LINUX) || defined(PLATFORM_EMSCRIPTEN)
            return malloc_usable_size((void *)memory);
        #else
            return malloc_size((void *)memory);
        #endif
        }

        template <typename T> static void Free(T *memory)
        {
            free(memory);
        }
    };

	// TODO: optimize the non-Windows version
	struct MallocAlignedRuntime
	{
		template <typename T = ui8> [[nodiscard]] UNIQUEPTRRETURN static T *Allocate(uiw count, uiw alignment)
		{
			if constexpr (!std::is_same_v<T, void>)
			{
				count *= sizeof(T);
			}

			#ifdef PLATFORM_WINDOWS
				return (T *)_aligned_malloc(count, alignment);
			#else
				void *memory;
				posix_memalign(&memory, alignment, count);
				return (T *)memory;
			#endif
		}

		template <typename T> [[nodiscard]] static T *Reallocate(T *memory, uiw count, uiw alignment)
		{
			if constexpr (!std::is_same_v<T, void>)
			{
				count *= sizeof(T);
			}

			#ifdef PLATFORM_WINDOWS
				return (T *)_aligned_realloc(memory, count, alignment);
			#else
				memory = (T *)realloc(memory, count);
				if ((uiw)memory & (alignment - 1))
				{
					void *temp;
					posix_memalign(&temp, alignment, count);
					memcpy(temp, memory, count);
					free(memory);
					memory = (T *)temp;
				}
				return memory;
			#endif
		}

		template <typename T> static void Free(T *memory)
		{
			#ifdef PLATFORM_WINDOWS
				_aligned_free(memory);
			#else
				free(memory);
			#endif
		}

		// TODO: any solutions?
		template <typename T> [[nodiscard]] static bool ReallocateInplace(T *memory, uiw count)
		{
			return false;
		}

        template <typename T> [[nodiscard]] static uiw MemorySize(const T *memory, uiw alignment)
        {
			#ifdef PLATFORM_WINDOWS
				return memory ? _aligned_msize((void *)memory, alignment, 0) : 0;
			#else
				return Malloc::MemorySize(memory);
			#endif
		}
	};

	template <uiw Alignment> struct MallocAlignedPredefined
	{
		template <typename T = ui8> [[nodiscard]] UNIQUEPTRRETURN static T *Allocate(uiw count)
		{
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::Allocate<T>(count, Alignment);
			}
			else
			{
				return Malloc::Allocate<T>(count);
			}
		}

		template <typename T> [[nodiscard]] static T *Reallocate(T *memory, uiw count)
		{
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::Reallocate(memory, count, Alignment);
			}
			else
			{
				return Malloc::Reallocate(memory, count);
			}
		}

		template <typename T> static void Free(T *memory)
		{
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::Free(memory);
			}
			else
			{
				return Malloc::Free(memory);
			}
		}

		template <typename T> [[nodiscard]] static bool ReallocateInplace(T *memory, uiw count)
		{
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::ReallocateInplace(memory, count);
			}
			else
			{
				return Malloc::ReallocateInplace(memory, count);
			}
		}

        template <typename T> [[nodiscard]] static uiw MemorySize(const T *memory)
        {
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::MemorySize(memory, Alignment);
			}
			else
			{
				return Malloc::MemorySize(memory);
			}
		}
	};
	
    struct MallocAligned
    {
        template <uiw Alignment, typename T = ui8> [[nodiscard]] UNIQUEPTRRETURN static T *Allocate(uiw count)
        {
			return MallocAlignedPredefined<Alignment>::template Allocate<T>(count);
        }

        template <uiw Alignment, typename T> [[nodiscard]] static T *Reallocate(T *memory, uiw count)
        {
			return MallocAlignedPredefined<Alignment>::template Reallocate(memory, count);
        }

        template <uiw Alignment, typename T> static void Free(T *memory)
        {
			return MallocAlignedPredefined<Alignment>::template Free(memory);
        }

		template <uiw Alignment, typename T> [[nodiscard]] static bool ReallocateInplace(T *memory, uiw count)
		{
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::ReallocateInplace(memory, count);
			}
			else
			{
				return Malloc::ReallocateInplace(memory, count);
			}
		}

        template <uiw Alignment, typename T> [[nodiscard]] static uiw MemorySize(const T *memory)
        {
			if constexpr (Alignment > MinimalGuaranteedAlignment)
			{
				return MallocAlignedRuntime::MemorySize(memory, Alignment);
			}
			else
			{
				return Malloc::MemorySize(memory);
			}
		}
    };
}
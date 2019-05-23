#pragma once

#include "NameOfType.hpp"
#include "HashFuncs.hpp"
#include "CompileTimeStrings.hpp"

#ifdef DEBUG
    #define USE_ID_NAMES
#endif

namespace StdLib
{
	class StableTypeId
	{
	public:
		using idType = ui32;

		constexpr StableTypeId() = default;

		constexpr StableTypeId(idType id) : _id(id)
		{}

#ifdef USE_ID_NAMES
		constexpr StableTypeId(idType id, const char *name) : _id(id), _name{ name }
		{}
#endif

		[[nodiscard]] constexpr bool operator == (const StableTypeId &other) const
		{
			return _id == other._id;
		}

		[[nodiscard]] constexpr bool operator != (const StableTypeId &other) const
		{
			return _id != other._id;
		}

		[[nodiscard]] constexpr bool operator < (const StableTypeId &other) const
		{
			return _id < other._id;
		}

		[[nodiscard]] constexpr bool operator <= (const StableTypeId &other) const
		{
			return _id <= other._id;
		}

		[[nodiscard]] constexpr bool operator > (const StableTypeId &other) const
		{
			return _id > other._id;
		}

		[[nodiscard]] constexpr bool operator >= (const StableTypeId &other) const
		{
			return _id >= other._id;
		}

		[[nodiscard]] constexpr idType Hash() const
		{
			return _id;
		}

		constexpr const char *Name() const
		{
#ifdef USE_ID_NAMES
			return _name;
#else
			return "{DebugTypeNamesDisabled}";
#endif
		}

	private:
		idType _id{};
#ifdef USE_ID_NAMES
		const char *_name{};
#endif
	};

	struct StableTypeIdentifiableBase
	{};
    
	template <typename T> struct EMPTY_BASES TypeIdentifiable : StableTypeIdentifiableBase
	{
	private:
		static constexpr uiw staticNameLength = NameOfType<T>.size() - 1;
		static constexpr std::array<char, staticNameLength + 1> staticName = NameOfType<T>;
		static constexpr StableTypeId::idType stableId = StdLib::Hash::FNVHashCT<std::is_same_v<StableTypeId::idType, ui64> ? StdLib::Hash::Precision::P64 : StdLib::Hash::Precision::P32, char, staticNameLength>(staticName.data());

	public:
		[[nodiscard]] static constexpr StableTypeId GetTypeId()
		{
#ifdef USE_ID_NAMES
			return { stableId, staticName.data() };
#else
			return stableId;
#endif
		}

		[[nodiscard]] static constexpr std::string_view GetTypeName()
		{
			return { staticName.data(), staticNameLength };
		}
	};

    template <StableTypeId::idType stableId, ui64 encoded0 = 0, ui64 encoded1 = 0, ui64 encoded2 = 0> struct EMPTY_BASES StableTypeIdentifiable : StableTypeIdentifiableBase
    {
    private:
        static constexpr uiw staticNameLength = CompileTimeStrings::DecodeASCIIToLength<encoded0, encoded1, encoded2>();
		static constexpr std::array<char, staticNameLength + 1> staticName = CompileTimeStrings::DecodeASCIIToArray<staticNameLength + 1, encoded0, encoded1, encoded2>();

    public:
        [[nodiscard]] static constexpr StableTypeId GetTypeId()
        {
        #ifdef USE_ID_NAMES
            return {stableId, staticName.data()};
        #else
            return stableId;
        #endif
        }

        [[nodiscard]] static constexpr std::string_view GetTypeName()
        {
            return {staticName.data(), staticNameLength};
        }
    };

    //template <TypeId::InternalIdType id, bool isWriteable> struct TypeIdToType;
}

/*#define GENERATE_TYPE_ID_TO_TYPE(T) \
    template <> struct TypeIdToType<T::GetTypeId().InternalId(), true> \
    { \
        using type = T; \
    }; \
    template <> struct TypeIdToType<T::GetTypeId().InternalId(), false> \
    { \
        using type = const T; \
    }*/

namespace std
{
    //template <> struct hash<StdLib::TypeId>
    //{
    //    size_t operator()(const StdLib::TypeId &value) const
    //    {
    //        return (size_t)value.Hash();
    //    }
    //};

	template <> struct hash<StdLib::StableTypeId>
	{
		[[nodiscard]] size_t operator()(const StdLib::StableTypeId &value) const
		{
			return (size_t)value.Hash();
		}
	};
}

#define NAME_TO_STABLE_ID(name) StableTypeIdentifiable<StdLib::Hash::FNVHashCT<std::is_same_v<StableTypeId::idType, ui64> ? StdLib::Hash::Precision::P64 : StdLib::Hash::Precision::P32, char, CountOf(TOSTR(name)), true>(TOSTR(name)), \
    StdLib::CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), StdLib::CompileTimeStrings::CharsPerNumber * 0), \
    StdLib::CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), StdLib::CompileTimeStrings::CharsPerNumber * 1), \
    StdLib::CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), StdLib::CompileTimeStrings::CharsPerNumber * 2)>
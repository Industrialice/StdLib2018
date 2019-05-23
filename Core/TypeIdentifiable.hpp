#pragma once

#include "NameOfType.hpp"
#include "HashFuncs.hpp"
#include "CompileTimeStrings.hpp"

#ifdef DEBUG
    #define USE_ID_NAMES
#endif

namespace StdLib
{
	class TypeId
	{
	public:
		using idType = ui32;

		constexpr TypeId() = default;

		constexpr TypeId(idType id) : _id(id)
		{}

#ifdef USE_ID_NAMES
		constexpr TypeId(idType id, const char *name) : _id(id), _name{ name }
		{}
#endif

		[[nodiscard]] constexpr bool operator == (const TypeId &other) const
		{
			return _id == other._id;
		}

		[[nodiscard]] constexpr bool operator != (const TypeId &other) const
		{
			return _id != other._id;
		}

		[[nodiscard]] constexpr bool operator < (const TypeId &other) const
		{
			return _id < other._id;
		}

		[[nodiscard]] constexpr bool operator <= (const TypeId &other) const
		{
			return _id <= other._id;
		}

		[[nodiscard]] constexpr bool operator > (const TypeId &other) const
		{
			return _id > other._id;
		}

		[[nodiscard]] constexpr bool operator >= (const TypeId &other) const
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

	struct TypeIdentifiableBase
	{};
    
	template <typename T> struct EMPTY_BASES TypeIdentifiable : TypeIdentifiableBase
	{
	private:
		static constexpr auto staticNameToHash = NameOfType<T>;
		static constexpr auto staticName = NameOfType<T, true>;
		static constexpr TypeId::idType stableId = StdLib::Hash::FNVHashCT<std::is_same_v<TypeId::idType, ui64> ? StdLib::Hash::Precision::P64 : StdLib::Hash::Precision::P32, char, staticNameToHash.size() - 1>(staticNameToHash.data());

	public:
		[[nodiscard]] static constexpr TypeId GetTypeId()
		{
#ifdef USE_ID_NAMES
			return { stableId, staticName.data() };
#else
			return stableId;
#endif
		}

		[[nodiscard]] static constexpr std::string_view GetTypeName()
		{
			return { staticName.data(), staticName.size() - 1 };
		}
	};
}

namespace std
{
	template <> struct hash<StdLib::TypeId>
	{
		[[nodiscard]] size_t operator()(const StdLib::TypeId &value) const
		{
			return (size_t)value.Hash();
		}
	};
}
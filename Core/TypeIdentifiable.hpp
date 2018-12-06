#pragma once

#include "HashFuncs.hpp"

namespace StdLib
{
	class TypeId
	{
		const char *_id{};

	public:
		using InternalIdType = decltype(_id);

        constexpr TypeId() = default;

		constexpr TypeId(InternalIdType id) : _id(id)
		{}

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

		[[nodiscard]] constexpr bool operator > (const TypeId &other) const
		{
			return _id > other._id;
		}

		[[nodiscard]] constexpr InternalIdType InternalId() const
		{
			return _id;
		}

		[[nodiscard]] ui64 Hash() const
		{
			return Hash::FNVHash<Hash::Precision::P64>(_id);
		}
	};

    template <typename T> class TypeIdentifiable
    {
        static constexpr char var = 0;

    public:
        [[nodiscard]] static constexpr TypeId GetTypeId()
        {
			return &var;
        }
    };

	class StableTypeId
	{
		ui64 _id{};

	public:
		using InternalIdType = decltype(_id);

		constexpr StableTypeId() = default;

		constexpr StableTypeId(InternalIdType id) : _id(id)
		{}

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

		[[nodiscard]] constexpr bool operator > (const StableTypeId &other) const
		{
			return _id > other._id;
		}

		[[nodiscard]] constexpr ui64 Hash() const
		{
			return _id;
		}
	};

	template <ui64 stableId> class StableTypeIdentifiable
	{
	public:
		[[nodiscard]] static constexpr StableTypeId GetTypeId()
		{
			return stableId;
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
    template <> struct hash<StdLib::TypeId>
    {
        size_t operator()(const StdLib::TypeId &value) const
        {
            return (size_t)value.Hash();
        }
    };

	template <> struct hash<StdLib::StableTypeId>
	{
		size_t operator()(const StdLib::StableTypeId &value) const
		{
			return (size_t)value.Hash();
		}
	};
}
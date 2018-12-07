#pragma once

#include "HashFuncs.hpp"

namespace StdLib
{
    class TypeId
    {
        const char *_id{};
    #ifdef DEBUG
        union
        {
            std::array<char, 22> name;
            char displayName[22];
        } _u{};
    #endif

    public:
        using InternalIdType = decltype(_id);

        constexpr TypeId() = default;

        constexpr TypeId(InternalIdType id) : _id(id)
        {}

    #ifdef DEBUG
        constexpr TypeId(InternalIdType id, std::array<char, 22> name) : _id(id), _u{name}
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
    
#ifdef DEBUG
    template <typename T, ui64 encoded0 = 0, ui64 encoded1 = 0, ui64 encoded2 = 0> class TypeIdentifiable
    {
        static constexpr char var = 0;

    public:
        [[nodiscard]] static constexpr TypeId GetTypeId()
        {
            std::array<char, 22> name{};
            CompileTimeStrings::DecodeASCII<encoded0, encoded1, encoded2>(name.data(), name.size());
            return {&var, name};
        }
    };
#else
    template <typename T> class TypeIdentifiable
    {
        static constexpr char var = 0;

    public:
        [[nodiscard]] static constexpr TypeId GetTypeId()
        {
            return &var;
        }
    };
#endif

	class StableTypeId
	{
		ui64 _id{};
    #ifdef DEBUG
        union
        {
            std::array<char, 22> name;
            char displayName[22];
        } _u{};
    #endif

	public:
		using InternalIdType = decltype(_id);

		constexpr StableTypeId() = default;

		constexpr StableTypeId(InternalIdType id) : _id(id)
		{}

    #ifdef DEBUG
        constexpr StableTypeId(InternalIdType id, std::array<char, 22> name) : _id(id), _u{name}
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

		[[nodiscard]] constexpr bool operator > (const StableTypeId &other) const
		{
			return _id > other._id;
		}

		[[nodiscard]] constexpr ui64 Hash() const
		{
			return _id;
		}
	};

#ifdef DEBUG
    template <ui64 stableId, ui64 encoded0 = 0, ui64 encoded1 = 0, ui64 encoded2 = 0> class StableTypeIdentifiable
    {
    public:
        [[nodiscard]] static constexpr StableTypeId GetTypeId()
        {
            std::array<char, 22> name{};
            CompileTimeStrings::DecodeASCII<encoded0, encoded1, encoded2>(name.data(), name.size());
            return {stableId, name};
        }
    };
#else
	template <ui64 stableId> class StableTypeIdentifiable
	{
	public:
		[[nodiscard]] static constexpr StableTypeId GetTypeId()
		{
			return stableId;
		}
	};
#endif

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
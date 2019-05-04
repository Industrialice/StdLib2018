#pragma once

#include "HashFuncs.hpp"

#ifdef DEBUG
    #define USE_ID_NAMES
#endif

namespace StdLib
{
// Doesn't work in Release in Visual C++
//    class TypeId
//    {
//        const char *_id{};
//    #ifdef USE_ID_NAMES
//        union
//        {
//            std::array<char, 28> name;
//            char displayName[28];
//        } _u{};
//    #endif
//
//    public:
//        using InternalIdType = decltype(_id);
//
//        constexpr TypeId() = default;
//
//        constexpr TypeId(InternalIdType id) : _id(id)
//        {}
//
//    #ifdef USE_ID_NAMES
//        constexpr TypeId(InternalIdType id, std::array<char, 28> name) : _id(id), _u{name}
//        {}
//    #endif
//
//		[[nodiscard]] constexpr bool operator == (const TypeId &other) const
//		{
//			return _id == other._id;
//		}
//
//		[[nodiscard]] constexpr bool operator != (const TypeId &other) const
//		{
//			return _id != other._id;
//		}
//
//		[[nodiscard]] constexpr bool operator < (const TypeId &other) const
//		{
//			return _id < other._id;
//		}
//
//        [[nodiscard]] constexpr bool operator <= (const TypeId &other) const
//        {
//            return _id <= other._id;
//        }
//
//		[[nodiscard]] constexpr bool operator > (const TypeId &other) const
//		{
//			return _id > other._id;
//		}
//
//        [[nodiscard]] constexpr bool operator >= (const TypeId &other) const
//        {
//            return _id >= other._id;
//        }
//
//		[[nodiscard]] constexpr InternalIdType InternalId() const
//		{
//			return _id;
//		}
//
//		[[nodiscard]] ui64 Hash() const
//		{
//			return Hash::FNVHash<Hash::Precision::P64>(_id);
//		}
//	};
//    
//#ifdef USE_ID_NAMES
//    template <typename T, ui64 encoded0 = 0, ui64 encoded1 = 0, ui64 encoded2 = 0> class TypeIdentifiable
//    {
//        static constexpr char var = 0;
//
//    public:
//        [[nodiscard]] static constexpr TypeId GetTypeId()
//        {
//            std::array<char, 28> name{};
//            CompileTimeStrings::DecodeASCII<encoded0, encoded1, encoded2>(name.data(), name.size());
//            return {&var, name};
//        }
//    };
//#else
//    template <typename T> class TypeIdentifiable
//    {
//        static constexpr char var = 0;
//
//    public:
//        [[nodiscard]] static constexpr TypeId GetTypeId()
//        {
//            return &var;
//        }
//    };
//#endif

	class StableTypeId
	{
		ui64 _id{};
    #ifdef USE_ID_NAMES
		const char *_name{};
    #endif

	public:
		using InternalIdType = decltype(_id);

		constexpr StableTypeId() = default;

		constexpr StableTypeId(InternalIdType id) : _id(id)
		{}

    #ifdef USE_ID_NAMES
        constexpr StableTypeId(InternalIdType id, const char *name) : _id(id), _name{name}
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

		[[nodiscard]] constexpr ui64 Hash() const
		{
			return _id;
		}

        constexpr const char *Name() const
        {
        #ifdef USE_ID_NAMES
            return _name;
        #else
            return "{NoNameInRelease}";
        #endif
        }
	};

    struct StableTypeIdentifiableBase
    {};

#ifdef USE_ID_NAMES
    template <ui64 stableId, ui64 encoded0 = 0, ui64 encoded1 = 0, ui64 encoded2 = 0> class EMPTY_BASES StableTypeIdentifiable : public StableTypeIdentifiableBase
    {
		static constexpr std::array<char, 28> staticName = CompileTimeStrings::DecodeASCIIToArray<28, encoded0, encoded1, encoded2>();

    public:
        [[nodiscard]] static constexpr StableTypeId GetTypeId()
        {
            return {stableId, staticName.data()};
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
    //template <> struct hash<StdLib::TypeId>
    //{
    //    size_t operator()(const StdLib::TypeId &value) const
    //    {
    //        return (size_t)value.Hash();
    //    }
    //};

	template <> struct hash<StdLib::StableTypeId>
	{
		size_t operator()(const StdLib::StableTypeId &value) const
		{
			return (size_t)value.Hash();
		}
	};
}

#ifdef USE_ID_NAMES
    #define NAME_TO_STABLE_ID(name) StableTypeIdentifiable<Hash::FNVHashCT<Hash::Precision::P64, char, CountOf(TOSTR(name)), true>(TOSTR(name)), \
        CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 0), \
        CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 1), \
        CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 2)>
#else
    #define NAME_TO_STABLE_ID(name) StableTypeIdentifiable<Hash::FNVHashCT<Hash::Precision::P64, char, CountOf(TOSTR(name)), true>(TOSTR(name))>
#endif
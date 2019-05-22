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
	public:
		using idType = ui32;

		constexpr StableTypeId() = default;

		constexpr StableTypeId(idType id) : _id(id)
		{}

    #ifdef USE_ID_NAMES
        constexpr StableTypeId(idType id, const char *name) : _id(id), _name{name}
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

#define NAME_TO_STABLE_ID(name) StableTypeIdentifiable<Hash::FNVHashCT<std::is_same_v<StableTypeId::idType, ui64> ? Hash::Precision::P64 : Hash::Precision::P32, char, CountOf(TOSTR(name)), true>(TOSTR(name)), \
    CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 0), \
    CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 1), \
    CompileTimeStrings::EncodeASCII(TOSTR(name), CountOf(TOSTR(name)), CompileTimeStrings::CharsPerNumber * 2)>
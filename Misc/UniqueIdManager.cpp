#include "_PreHeader.hpp"
#include "UniqueIdManager.hpp"

using namespace StdLib;
using std::array;
using std::make_unique;

template <ui32 level> [[nodiscard]] static FORCEINLINE ui32 IDToLevelIndex(ui32 id)
{
    if constexpr (level == 0)
    {
        return id / (64 * 64 * 64 * 64 * 64);
    }
    else if constexpr (level == 1)
    {
        id %= (64 * 64 * 64 * 64 * 64);
        return id / (64 * 64 * 64 * 64);
    }
    else if constexpr (level == 2)
    {
        id %= (64 * 64 * 64 * 64);
        return id / (64 * 64 * 64);
    }
    else if constexpr (level == 3)
    {
        id %= (64 * 64 * 64);
        return id / (64 * 64);
    }
    else if constexpr (level == 4)
    {
        id %= (64 * 64);
        return id / 64;
    }
    else if constexpr (level == 5)
    {
        return id % 64;
    }
}

template <typename T> NOINLINE void UniqueIdManager::AllocateLevel(T &level)
{
	if constexpr (std::is_same_v<T, Level1>)
	{
		level.level = make_unique<array<Level2, 64>>();
	}
	else if constexpr (std::is_same_v<T, Level2>)
	{
		level.level = make_unique<array<Level3, 64>>();
	}
	else if constexpr (std::is_same_v<T, Level3>)
	{
		level.level = make_unique<array<ui64, 64>>();
		level.level->fill(ui64_max);
	}
}

ui32 UniqueIdManager::Allocate()
{
    for (uiw level0Index = 0; level0Index < 4; ++level0Index)
    {
        ui64 &level0Mask = _level0Masks[level0Index];
        if (level0Mask == 0)
        {
            continue; // the level is full
        }

        array<Level1, 64> &level0Levels = _level0Levels[level0Index];

		uiw id = level0Index * (64 * 64 * 64 * 64 * 64);

        uiw level1Index = Funcs::IndexOfLeastSignificantNonZeroBit(level0Mask);
        Level1 &level1 = level0Levels[level1Index];
        ASSUME(level1.mask != 0);
        id += level1Index * (64 * 64 * 64 * 64);

        if (level1.level == nullptr)
        {
			AllocateLevel(level1);
        }
		uiw level2Index = Funcs::IndexOfLeastSignificantNonZeroBit(level1.mask);
        Level2 &level2 = level1.level->operator[](level2Index);
        ASSUME(level2.mask != 0);
        id += level2Index * (64 * 64 * 64);

        if (level2.level == nullptr)
        {
			AllocateLevel(level2);
        }
		uiw level3Index = Funcs::IndexOfLeastSignificantNonZeroBit(level2.mask);
        Level3 &level3 = level2.level->operator[](level3Index);
        ASSUME(level3.mask != 0);
        id += level3Index * (64 * 64);

        if (level3.level == nullptr)
        {
			AllocateLevel(level3);
        }
		uiw level4Index = Funcs::IndexOfLeastSignificantNonZeroBit(level3.mask);
        ui64 &level4 = level3.level->operator[](level4Index);
        ASSUME(level4 != 0);
        id += level4Index * 64;

		uiw level5Index = Funcs::IndexOfLeastSignificantNonZeroBit(level4);
        id += level5Index;

        level4 = Funcs::SetBit(level4, level5Index, false);
        if (level4 == 0)
        {
            level3.mask = Funcs::SetBit(level3.mask, level4Index, false);
            if (level3.mask == 0)
            {
                level2.mask = Funcs::SetBit(level2.mask, level3Index, false);
                if (level2.mask == 0)
                {
                    level1.mask = Funcs::SetBit(level1.mask, level2Index, false);
                    if (level1.mask == 0)
                    {
                        level0Mask = Funcs::SetBit(level0Mask, level1Index, false);
                    }
                }
            }
        }

        return static_cast<ui32>(id);
    }

    return invalidId;
}

bool UniqueIdManager::Allocate(ui32 id)
{
    ASSUME(id != invalidId);
    ui32 level0Index = IDToLevelIndex<0>(id);
    ui64 &level0Mask = _level0Masks[level0Index];
    array<Level1, 64> &level0Levels = _level0Levels[level0Index];
    ui32 level1Index = IDToLevelIndex<1>(id);
    Level1 &level1 = level0Levels[level1Index];
    if (level1.level == nullptr)
    {
		AllocateLevel(level1);
    }
    ui32 level2Index = IDToLevelIndex<2>(id);
    Level2 &level2 = level1.level->operator[](level2Index);
    if (level2.level == nullptr)
    {
		AllocateLevel(level2);
    }
    ui32 level3Index = IDToLevelIndex<3>(id);
    Level3 &level3 = level2.level->operator[](level3Index);
    if (level3.level == nullptr)
    {
		AllocateLevel(level3);
    }
    ui32 level4Index = IDToLevelIndex<4>(id);
    ui64 &level4 = level3.level->operator[](level4Index);
    ui32 level5Index = IDToLevelIndex<5>(id);
    if (Funcs::IsBitSet(level4, level5Index) == false)
    {
        return false;
    }
    level4 = Funcs::SetBit(level4, level5Index, false);
    if (level4 == 0)
    {
        level3.mask = Funcs::SetBit(level3.mask, level4Index, false);
        if (level3.mask == 0)
        {
            level2.mask = Funcs::SetBit(level2.mask, level3Index, false);
            if (level2.mask == 0)
            {
                level1.mask = Funcs::SetBit(level1.mask, level2Index, false);
                if (level1.mask == 0)
                {
                    level0Mask = Funcs::SetBit(level0Mask, level1Index, false);
                }
            }
        }
    }
    return true;
}

bool UniqueIdManager::Free(ui32 id)
{
    ASSUME(id != invalidId);
    ui32 level0Index = IDToLevelIndex<0>(id);
    ui64 &level0Mask = _level0Masks[level0Index];
    array<Level1, 64> &level0Levels = _level0Levels[level0Index];
    ui32 level1Index = IDToLevelIndex<1>(id);
    Level1 &level1 = level0Levels[level1Index];
    if (level1.level == nullptr)
    {
        return false;
    }
    ui32 level2Index = IDToLevelIndex<2>(id);
    Level2 &level2 = level1.level->operator[](level2Index);
    if (level2.level == nullptr)
    {
        return false;
    }
    ui32 level3Index = IDToLevelIndex<3>(id);
    Level3 &level3 = level2.level->operator[](level3Index);
    if (level3.level == nullptr)
    {
        return false;
    }
    ui32 level4Index = IDToLevelIndex<4>(id);
    ui64 &level4 = level3.level->operator[](level4Index);
    ui32 level5Index = IDToLevelIndex<5>(id);
    if (Funcs::IsBitSet(level4, level5Index))
    {
        return false;
    }
    level4 = Funcs::SetBit(level4, level5Index, true);
    if (level4 == ui64_max)
    {
        level3.mask = Funcs::SetBit(level3.mask, level4Index, true);
        if (level3.mask == ui64_max)
        {
            level2.mask = Funcs::SetBit(level2.mask, level3Index, true);
            if (level2.mask == ui64_max)
            {
                level1.mask = Funcs::SetBit(level1.mask, level2Index, true);
                if (level1.mask == ui64_max)
                {
                    level0Mask = Funcs::SetBit(level0Mask, level1Index, true);
                }
            }
        }
    }
    return true;
}

bool StdLib::UniqueIdManager::IsAllocated(ui32 id) const
{
    ASSUME(id != invalidId);
    ui32 level0Index = IDToLevelIndex<0>(id);
    const array<Level1, 64> &level0Levels = _level0Levels[level0Index];
    ui32 level1Index = IDToLevelIndex<1>(id);
    const Level1 &level1 = level0Levels[level1Index];
    if (level1.level == nullptr)
    {
        return false;
    }
    ui32 level2Index = IDToLevelIndex<2>(id);
    const Level2 &level2 = level1.level->operator[](level2Index);
    if (level2.level == nullptr)
    {
        return false;
    }
    ui32 level3Index = IDToLevelIndex<3>(id);
    const Level3 &level3 = level2.level->operator[](level3Index);
    if (level3.level == nullptr)
    {
        return false;
    }
    ui32 level4Index = IDToLevelIndex<4>(id);
    ui64 level4 = level3.level->operator[](level4Index);
    ui32 level5Index = IDToLevelIndex<5>(id);
    return Funcs::IsBitSet(level4, level5Index) == false;
}
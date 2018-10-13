#pragma once

namespace StdLib
{
    class UniqueIdManager
    {
        struct Level3
        {
            ui64 mask = ui64_max;
            std::unique_ptr<std::array<ui64, 64>> level{};
        };

        struct Level2
        {
            ui64 mask = ui64_max;
            std::unique_ptr<std::array<Level3, 64>> level{};
        };

        struct Level1
        {
            ui64 mask = ui64_max;
            std::unique_ptr<std::array<Level2, 64>> level{};
        };

        std::array<ui64, 4> _level0Masks{ui64_max, ui64_max, ui64_max, ui64_max};
        std::array<std::array<Level1, 64>, 4> _level0Levels{};

		template <typename T> NOINLINE void AllocateLevel(T &level);

    public:
        static constexpr ui32 invalidId = ui32_max;

        [[nodiscard]] ui32 Allocate();
        [[nodiscard]] bool Allocate(ui32 id);
        bool Free(ui32 id);
        [[nodiscard]] bool IsAllocated(ui32 id) const;
        // TODO: shink_to_fit
        // TODO: enumerate allocated ids
    };
}
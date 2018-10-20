#include "_PreHeader.hpp"
#include <UniqueIdManager.hpp>
#include <random>

using namespace StdLib;

static void Shuffle(const std::vector<std::pair<ui32, ui32>> &shuffleIndexes, std::vector<ui32> &ids)
{
    for (size_t index = 0, size = ids.size(); index < size; ++index)
    {
        auto p = shuffleIndexes[index];
        std::swap(ids[p.first], ids[p.second]);
    }
}

static std::pair<f32, f32> Benchmark(const std::vector<std::pair<ui32, ui32>> &shuffleIndexes, std::vector<ui32> &ids)
{
    ui32 requestSize = (ui32)ids.size();

	UniqueIdManager manager;

    auto startRandomGenerating = TimeMoment::Now();

    for (ui32 index = 0; index < requestSize; ++index)
    {
        ids[index] = manager.Allocate();
    }

    auto endRandomGenerating = TimeMoment::Now();

    manager = {};
    Shuffle(shuffleIndexes, ids);

    auto startLoading = TimeMoment::Now();

    for (ui32 id : ids)
    {
        bool result = manager.Allocate(id);
        ASSUME(result);
    }

    auto endLoading = TimeMoment::Now();

    f32 time0 = (endRandomGenerating - startRandomGenerating).ToSeconds();
    f32 time1 = (endLoading - startLoading).ToSeconds();

    return {time0, time1};
}

static void PrepareShuffler(uiw requestSize, std::vector<std::pair<ui32, ui32>> &shuffleIndexes)
{
    shuffleIndexes.clear();

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<ui32> uni(0, (ui32)requestSize - 1);

    shuffleIndexes.reserve(requestSize);
    for (uiw index = 0; index < requestSize; ++index)
    {
        ui32 r0 = uni(rng);
        ui32 r1 = uni(rng);
        while (r0 == r1)
        {
            r0 = uni(rng);
        }
        shuffleIndexes.push_back({r0, r1});
    }
}

void UniqueIdManagerBenchmark()
{
    f32 bestTime0 = f32_max, bestTime1 = f32_max;
    std::vector<ui32> ids(100'000);
    std::vector<std::pair<ui32, ui32>> shuffleIndexes;
    PrepareShuffler(ids.size(), shuffleIndexes);
    for (ui32 index = 0; index < 150; ++index)
    {
        auto[time0, time1] = Benchmark(shuffleIndexes, ids);
        bestTime0 = std::min(bestTime0, time0);
        bestTime1 = std::min(bestTime1, time1);
    }

    PRINTLOG("Done 100k for %g : %g\n", bestTime0, bestTime1);

    bestTime0 = bestTime1 = f32_max;
    ids.resize(1'000'000);
    PrepareShuffler(ids.size(), shuffleIndexes);
    for (ui32 index = 0; index < 50; ++index)
    {
        auto[time0, time1] = Benchmark(shuffleIndexes, ids);
        bestTime0 = std::min(bestTime0, time0);
        bestTime1 = std::min(bestTime1, time1);
    }

    PRINTLOG("Done 1kk for %g : %g\n", bestTime0, bestTime1);

    bestTime0 = bestTime1 = f32_max;
    ids.resize(10'000'000);
    PrepareShuffler(ids.size(), shuffleIndexes);
    for (ui32 index = 0; index < 10; ++index)
    {
        auto[time0, time1] = Benchmark(shuffleIndexes, ids);
        bestTime0 = std::min(bestTime0, time0);
        bestTime1 = std::min(bestTime1, time1);
    }

    PRINTLOG("Done 10kk for %g : %g\n", bestTime0, bestTime1);

    PRINTLOG("~~~Finished benchmarking unique id manager~~~\n");
}
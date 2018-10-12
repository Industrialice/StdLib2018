#include "_PreHeader.hpp"

using namespace StdLib;

void UniqueIdManagerTests()
{
    ui32 requestSize = 1'000;

    std::vector<ui32> ids;
    ids.resize(requestSize);

    UniqueIdManager manager;

    for (ui32 index = 0; index < requestSize; ++index)
    {
        ids[index] = manager.Allocate();
    }

    for (ui32 index = 0; index < requestSize * 2; ++index)
    {
        if (index < requestSize)
        {
            ASSUME(manager.IsAllocated(index));
        }
        else
        {
            ASSUME(false == manager.IsAllocated(index));
        }
    }

    manager = {};

    for (ui32 id : ids)
    {
        bool result = manager.Allocate(id);
        ASSUME(result);
    }

    for (ui32 index = 0; index < requestSize * 2; ++index)
    {
        if (index < requestSize)
        {
            ASSUME(manager.IsAllocated(ids[index]));
            bool freeResult = manager.Free(ids[index]);
            ASSUME(freeResult);
        }
        else
        {
            ASSUME(false == manager.IsAllocated(index));
            bool freeResult = manager.Free(index);
            ASSUME(false == freeResult);
        }
    }

    for (ui32 id : ids)
    {
        bool result = manager.Allocate(id);
        ASSUME(result);
    }

    bool freeResult = manager.Free(requestSize / 2);
    ASSUME(freeResult);
    freeResult = manager.Free(requestSize * 2);
    ASSUME(freeResult == false);

    for (ui32 index = 0; index < requestSize * 2; ++index)
    {
        if (index < requestSize && index != requestSize / 2)
        {
            ASSUME(manager.IsAllocated(index));
        }
        else
        {
            ASSUME(false == manager.IsAllocated(index));
        }
    }

    ui32 maxId = 0;
    manager = {};
    std::unordered_set<ui32> randomIds;
    for (ui32 index = 0; index < requestSize; ++index)
    {
        ui32 id = rand() % requestSize;
        maxId = std::max(maxId, id);
        randomIds.emplace(id);
    }

    for (ui32 id : randomIds)
    {
        bool result = manager.Allocate(id);
        ASSUME(result);
    }

    for (ui32 id : randomIds)
    {
        bool result = manager.Allocate(id);
        ASSUME(result == false);
    }

    for (ui32 id = 0; id < maxId * 2; ++id)
    {
        bool result = manager.IsAllocated(id);
        bool isFound = randomIds.find(id) != randomIds.end();
        ASSUME(result == isFound);
    }

    for (ui32 index = 0; index < requestSize * 2; ++index)
    {
        ui32 allocated = manager.Allocate();
        bool isFound = randomIds.find(allocated) != randomIds.end();
        ASSUME(isFound == false);
    }

    for (ui32 id : randomIds)
    {
        bool result = manager.Free(id);
        ASSUME(result);
    }

    for (ui32 id : randomIds)
    {
        bool result = manager.IsAllocated(id);
        ASSUME(result == false);
    }

    PRINTLOG("finished unique id manager tests\n");
}
#pragma once

#include "FilePath.hpp"

namespace StdLib::FileSystem
{
    enum class ObjectType
    {
        File, Folder
    };

    // use move functions if you want to rename the file
    Result<ObjectType> Classify(const FilePath &sourcePnn); // use it to also check existence, if it doesn't exist, the result will be NotFound
    Error<> MoveTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace); // if moving across volumes, source removing is not guaranteed
    Error<> CopyTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace);
    Error<> Remove(const FilePath &path);
    Error<> VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen); // maxLen include null-symbol, if buffer was too small, returns a zero
    Result<bool> IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1); // the function may fail if it cannot open one of the files
    Result<bool> IsFolderEmpty(const FilePath &pnn);
    Result<bool> IsReadOnlyGet(const FilePath &pnn);
    Error<> IsReadOnlySet(const FilePath &pnn, bool isReadOnly);
    Error<> CreateNewFolder(const FilePath &where, const FilePath &name, bool isOverrideExisting);
    Result<FilePath> CurrentWorkingPathGet();
    Error<> CurrentWorkingPathSet(const FilePath &path);
}
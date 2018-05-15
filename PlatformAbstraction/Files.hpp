#pragma once

#include "FilePath.hpp"

namespace StdLib::Files
{
    // use move functions if you want to rename the file
    Error<> MoveFileTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false); // if moving across volumes, source removing is not guaranteed
    Error<> MoveFolderTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false); // if moving across volumes, source removing is not guaranteed
    Error<> MoveObjectTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false); // if moving across volumes, source removing is not guaranteed
    Error<> CopyFileTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false);
    Error<> CopyFolderTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false);
    Error<> CopyObjectTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace = false);
    Error<> RemoveFile(const FilePath &pnn);
    Error<> RemoveFolder(const FilePath &path);
    Error<> RemoveObject(const FilePath &path);
    Error<> VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen); // maxLen include null-symbol, if buffer was too small, returns a zero
    Result<bool> IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1); // the function may fail if it cannot open one of the files
    Result<bool> IsExists(const FilePath &pnn);
    Result<bool> IsFile(const FilePath &pnn);
    Result<bool> IsFolder(const FilePath &pnn);
    Result<bool> IsFolderEmpty(const FilePath &pnn);
    Result<bool> IsFileReadOnlyGet(const FilePath &pnn);
    Error<> IsFileReadOnlySet(const FilePath &pnn, bool is_ro);
    Error<> CreateNewFolder(const FilePath &where, const FilePath &name, bool isOverrideExistingObject = false);
    Error<> CreateNewFile(const FilePath &where, const FilePath &name, bool isOverrideExistingObject = false);
    Result<FilePath> CurrentWorkingPathGet();
    Error<> CurrentWorkingPathSet(const FilePath &path);
}
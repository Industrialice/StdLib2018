#include <StdCoreLib.hpp>
#include "FileSystem.hpp"
#include <Windows.h>

using namespace StdLib;

#if defined(_MSC_VER) && !defined(DEBUG)
    #pragma optimize( "s", on ) // TODO: global solution
#endif

extern NOINLINE Error<> StdLib_FileError()
{
    DWORD code = GetLastError();

    switch (code)
    {
    case ERROR_FILE_INVALID:
        return DefaultError::InvalidArgument();
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return DefaultError::NotFound();
    case ERROR_ACCESS_DENIED:
    case ERROR_WRITE_PROTECT:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
    case ERROR_USER_MAPPED_FILE:
        return DefaultError::AccessDenied();
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        return DefaultError::OutOfMemory();
    case ERROR_FILE_EXISTS:
    case ERROR_ALREADY_EXISTS:
        return DefaultError::AlreadyExists();
    default:
        return DefaultError::UnknownError();
    }
}

static Error<> RemoveFileInternal(const wchar_t *pnn);
static Error<> RemoveFolderInternal(const wchar_t *pnn);
static Error<> CopyFileInternal(const wchar_t *sourcePnn, const wchar_t *targetPnn);
static Error<> CopyFolderInternal(const wchar_t *sourcePnn, const wchar_t *targetPnn);

NOINLINE Error<> FileSystem::MoveTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    FilePath source = sourcePnn, target = targetPnn;
    BOOL wapiResult;
    DWORD flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH;
    if (isReplace)
    {
        flags |= MOVEFILE_REPLACE_EXISTING;
    }

    source.MakeAbsolute();
    target.MakeAbsolute();

    if (!source.IsValid() || !target.IsValid() || source == target)
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    wapiResult = MoveFileExW(source.PlatformPath().data(), target.PlatformPath().data(), flags);
    if (wapiResult != TRUE)
    {
        return DefaultError::UnknownError("MoveFileExW failed");
    }

    Remove(source);

    return DefaultError::Ok();
}

Error<> FileSystem::CopyTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    auto classifyResult = Classify(sourcePnn);
    if (!classifyResult)
    {
        return classifyResult.GetError();
    }

    if (!isReplace && Classify(targetPnn))
    {
        return DefaultError::AlreadyExists();
    }

    auto funcPtr = classifyResult.Unwrap() == ObjectType::File ? CopyFileInternal : CopyFolderInternal;
    return funcPtr(sourcePnn.PlatformPath().data(), targetPnn.PlatformPath().data());
}

auto FileSystem::Classify(const FilePath &sourcePnn) -> Result<ObjectType>
{
    DWORD attribs = GetFileAttributesW(sourcePnn.PlatformPath().data());
    if (attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    bool isFolder = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    return isFolder ? ObjectType::Folder : ObjectType::File;
}

Error<> FileSystem::Remove(const FilePath &path)
{
    auto type = Classify(path);
    if (!type)
    {
        return type.GetError();
    }

    bool isFile = (type.Unwrap() == ObjectType::File);
    if (isFile)
    {
        return RemoveFileInternal(path.PlatformPath().data());
    }
    return RemoveFolderInternal(path.PlatformPath().data());
}

Error<> FileSystem::VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen)
{
   // TODO:
    return DefaultError::NotImplemented();
}

NOINLINE Result<bool> FileSystem::IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1)
{
    HANDLE h0 = INVALID_HANDLE_VALUE, h1 = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION inf0, inf1;

    if (!pnn0.IsValid() || !pnn1.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    if (pnn0 == pnn1)
    {
        return true;
    }

    h0 = CreateFileW(pnn0.PlatformPath().data(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (h0 == INVALID_HANDLE_VALUE)
    {
        h0 = CreateFileW(pnn0.PlatformPath().data(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
        if (h0 == INVALID_HANDLE_VALUE)
        {
            return DefaultError::UnknownError("failed to open the first file");
        }
    }

    h1 = CreateFileW(pnn1.PlatformPath().data(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (h1 == INVALID_HANDLE_VALUE)
    {
        h1 = CreateFileW(pnn1.PlatformPath().data(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
        if (h1 == INVALID_HANDLE_VALUE)
        {
            CloseHandle(h0);
            return DefaultError::UnknownError("failed to open the second file");
        }
    }

    if (GetFileInformationByHandle(h0, &inf0) != TRUE)
    {
        CloseHandle(h0);
        CloseHandle(h1);
        return DefaultError::UnknownError("failed to retrieve file information of the first file");
    }

    if (GetFileInformationByHandle(h1, &inf1) != TRUE)
    {
        CloseHandle(h0);
        CloseHandle(h1);
        return DefaultError::UnknownError("failed to retrieve file information of the second file");
    }

    bool result = inf0.nFileIndexLow == inf1.nFileIndexLow;
    result &= inf0.nFileIndexHigh == inf1.nFileIndexHigh;
    result &= inf0.dwVolumeSerialNumber == inf1.dwVolumeSerialNumber;

    CloseHandle(h0);
    CloseHandle(h1);

    return result;
}

NOINLINE Result<bool> FileSystem::IsFolderEmpty(const FilePath &pnn)
{
    auto classified = FileSystem::Classify(pnn);
    if (!classified)
    {
        return classified.GetError();
    }

    if (classified.Unwrap() != ObjectType::Folder)
    {
        return DefaultError::InvalidArgument("is not a folder");
    }

    std::wstring buf;
    WIN32_FIND_DATAW dataFind;
    HANDLE hfind;
    bool isEmpty = true;

    buf = pnn.PlatformPath();
    buf += L"\\*";

    hfind = FindFirstFileW(buf.c_str(), &dataFind);
    if (hfind == INVALID_HANDLE_VALUE)
    {
        return DefaultError::UnknownError("FindFirstFileW failed");
    }
    do
    {
        if (!wcscmp(dataFind.cFileName, L".") || !wcscmp(dataFind.cFileName, L".."))
        {
            continue;
        }

        isEmpty = false;
        break;
    } while (FindNextFileW(hfind, &dataFind));

    FindClose(hfind);

    return isEmpty;
}

Result<bool> FileSystem::IsReadOnlyGet(const FilePath &pnn)
{
    DWORD attribs = GetFileAttributesW(pnn.PlatformPath().data());
    if (attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    return (attribs & FILE_ATTRIBUTE_READONLY) != 0;
}

NOINLINE Error<> FileSystem::IsReadOnlySet(const FilePath &pnn, bool isReadOnly)
{
    DWORD new_attribs;
    DWORD old_attribs = GetFileAttributesW(pnn.PlatformPath().data());
    if (old_attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    if (isReadOnly)
    {
        new_attribs = old_attribs | FILE_ATTRIBUTE_READONLY;
    }
    else
    {
        new_attribs = old_attribs & ~FILE_ATTRIBUTE_READONLY;
    }

    if (old_attribs != new_attribs)
    {
        if (SetFileAttributesW(pnn.PlatformPath().data(), new_attribs) != TRUE)
        {
            return StdLib_FileError();
        }
    }

    return DefaultError::Ok();
}

NOINLINE Error<> FileSystem::CreateNewFolder(const FilePath &where, const FilePath &name, bool isOverrideExisting)
{
    FilePath fullPath = where;
    fullPath.AddLevel();
    fullPath += name;

    if (FileSystem::Classify(fullPath)) // already exists
    {
        if (isOverrideExisting)
        {
            auto removeResult = FileSystem::Remove(fullPath);
            if (removeResult)
            {
                return removeResult;
            }
        }
        else
        {
            return DefaultError::AlreadyExists();
        }
    }

    if (CreateDirectoryW(fullPath.PlatformPath().data(), 0) != TRUE)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

Result<FilePath> FileSystem::CurrentWorkingPathGet()
{
    wchar_t tempBuf[MaxPathLength];

    DWORD result = GetCurrentDirectoryW(MaxPathLength, tempBuf);
    if (result > MaxPathLength) // if result bigger than MAX_PATH_LENGTH, MAX_PATH_LENGTH wasn't enough
    {
        return DefaultError::OutOfMemory("buffer size was insufficient");
    }
    if (result == 0)
    {
        return DefaultError::UnknownError("GetCurrentDirectoryW returned 0");
    }
    return FilePath(tempBuf);
}

Error<> FileSystem::CurrentWorkingPathSet(const FilePath &path)
{
    if (!path.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }
    BOOL result = SetCurrentDirectoryW(path.PlatformPath().data());
    if (result != TRUE)
    {
        return DefaultError::UnknownError("SetCurrentDirectoryW failed");
    }
    return DefaultError::Ok();
}

Error<> RemoveFileInternal(const wchar_t *pnn)
{
    BOOL result = DeleteFileW(pnn) != 0;
    if (result == false)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

Error<> RemoveFolderInternal(const wchar_t *pnn)
{
    uiw len;
    std::wstring buf;
    WIN32_FIND_DATAW dataFind;
    HANDLE hfind;

    buf = pnn;
    len = buf.size() + 1;
    buf += L"\\*";

    hfind = FindFirstFileW(buf.c_str(), &dataFind);
    if (hfind == INVALID_HANDLE_VALUE)
    {
        return DefaultError::UnknownError("FindFirstFileW failed");
    }
    do
    {
        if (!wcscmp(dataFind.cFileName, L".") || !wcscmp(dataFind.cFileName, L".."))
        {
            continue;
        }

        buf.resize(len);
        buf += dataFind.cFileName;
        if (dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            Error<> removeFolderResult = RemoveFolderInternal(buf.c_str());
            if (removeFolderResult)
            {
                FindClose(hfind);
                return removeFolderResult;
            }
        }
        else
        {
            Error<> removeFileResult = RemoveFileInternal(buf.c_str());
            if (removeFileResult)
            {
                FindClose(hfind);
                return removeFileResult;
            }
        }
    } while (FindNextFileW(hfind, &dataFind));

    FindClose(hfind);

    BOOL winResult = RemoveDirectoryW(pnn) != 0;
    if (winResult != TRUE)
    {
        return DefaultError::UnknownError("RemoveDirectoryW failed");
    }

    return DefaultError::Ok();
}

Error<> CopyFileInternal(const wchar_t *sourcePnn, const wchar_t *targetPnn)
{    
    // TODO: Windows 7, Windows Server 2008 R2, Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP: Security resource properties for the existing file are not copied to the new file until Windows 8 and Windows Server 2012.
    BOOL result = CopyFileW(sourcePnn, targetPnn, FALSE);
    if (result == FALSE)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

Error<> CopyFolderInternal(const wchar_t *sourcePnn, const wchar_t *targetPnn)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}
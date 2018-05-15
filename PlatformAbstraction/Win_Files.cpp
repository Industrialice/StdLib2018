#include <StdCoreLib.hpp>
#include "Files.hpp"
#include <Windows.h>

using namespace StdLib;

#if defined(_MSC_VER) && !defined(DEBUG)
    #pragma optimize( "s", on ) // TODO: global solution
#endif

NOINLINE Error<> StdLib_FileError()
{
    DWORD code = GetLastError();

    switch (code)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return DefaultError::NotFound();
    case ERROR_ACCESS_DENIED:
    case ERROR_WRITE_PROTECT:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
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

Error<> Files::MoveFileTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    Result<bool> isFile = Files::IsFile(sourcePnn); // will check the pnn
    if (!isFile)
    {
        return isFile.GetError();
    }
    if (!isFile.Unwrap())
    {
        return DefaultError::InvalidArgument("source is not a file");
    }

    return Files::MoveObjectTo(sourcePnn, targetPnn, isReplace); // will check the pnn
}

Error<> Files::MoveFolderTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    Result<bool> isFolder = Files::IsFolder(sourcePnn); // will check the pnn
    if (!isFolder)
    {
        return isFolder.GetError();
    }
    if (!isFolder.Unwrap())
    {
        return DefaultError::InvalidArgument("source is not a folder");
    }

    return Files::MoveObjectTo(sourcePnn, targetPnn, isReplace); // will check the pnn
}

NOINLINE Error<> Files::MoveObjectTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
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

    return DefaultError::Ok();
}

Error<> Files::CopyFileTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    Result<bool> isFile = Files::IsFile(sourcePnn); // will check the pnn
    if (!isFile)
    {
        return isFile.GetError();
    }
    if (!isFile.Unwrap())
    {
        return DefaultError::InvalidArgument("source is not a file");
    }

    return CopyObjectTo(sourcePnn, targetPnn, isReplace); // will check the pnn
}

Error<> Files::CopyFolderTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    Result<bool> isFolder = Files::IsFolder(sourcePnn); // will check the pnn
    if (!isFolder)
    {
        return isFolder.GetError();
    }
    if (!isFolder.Unwrap())
    {
        return DefaultError::InvalidArgument("source is not a folder");
    }

    return CopyObjectTo(sourcePnn, targetPnn, isReplace); // will check the pnn
}

Error<> Files::CopyObjectTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    if (!sourcePnn.IsValid() || !targetPnn.IsValid())
    {
        return DefaultError::InvalidArgument("source or target path isn't valid");
    }

   // TODO: Windows 7, Windows Server 2008 R2, Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP: Security resource properties for the existing file are not copied to the new file until Windows 8 and Windows Server 2012.
    BOOL result = ::CopyFileW(sourcePnn.PlatformPath().data(), targetPnn.PlatformPath().data(), isReplace ? FALSE : TRUE);
    if (result == FALSE)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

NOINLINE Error<> Files::RemoveFile(const FilePath &pnn)
{
    Result<bool> isFile = Files::IsFile(pnn); // will check the pnn
    if (!isFile)
    {
        return isFile.GetError();
    }
    if (!isFile.Unwrap())
    {
        return DefaultError::InvalidArgument("pnn doesn't point to a file");
    }

    BOOL result = DeleteFileW(pnn.PlatformPath().data()) != 0;
    if (result == false)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

NOINLINE Error<> Files::RemoveFolder(const FilePath &path) // potentially recursive
{
    uiw len;
    std::wstring buf;
    WIN32_FIND_DATAW dataFind;
    HANDLE hfind;

    Result<bool> isFolder = Files::IsFolder(path); // will check the pnn
    if (!isFolder)
    {
        return isFolder.GetError();
    }
    if (!isFolder.Unwrap())
    {
        return DefaultError::InvalidArgument("is not a folder");
    }

    buf = path.PlatformPath();
    len = buf.size() + 1;
    buf += L"\\*";

    hfind = FindFirstFileW(buf.c_str(), &dataFind);
    if (hfind == INVALID_HANDLE_VALUE)
    {
        return DefaultError::UnknownError("FindFirstFileW failed");
    }
    do
    {
        if (!::wcscmp(dataFind.cFileName, L".") || !::wcscmp(dataFind.cFileName, L".."))
        {
            continue;
        }

        buf.resize(len);
        buf += dataFind.cFileName;
        if (dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            Error<> removeFolderResult = RemoveFolder(buf.c_str());
            if (removeFolderResult)
            {
                FindClose(hfind);
                return removeFolderResult;
            }
        }
        else
        {
            Error<> removeFileResult = RemoveFile(buf.c_str());
            if (removeFileResult)
            {
                FindClose(hfind);
                return removeFileResult;
            }
        }
    } while (FindNextFileW(hfind, &dataFind));

    FindClose(hfind);

    BOOL winResult = RemoveDirectoryW(path.PlatformPath().data()) != 0;
    if (winResult != TRUE)
    {
        return DefaultError::UnknownError("RemoveDirectoryW failed");
    }

    return DefaultError::Ok();
}

Error<> Files::RemoveObject(const FilePath &path)
{
    Result<bool> isFile = Files::IsFile(path);
    if (isFile && isFile.Unwrap())
    {
        return Files::RemoveFile(path);
    }

    Result<bool> isFolder = Files::IsFolder(path);
    if (isFolder && isFolder.Unwrap())
    {
        return Files::RemoveFolder(path);
    }

    if (!isFile)
    {
        return isFile.GetError();
    }
    if (!isFolder)
    {
        return isFolder.GetError();
    }

    return DefaultError::UnknownError();
}

Error<> Files::VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen)
{
   // TODO:
    return DefaultError::NotImplemented();
}

NOINLINE Result<bool> Files::IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1)
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

NOINLINE Result<bool> Files::IsExists(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }
    DWORD attribs = GetFileAttributesW(pnn.PlatformPath().data());
    return attribs != INVALID_FILE_ATTRIBUTES;
}

Result<bool> Files::IsFile(const FilePath &pnn)
{
    DWORD attribs;

    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    attribs = GetFileAttributesW(pnn.PlatformPath().data());
    if (attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    return ((attribs & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

Result<bool> Files::IsFolder(const FilePath &pnn)
{
    auto isFile = Files::IsFile(pnn);
    if (!isFile)
    {
        return isFile;
    }

    return isFile.Unwrap() == false;
}

Result<bool> Files::IsFolderEmpty(const FilePath &pnn)
{
    auto isFile = Files::IsFile(pnn);
    if (!isFile)
    {
        return isFile;
    }

    if (isFile.Unwrap())
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
        if (!::wcscmp(dataFind.cFileName, L".") || !::wcscmp(dataFind.cFileName, L".."))
        {
            continue;
        }

        isEmpty = false;
        break;
    } while (FindNextFileW(hfind, &dataFind));

    FindClose(hfind);

    return isEmpty;
}

NOINLINE Result<bool> Files::IsFileReadOnlyGet(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    DWORD attribs = GetFileAttributesW(pnn.PlatformPath().data());
    if (attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    return (attribs & FILE_ATTRIBUTE_READONLY) != 0;
}

NOINLINE Error<> Files::IsFileReadOnlySet(const FilePath &pnn, bool isReadOnly)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

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

NOINLINE Error<> Files::CreateNewFolder(const FilePath &where, const FilePath &name, bool isOverrideExistingObject)
{
    FilePath fullPath = where;
    fullPath.AddLevel();
    fullPath += name;

    auto isExists = Files::IsExists(fullPath); // will check the path
    if (!isExists)
    {
        return isExists.GetError();
    }

    if (isExists.Unwrap())
    {
        if (isOverrideExistingObject)
        {
            auto removeResult = Files::RemoveObject(fullPath);
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

NOINLINE Error<> Files::CreateNewFile(const FilePath &where, const FilePath &name, bool isOverrideExistingObject)
{
    FilePath fullPath = where;
    fullPath.AddLevel();
    fullPath += name;

    auto isExists = Files::IsExists(fullPath); // will check the path
    if (!isExists)
    {
        return isExists.GetError();
    }

    if (isExists.Unwrap())
    {
        if (isOverrideExistingObject)
        {
            auto removeResult = Files::RemoveObject(fullPath);
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

    HANDLE file = CreateFileW(fullPath.PlatformPath().data(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, isOverrideExistingObject ? CREATE_ALWAYS : CREATE_NEW, 0, 0);
    if (file == INVALID_HANDLE_VALUE)
    {
        return StdLib_FileError();
    }

    CloseHandle(file);

    return DefaultError::Ok();
}

Result<FilePath> Files::CurrentWorkingPathGet()
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

Error<> Files::CurrentWorkingPathSet(const FilePath &path)
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
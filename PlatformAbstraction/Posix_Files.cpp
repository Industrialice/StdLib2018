#include <StdCoreLib.hpp>
#include "Files.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

using namespace StdLib;

extern NOINLINE Error<> StdLib_FileError()
{
    int code = errno;

    switch (code)
    {
    case EPERM:
        return DefaultError::Unsupported();
    case ENAMETOOLONG:
    case ENOENT:
    case ENOTDIR:
    case EINVAL:
    case EBADF:
        return DefaultError::InvalidArgument();
    case EACCES:
    case EFAULT:
        return DefaultError::AccessDenied();
    case EDQUOT:
    case ELOOP:
    case EMLINK:
    case ENOMEM:
    case ENOSPC:
    case EOVERFLOW:
        return DefaultError::OutOfMemory();
    case EEXIST:
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
    /*FilePath source = sourcePnn, target = targetPnn;
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

    return DefaultError::Ok();*/
    NOIMPL;
    return DefaultError::NotImplemented();
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

    NOIMPL;
    return DefaultError::NotImplemented();

    // TODO: Windows 7, Windows Server 2008 R2, Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP: Security resource properties for the existing file are not copied to the new file until Windows 8 and Windows Server 2012.
    /*BOOL result = CopyFileW(sourcePnn.PlatformPath().data(), targetPnn.PlatformPath().data(), isReplace ? FALSE : TRUE);
    if (result == FALSE)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();*/
}

NOINLINE Error<> Files::RemoveFile(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    if (unlink(pnn.PlatformPath().data()) != 0)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}

NOINLINE Error<> Files::RemoveFolder(const FilePath &path) // potentially recursive
{
    if (!path.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    DIR *dirHandle;
    struct dirent *direntDir;

    char currentPath[MaxPathLength];
    uiw originalPathLength = path.PlatformPath().length();
    ASSUME(originalPathLength <= MaxPathLength);
    strcpy(currentPath, path.PlatformPath().data());

    dirHandle = opendir(currentPath);
    if (!dirHandle)
    {
        return StdLib_FileError();
    }

    for (; ; )
    {
        direntDir = readdir(dirHandle);
        if (!direntDir)
        {
            break;
        }

        if (!strcmp(direntDir->d_name, ".") || !strcmp(direntDir->d_name, ".."))
        {
            continue;
        }

        strcpy(currentPath + originalPathLength, direntDir->d_name);

        struct stat st;
        if (stat(currentPath, &st) != 0)
        {
            closedir(dirHandle);
            return StdLib_FileError();
        }

        if (S_ISDIR(st.st_mode))
        {
            if (auto removeError = Files::RemoveFolder(FilePath::FromChar(currentPath)); removeError)
            {
                closedir(dirHandle);
                return removeError;
            }
        }
        else
        {
            ASSUME(S_ISREG(st.st_mode));
            if (auto removeError = Files::RemoveFile(FilePath::FromChar(currentPath)); removeError)
            {
                closedir(dirHandle);
                return removeError;
            }
        }
    }

    closedir(dirHandle);

    if (rmdir(path.PlatformPath().data()) != 0)
    {
        return StdLib_FileError();
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
    NOIMPL;
    return DefaultError::NotImplemented();
}

NOINLINE Result<bool> Files::IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1)
{
    /*HANDLE h0 = INVALID_HANDLE_VALUE, h1 = INVALID_HANDLE_VALUE;
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

    return result;*/
    NOIMPL;
    return DefaultError::NotImplemented();
}

NOINLINE Result<bool> Files::IsExists(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }
    struct stat st;
    if (stat(pnn.PlatformPath().data(), &st) != 0)
    {
        if (errno == ENOENT) // does not exist
        {
            return false;
        }
        return StdLib_FileError();
    }
    auto mode = st.st_mode & S_IFMT;
    return mode == S_IFDIR || mode == S_IFREG;
}

Result<bool> Files::IsFile(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }
    struct stat st;
    if (stat(pnn.PlatformPath().data(), &st) != 0)
    {
        return StdLib_FileError();
    }
    auto mode = st.st_mode & S_IFMT;
    return mode == S_IFREG;
}

Result<bool> Files::IsFolder(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }
    struct stat st;
    if (stat(pnn.PlatformPath().data(), &st) != 0)
    {
        return StdLib_FileError();
    }
    auto mode = st.st_mode & S_IFMT;
    return mode == S_IFDIR;
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

    NOIMPL;
    return DefaultError::NotImplemented();

    /*std::wstring buf;
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

    return isEmpty;*/
}

NOINLINE Result<bool> Files::IsFileReadOnlyGet(const FilePath &pnn)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    NOIMPL;
    return DefaultError::NotImplemented();

    /*DWORD attribs = GetFileAttributesW(pnn.PlatformPath().data());
    if (attribs == INVALID_FILE_ATTRIBUTES)
    {
        return StdLib_FileError();
    }

    return (attribs & FILE_ATTRIBUTE_READONLY) != 0;*/
}

NOINLINE Error<> Files::IsFileReadOnlySet(const FilePath &pnn, bool isReadOnly)
{
    if (!pnn.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    NOIMPL;
    return DefaultError::NotImplemented();

    /*DWORD new_attribs;
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

    return DefaultError::Ok();*/
}

NOINLINE Error<> Files::CreateNewFolder(const FilePath &where, const FilePath &name, bool isOverrideExistingObject)
{
    FilePath fullPath = where;
    fullPath.AddLevel();
    fullPath += name;

    mode_t processMask = umask(0);

    int code = mkdir(fullPath.PlatformPath().data(), S_IRWXU | S_IRWXG | S_IRWXO);
    if (code != 0) // failed to create a directory
    {
        int error = errno;
        if (error == EEXIST) // because an object with such path already exists
        {
            if (!isOverrideExistingObject)
            {
                umask(processMask);
                return DefaultError::AlreadyExists();
            }

            auto removeResult = RemoveObject(fullPath); // remove the old object
            if (removeResult)
            {
                umask(processMask);
                return removeResult;
            }

            if (mkdir(fullPath.PlatformPath().data(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) // and try again
            {
                umask(processMask);
                return StdLib_FileError();
            }
        }
        else // some other error, return it
        {
            umask(processMask);
            return StdLib_FileError();
        }
    }

    umask(processMask);
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

    NOIMPL;
    return DefaultError::NotImplemented();

    /*HANDLE file = CreateFileW(fullPath.PlatformPath().data(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, isOverrideExistingObject ? CREATE_ALWAYS : CREATE_NEW, 0, 0);
    if (file == INVALID_HANDLE_VALUE)
    {
        return StdLib_FileError();
    }

    CloseHandle(file);

    return DefaultError::Ok();*/
}

Result<FilePath> Files::CurrentWorkingPathGet()
{
    char path[MaxFileNameLength];
    char *result = getcwd(path, MaxFileNameLength);
    if (!result)
    {
        return StdLib_FileError();
    }
    return FilePath::FromChar(path);
}

Error<> Files::CurrentWorkingPathSet(const FilePath &path)
{
    if (!path.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    if (chdir(path.PlatformPath().data()) != 0)
    {
        return StdLib_FileError();
    }

    return DefaultError::Ok();
}
#include "_PreHeader.hpp"
#include "FileSystem.hpp"
#include "PlatformErrorResolve.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

#undef __USE_FILE_OFFSET64
#include <sys/sendfile.h> /* using 64 bit offsets will pick sendfile64 which isn't available for APIs below 21 */

using namespace StdLib;

static Error<> RemoveFileInternal(const char *pnn);
static Error<> RemoveFolderInternal(const char *pnn);
static Error<> CopyFileInternal(const char *sourcePnn, const char *targetPnn);
static Error<> CopyFolderInternal(const char *sourcePnn, const char *targetPnn);

NOINLINE Error<> FileSystem::MoveTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
{
    if (!isReplace)
    {
        if (Classify(targetPnn))
        {
            return DefaultError::AlreadyExists("object at target path already exists");
        }
    }

    if (rename(sourcePnn.PlatformPath().data(), targetPnn.PlatformPath().data()) != 0)
    {
        return PlatformErrorResolve();
    }

    return DefaultError::Ok();
}

NOINLINE Error<> FileSystem::CopyTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace)
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
    struct stat st;
    if (stat(sourcePnn.PlatformPath().data(), &st) != 0)
    {
        return PlatformErrorResolve();
    }
    auto mode = st.st_mode & S_IFMT;
    if (mode == S_IFREG)
    {
        return ObjectType::File;
    }
    if (mode == S_IFDIR)
    {
        return ObjectType::Folder;
    }
    return DefaultError::UnknownError("stat returned unknown mode");
}

Error<> FileSystem::Remove(const FilePath &pnn)
{
    auto type = Classify(pnn);
    if (!type)
    {
        return type.GetError();
    }

    bool isFile = (type.Unwrap() == ObjectType::File);
    if (isFile)
    {
        return RemoveFileInternal(pnn.PlatformPath().data());
    }
    return RemoveFolderInternal(pnn.PlatformPath().data());
}

Error<> FileSystem::VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}

NOINLINE Result<bool> FileSystem::IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1)
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

    bool isFilesFound = false;

    DIR *dirHandle = opendir(pnn.PlatformPath().data());
    if (!dirHandle)
    {
        return PlatformErrorResolve();
    }

    for (; ; )
    {
        struct dirent *direntDir = readdir(dirHandle);
        if (!direntDir)
        {
            break;
        }

        if (!strcmp(direntDir->d_name, ".") || !strcmp(direntDir->d_name, ".."))
        {
            continue;
        }

        isFilesFound = true;
        break;
    }

    closedir(dirHandle);
    return !isFilesFound;
}

Result<bool> FileSystem::IsReadOnly(const FilePath &pnn)
{
    struct stat fileStats;
    if (stat(pnn.PlatformPath().data(), &fileStats) != 0)
    {
        return PlatformErrorResolve();
    }

    mode_t mode = fileStats.st_mode & ~S_IFMT;
    return mode == (S_IRUSR | S_IRGRP | S_IROTH);
}

Error<> FileSystem::IsReadOnly(const FilePath &pnn, bool isReadOnly)
{
    mode_t mode = isReadOnly ? (S_IRUSR | S_IRGRP | S_IROTH) : (S_IRWXU | S_IRWXG | S_IRWXO);
    if (chmod(pnn.PlatformPath().data(), mode) != 0)
    {
        return PlatformErrorResolve();
    }
    return DefaultError::Ok();
}

NOINLINE Error<> FileSystem::CreateFolder(const FilePath &where, const FilePath &name, bool isOverrideExisting)
{
    FilePath fullPath = where;
    fullPath.AddLevel();
    fullPath += name;
    fullPath.Normalize();

    mode_t processMask = umask(0);

    int code = mkdir(fullPath.PlatformPath().data(), S_IRWXU | S_IRWXG | S_IRWXO);
    if (code != 0) // failed to create a directory
    {
        int error = errno;
        if (error == EEXIST) // because an object with such path already exists
        {
            if (!isOverrideExisting)
            {
                umask(processMask);
                return DefaultError::AlreadyExists();
            }

            auto removeResult = Remove(fullPath); // remove the old object
            if (removeResult)
            {
                umask(processMask);
                return removeResult;
            }

            if (mkdir(fullPath.PlatformPath().data(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) // and try again
            {
                umask(processMask);
                return PlatformErrorResolve();
            }
        }
        else // some other error, return it
        {
            umask(processMask);
            return PlatformErrorResolve();
        }
    }

    umask(processMask);
    return DefaultError::Ok();
}

Result<FilePath> FileSystem::CurrentWorkingPathGet()
{
    char path[MaxFileNameLength];
    char *result = getcwd(path, MaxFileNameLength);
    if (!result)
    {
        return PlatformErrorResolve();
    }
    return FilePath::FromChar(path);
}

Error<> FileSystem::CurrentWorkingPathSet(const FilePath &path)
{
    if (!path.IsValid())
    {
        return DefaultError::InvalidArgument("invalid path");
    }

    if (chdir(path.PlatformPath().data()) != 0)
    {
        return PlatformErrorResolve();
    }

    return DefaultError::Ok();
}

// assume that all regular files are also symbolic links
Error<> FileSystem::Enumerate(const FilePath &path, const std::function<void(const FileEnumInfo &info)> &callback, EnumerateOptions::EnumerateOption options)
{
	if (options.Contains(EnumerateOptions::ReportFiles) == false && options.Contains(EnumerateOptions::ReportFolders) == false)
	{
		return DefaultError::InvalidArgument("Neither ReportFiles, nor ReportFolders is specified");
	}

	static_assert(sizeof(struct dirent) == sizeof(FileEnumInfo));

	DIR *dir = opendir(path.PlatformPath().data());
	if (dir == nullptr)
	{
		return PlatformErrorResolve("opendir failed");
	}

    struct dirent *data = readdir(dir);
	if (data == nullptr)
    {
	    return PlatformErrorResolve("First readdir failed");
    }

	for (; data != nullptr; data = readdir(dir))
    {
        if (!strcmp(data->d_name, ".") || !strcmp(data->d_name, ".."))
        {
            continue;
        }

        if (data->d_type & DT_DIR)
        {
            if (options.Contains(EnumerateOptions::ReportFolders))
            {
                callback(reinterpret_cast<FileEnumInfo &>(*data));
            }

            if (options.Contains(EnumerateOptions::Recursive))
            {
                auto error = Enumerate(FilePath(path).AddLevel().Append(data->d_name), callback, options);
                ASSUME(error != DefaultError::NotFound());
            }
        }
        else if (data->d_type & DT_REG)
        {
            if (options.Contains(EnumerateOptions::ReportFiles))
            {
                callback(reinterpret_cast<FileEnumInfo &>(*data));
            }
        }
    }

    closedir(dir);

	return DefaultError::Ok();
}

Error<> RemoveFileInternal(const char *pnn)
{
    if (unlink(pnn) != 0)
    {
        return PlatformErrorResolve();
    }

    return DefaultError::Ok();
}

Error<> RemoveFolderInternal(const char *pnn)
{
    char currentPath[MaxPathLength];
    uiw originalPathLength = strlen(pnn);
    ASSUME(originalPathLength <= MaxPathLength);
    strcpy(currentPath, pnn);
    if (originalPathLength != 0 && currentPath[originalPathLength - 1] != '/')
    {
        currentPath[originalPathLength] = '/';
        ++originalPathLength;
        currentPath[originalPathLength] = '\0';
    }

    DIR *dirHandle = opendir(currentPath);
    if (!dirHandle)
    {
        return PlatformErrorResolve();
    }

    for (;;)
    {
        struct dirent *direntDir = readdir(dirHandle);
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
            return PlatformErrorResolve();
        }

        if (S_ISDIR(st.st_mode))
        {
            strcat(currentPath + originalPathLength, "/");
            if (auto removeError = RemoveFolderInternal(currentPath); removeError)
            {
                closedir(dirHandle);
                return removeError;
            }
        }
        else
        {
            ASSUME(S_ISREG(st.st_mode));
            if (auto removeError = RemoveFileInternal(currentPath); removeError)
            {
                closedir(dirHandle);
                return removeError;
            }
        }
    }

    closedir(dirHandle);

    if (rmdir(pnn) != 0)
    {
        return PlatformErrorResolve();
    }

    return DefaultError::Ok();
}

Error<> CopyFileInternal(const char *sourcePnn, const char *targetPnn)
{
    int sourceFile = open(sourcePnn, O_RDONLY, 0);
    if (sourceFile == -1)
    {
        return PlatformErrorResolve();
    }

    struct stat64 statSource;
    if (fstat64(sourceFile, &statSource) != 0)
    {
        close(sourceFile);
        return PlatformErrorResolve();
    }

    mode_t processMask = umask(0);
    int targetFile = open(targetPnn, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (targetFile == -1)
    {
        close(sourceFile);
        return PlatformErrorResolve();
    }

    int copyFileResult = 0;

#ifdef PLATFORM_EMSCRIPTEN
    char localBuffer[8192];
    auto sizeLeft = statSource.st_size;
    while (sizeLeft > 0)
    {
        auto actuallyRead = read(sourceFile, localBuffer, CountOf(localBuffer));
        if (actuallyRead == 0)
        {
            break;
        }
        if (actuallyRead == static_cast<ssize_t>(-1))
        {
            copyFileResult = -1;
            break;
        }
        auto actuallyWritten = write(targetFile, localBuffer, actuallyRead);
        if (actuallyWritten == static_cast<ssize_t>(-1) || actuallyWritten < actuallyRead)
        {
            copyFileResult = -1;
            break;
        }
        ASSUME(sizeLeft >= actuallyRead);
        sizeLeft -= actuallyRead;
    }
#else
    copyFileResult = sendfile(targetFile, sourceFile, 0, statSource.st_size);
#endif
    close(sourceFile);
    close(targetFile);

    if (copyFileResult == -1)
    {
        return PlatformErrorResolve();
    }

    return DefaultError::Ok();
}

Error<> CopyFolderInternal(const char *sourcePnn, const char *targetPnn)
{
    NOIMPL;
    return DefaultError::NotImplemented();
}
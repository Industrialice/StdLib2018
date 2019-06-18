#pragma once

#include "FilePath.hpp"

namespace StdLib::FileSystem
{
    enum class ObjectType
    {
        File, Folder
    };

	struct EnumerateOptions
	{
		static constexpr struct EnumerateOption : EnumCombinable<EnumerateOption, ui8>
		{} Recursive = EnumerateOption::Create(1 << 0),
			ReportFiles = EnumerateOption::Create(1 << 1),
			ReportFolders = EnumerateOption::Create(1 << 2),
			FollowSymbolicLinks = EnumerateOption::Create(1 << 3),
			SkipHidden = EnumerateOption::Create(1 << 4);
	};

    // use move functions if you want to rename the file
	[[nodiscard]] Result<ObjectType> Classify(const FilePath &sourcePnn); // use it to also check existence, if it doesn't exist, the result will be NotFound
	Error<> MoveTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace); // if moving across volumes, source removing is not guaranteed
	Error<> CopyTo(const FilePath &sourcePnn, const FilePath &targetPnn, bool isReplace);
	Error<> Remove(const FilePath &path);
	[[nodiscard]] Error<> VolumeDriveName(const FilePath &path, char *RSTR output, uiw maxLen); // maxLen include null-symbol, if buffer was too small, returns a zero
	[[nodiscard]] Result<bool> IsPointToTheSameFile(const FilePath &pnn0, const FilePath &pnn1); // the function may fail if it cannot open one of the files
	[[nodiscard]] Result<bool> IsFolderEmpty(const FilePath &pnn);
	[[nodiscard]] Result<bool> IsReadOnly(const FilePath &pnn);
	Error<> IsReadOnly(const FilePath &pnn, bool isReadOnly);
	Error<> CreateFolder(const FilePath &where, const FilePath &name, bool isOverrideExisting);
	[[nodiscard]] Result<FilePath> CurrentWorkingPathGet();
	Error<> CurrentWorkingPathSet(const FilePath &path);
	Error<> Enumerate(const FilePath &path, const std::function<void(const FileEnumInfo &info)> &callback, EnumerateOptions::EnumerateOption options = EnumerateOptions::Recursive.Combined(EnumerateOptions::ReportFiles).Combined(EnumerateOptions::ReportFolders));
}
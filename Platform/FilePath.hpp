#pragma once

namespace StdLib
{
    // std::filesystem is still not available on platforms like Android
    class FilePath
    {
        pathString _path;

    public:
        // including null-terminator
        static constexpr uiw maxPathLength = MaxPathLength;
        static constexpr uiw maxFileNameLength = MaxFileNameLength;

        FilePath() {}
        FilePath(pathStringView path) : _path(path) {}
		FilePath(const PathChar *path) : _path(path) {}
		FilePath(PathChar ch) : _path(&ch, 1) {}
        FilePath(const pathString &path) : _path(path) {}
        FilePath(pathString &&path) : _path(std::move(path)) {}

        // the source will be simply copied, it won't be treated as UTF-8
		[[nodiscard]] static FilePath FromChar(std::string_view path);
		[[nodiscard]] static FilePath FromChar(const char *path);
		[[nodiscard]] static FilePath FromChar(char ch);
		[[nodiscard]] static FilePath FromChar(const std::string &path);
		[[nodiscard]] static FilePath FromChar(std::string &&path);

		[[nodiscard]] pathStringView PlatformPath() const { ASSUME(!_path[_path.size()]); return _path; } // always null-terminated

		FilePath &Append(pathStringView path) { _path += path; return *this; }
		FilePath &Append(const PathChar *path) { _path += path; return *this; }
		FilePath &Append(PathChar ch) { _path += ch; return *this; }
		FilePath &Append(const FilePath &path) { _path += path._path; return *this; }
		FilePath &Append(const pathString &path) { _path += path; return *this; }

        // simply concatenate
        FilePath &operator += (pathStringView path) { _path += path; return *this; }
        FilePath &operator += (const PathChar *path) { _path += path; return *this; }
        FilePath &operator += (PathChar ch) { _path += ch; return *this; }
        FilePath &operator += (const FilePath &path) { _path += path._path; return *this; }
        FilePath &operator += (const pathString &path) { _path += path;	return *this; }
		[[nodiscard]] FilePath operator + (pathStringView path) const { return FilePath(_path + pathString{path}); }
		[[nodiscard]] FilePath operator + (const PathChar *path) const { return FilePath(_path + pathString{path}); }
		[[nodiscard]] FilePath operator + (PathChar ch) const { return FilePath(_path + ch); }
		[[nodiscard]] FilePath operator + (const FilePath &path) const { return FilePath(_path + path._path); }
		[[nodiscard]] FilePath operator + (const pathString &path) const { return FilePath(_path + path); }
		[[nodiscard]] friend FilePath operator + (pathStringView left, const FilePath &right) { return FilePath(pathString{left} +right._path); }
		[[nodiscard]] friend FilePath operator + (const PathChar *left, const FilePath &right) { return FilePath(left + right._path); }
		[[nodiscard]] friend FilePath operator + (PathChar left, const FilePath &right) { return FilePath(left + right._path); }
		[[nodiscard]] friend FilePath operator + (const pathString &left, const FilePath &right) { return FilePath(left + right._path); }

        // add a new level
		[[nodiscard]] FilePath operator / (pathStringView other) const { auto copy = *this; copy.AddLevel(); return copy + other; }
		[[nodiscard]] FilePath operator / (const PathChar *other) const { auto copy = *this; copy.AddLevel(); return copy + other; }
		[[nodiscard]] FilePath operator / (PathChar other) const { auto copy = *this; copy.AddLevel(); return copy + other; }
		[[nodiscard]] FilePath operator / (const FilePath &other) const { auto copy = *this; copy.AddLevel(); return copy + other; }
		[[nodiscard]] FilePath operator / (const pathString &other) const { auto copy = *this; copy.AddLevel(); return copy + other; }
        FilePath &operator /= (pathStringView other) { AddLevel(); return *this += other; }
		FilePath &operator /= (const PathChar *other) { AddLevel(); return *this += other; }
		FilePath &operator /= (PathChar other) { AddLevel(); return *this += other; }
        FilePath &operator /= (const FilePath &other) { AddLevel(); return *this += other; }
        FilePath &operator /= (const pathString &other) { AddLevel(); return *this += other; }
		[[nodiscard]] friend FilePath operator / (pathStringView left, const FilePath &right) { return FilePath{left} / right; }
		[[nodiscard]] friend FilePath operator / (const PathChar *left, const FilePath &right) { return FilePath{ left } / right; }
		[[nodiscard]] friend FilePath operator / (PathChar left, const FilePath &right) { return FilePath{left} / right; }
		[[nodiscard]] friend FilePath operator / (const pathString &left, const FilePath &right) { return FilePath{left} / right; }

        FilePath &operator = (pathStringView path) { _path = path; return *this; }
		FilePath &operator = (const PathChar *path) { _path = path; return *this; }
		FilePath &operator = (PathChar path) { _path = path; return *this; }
        FilePath &operator = (const pathString &path) { _path = path; return *this; }
        FilePath &operator = (pathString &&path) { _path = std::move(path); return *this; }

		[[nodiscard]] bool operator == (pathStringView path) const { return _path == path; }
		[[nodiscard]] bool operator == (const PathChar *path) const { return _path == path; }
		[[nodiscard]] bool operator == (PathChar path) const { return _path == pathStringView(&path, 1); }
		[[nodiscard]] bool operator == (const FilePath &path) const { return _path == path._path; }
		[[nodiscard]] bool operator == (const pathString &path) const { return _path == path; }
		[[nodiscard]] bool operator != (pathStringView path) const { return _path != path; }
		[[nodiscard]] bool operator != (const PathChar *path) const { return _path != path; }
		[[nodiscard]] bool operator != (PathChar path) const { return _path != pathStringView(&path, 1); }
		[[nodiscard]] bool operator != (const FilePath &path) const { return _path != path._path; }
		[[nodiscard]] bool operator != (const pathString &path) const { return _path != path; }

		[[nodiscard]] bool operator < (const FilePath &other) const { return _path < other._path; } // so it can be used in containers

		FilePath &AddLevel(); // ignored if the path ends with a path delimiter
		FilePath &RemoveLevel(); /* does nothing if empty, C:/Pictures/ becomes C:/, C:/Pictures becomes C:/ */
		FilePath GetWithRemovedLevel() const;

		FilePath &RemoveTopLevel(); // C:/Pictures becomes Pictures
		FilePath GetWithRemovedTopLevel() const;

		[[nodiscard]] bool IsEmpty() const;
		[[nodiscard]] uiw Length() const;
		FilePath &Normalize();
		[[nodiscard]] FilePath GetNormalized() const;
		[[nodiscard]] bool IsValid() const; // false if too big or isn't supported by the platform

		FilePath &MakeAbsolute();
		[[nodiscard]] FilePath GetAbsolute() const;
		[[nodiscard]] bool IsAbsolute() const;

        // without extension
		[[nodiscard]] bool HasFileName() const;
		[[nodiscard]] std::optional<FilePath> FileName(std::optional<pathStringView> defaultName = std::nullopt) const;
		[[nodiscard]] std::optional<pathStringView> FileNameView(std::optional<pathStringView> defaultName = std::nullopt) const;
        void ReplaceFileName(pathStringView newName); // does nothing if both HasFileName() and HasExtension() are false

        // methods return and take extension without .
		[[nodiscard]] bool HasExtension() const;
		[[nodiscard]] std::optional<FilePath> Extension(std::optional<pathStringView> defaultName = std::nullopt) const;
		[[nodiscard]] std::optional<pathStringView> ExtensionView(std::optional<pathStringView> defaultName = std::nullopt) const;
        void ReplaceExtension(pathStringView newExt); // does nothing if both HasFileName() and HasExtension() are false

		// file name with exntension
		[[nodiscard]] bool HasFileNameWithExtension() const;
		[[nodiscard]] std::optional <FilePath> FileNameWithExtension(std::optional<pathStringView> defaultName = std::nullopt) const;
		[[nodiscard]] std::optional <pathStringView> FileNameWithExtensionView(std::optional<pathStringView> defaultName = std::nullopt) const;
        void ReplaceFileNameWithExtension(pathStringView newName); // does nothing if HasFileNameExt() is false
    };
}

namespace std
{
	template <> struct hash<StdLib::FilePath>
	{
		[[nodiscard]] size_t operator()(const StdLib::FilePath &value) const
		{
			return hash<StdLib::pathStringView>()(value.PlatformPath());
		}
	};
}
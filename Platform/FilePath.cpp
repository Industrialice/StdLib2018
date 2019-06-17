#include "_PreHeader.hpp"
#include "FilePath.hpp"

using namespace StdLib;

#ifdef PLATFORM_WINDOWS

FilePath FilePath::FromChar(std::string_view path)
{
    FilePath p;
    p._path.resize(path.size());
    for (uiw index = 0; index < path.size(); ++index)
    {
        p._path[index] = path[index];
    }
    return p;
}

FilePath FilePath::FromChar(const char *path)
{
    return FromChar(std::string_view{path});
}

FilePath FilePath::FromChar(char ch)
{
	return FromChar(std::string_view(&ch, 1));
}

FilePath FilePath::FromChar(const std::string &path)
{
    return FromChar(std::string_view{path});
}

FilePath FilePath::FromChar(std::string &&path)
{
    return FromChar(std::string_view{path});
}

FilePath &FilePath::Normalize()
{
    bool isPrevDelim = false;
    uiw target = 0;
    for (uiw source = 0; _path[source]; ++source)
    {
        PathChar ch = _path[source];
        if (IsPathDelim(ch))
        {
            if (!isPrevDelim)
            {
                _path[target++] = L'/';
                isPrevDelim = true;
            }
        }
        else
        {
            isPrevDelim = false;
            _path[target++] = ch;
        }
    }

    _path.resize(target);
    if (_path.size() && _path.back() == L'/')
    {
        _path.pop_back();
    }

    return *this;
}

FilePath FilePath::GetNormalized() const
{
    auto normalized = *this;
    normalized.Normalize();
    return normalized;
}

bool FilePath::IsValid() const
{
    if (_path.length() >= maxPathLength)
    {
        return false;
    }

    // TODO: actual validation of the path
    // TODO: file names length check
    return true;
}

FilePath &FilePath::MakeAbsolute()
{
    wchar_t tempBuf[maxPathLength];
    DWORD result = ::GetFullPathNameW(_path.c_str(), maxPathLength, tempBuf, 0);
    if (result)
    {
        ASSUME(tempBuf[result - 1] == _path.back()); // I don't think GetFullPathName can remove/change the last (back)slash?
        _path = tempBuf;
    }
	return *this;
}

FilePath FilePath::GetAbsolute() const
{
    wchar_t tempBuf[maxPathLength];
    DWORD result = ::GetFullPathNameW(_path.c_str(), maxPathLength, tempBuf, 0);
    if (result)
    {
        ASSUME(tempBuf[result - 1] == _path.back()); // I don't think GetFullPathName can remove/change the last (back)slash?
        return tempBuf;
    }
    return {};
}

bool FilePath::IsAbsolute() const
{
    NOIMPL;
    return false;
}

#else

FilePath FilePath::FromChar(std::string_view path)
{
    return FilePath(path);
}

FilePath FilePath::FromChar(const char *path)
{
    return FilePath(path);
}

FilePath FilePath::FromChar(const std::string &path)
{
    return FilePath(path);
}

FilePath FilePath::FromChar(std::string &&path)
{
    return FilePath(std::move(path));
}

FilePath &FilePath::Normalize()
{
    bool isPrevDelim = false;
    uiw target = 0;
    for (uiw source = 0; _path[source]; ++source)
    {
        PathChar ch = _path[source];
        if (IsPathDelim(ch))
        {
            if (!isPrevDelim)
            {
                _path[target++] = '/';
                isPrevDelim = true;
            }
        }
        else
        {
            isPrevDelim = false;
            _path[target++] = ch;
        }
    }

    _path.resize(target);
    if (_path.size() && _path.back() == '/')
    {
        _path.pop_back();
    }

    return *this;
}

bool FilePath::IsValid() const
{
    if (_path.length() >= maxPathLength)
    {
        return false;
    }

    for (PathChar ch : _path)
    {
        if (IsPathDelim(ch))
        {
            if (!IsValidPathDelim(ch))
            {
                return false;
            }
        }
    }

    // TODO: actual validation of the math
    // TODO: file names length check
    return true;
}

FilePath &FilePath::MakeAbsolute()
{
    NOIMPL;
	return *this;
}

FilePath FilePath::GetAbsolute() const
{
    NOIMPL;
    return {};
}

bool FilePath::IsAbsolute() const
{
    NOIMPL;
    return false;
}

#endif

FilePath &FilePath::AddLevel()
{
    if (!_path.empty() && !IsPathDelim(_path.back()))
    {
        _path += '/';
    }
    return *this;
}

FilePath &FilePath::RemoveLevel()
{
    if (!_path.empty())
    {
        do
        {
            _path.pop_back();
        } while (!_path.empty() && !IsPathDelim(_path.back()));
    }
    return *this;
}

bool FilePath::IsEmpty() const
{
    return _path.empty();
}

uiw FilePath::Length() const
{
    return _path.length();
}

bool FilePath::HasFileName() const
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return false;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            return true;
        }
    }

    return false;
}

std::optional<FilePath> FilePath::FileName(std::optional<pathStringView> defaultName) const
{
    auto view = FileNameView();
    if (view) return {*view};
    if (defaultName) return {*defaultName};
    return std::nullopt;
}

std::optional<pathStringView> FilePath::FileNameView(std::optional<pathStringView> defaultName) const
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return defaultName;
    }

    auto dotIt = _path.rbegin();
    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (*it == '.')
        {
            dotIt = it + 1;
        }
        else if (IsPathDelim(*it))
        {
            return pathStringView(&*(it - 1), it - dotIt);
        }
    }

    return defaultName;
}

void FilePath::ReplaceFileName(pathStringView newName)
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return;
    }

    auto dotIt = _path.end();
    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (*it == '.')
        {
            dotIt = it.base() - 1;
        }
        else if (IsPathDelim(*it))
        {
            auto begin = it.base();
            _path.erase(begin, dotIt);
            _path.insert(begin - _path.begin(), newName);
            return;
        }
    }
}

bool FilePath::HasExtension() const
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return false;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            return false;
        }

        if (*it == '.')
        {
            return true;
        }
    }

    return false;
}

std::optional<FilePath> FilePath::Extension(std::optional<pathStringView> defaultName) const
{
    auto view = ExtensionView();
    if (view) return {*view};
    if (defaultName) return {*defaultName};
    return std::nullopt;
}

std::optional<pathStringView> FilePath::ExtensionView(std::optional<pathStringView> defaultName) const
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return defaultName;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            return defaultName;
        }

        if (*it == '.')
        {
            return pathStringView(&*(it - 1), it - _path.rbegin());
        }
    }

    return defaultName;
}

void FilePath::ReplaceExtension(pathStringView newExt)
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            return;
        }

        if (*it == '.')
        {
            auto begin = it.base();
            _path.erase(begin, _path.end());
            _path.insert(begin - _path.begin(), newExt);
            return;
        }
    }
}

bool FilePath::HasFileNameExt() const
{
    return HasFileName();
}

std::optional<FilePath> FilePath::FileNameExt(std::optional<pathStringView> defaultName) const
{
    auto view = FileNameExtView();
    if (view) return {*view};
    if (defaultName) return {*defaultName};
    return std::nullopt;
}

std::optional<pathStringView> FilePath::FileNameExtView(std::optional<pathStringView> defaultName) const
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return defaultName;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            return pathStringView(&*(it - 1), it - _path.rbegin());
        }
    }

    return defaultName;
}

void FilePath::ReplaceFileNameExt(pathStringView newName)
{
    if (!IsValid() || IsPathDelim(_path.back()))
    {
        return;
    }

    for (auto it = _path.rbegin(); it != _path.rend(); ++it)
    {
        if (IsPathDelim(*it))
        {
            auto begin = it.base();
            _path.erase(begin, _path.end());
            _path.insert(begin - _path.begin(), newName);
            return;
        }
    }
}

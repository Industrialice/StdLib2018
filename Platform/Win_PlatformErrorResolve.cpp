#include "_PreHeader.hpp"
#include "PlatformErrorResolve.hpp"

NOINLINE auto StdLib::PlatformErrorResolve(const char *message) -> Error<>
{
	DWORD code = GetLastError();

	switch (code)
	{
		case ERROR_FILE_INVALID:
			return DefaultError::InvalidArgument(message);
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			return DefaultError::NotFound(message);
		case ERROR_ACCESS_DENIED:
		case ERROR_WRITE_PROTECT:
		case ERROR_SHARING_VIOLATION:
		case ERROR_LOCK_VIOLATION:
		case ERROR_USER_MAPPED_FILE:
			return DefaultError::AccessDenied(message);
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_OUTOFMEMORY:
			return DefaultError::OutOfMemory(message);
		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			return DefaultError::AlreadyExists(message);
		default:
			return DefaultError::UnknownError(message);
	}
}
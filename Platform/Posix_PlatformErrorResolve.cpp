#include "_PreHeader.hpp"
#include "PlatformErrorResolve.hpp"

NOINLINE auto StdLib::PlatformErrorResolve(const char *message) -> Error<>
{
	int code = errno;

	switch (code)
	{
		case EPERM:
			return DefaultError::Unsupported(message);
		case ENOENT:
			return DefaultError::NotFound(message);
		case ENAMETOOLONG:
		case ENOTDIR:
		case EINVAL:
		case EBADF:
		case ENODEV:
			return DefaultError::InvalidArgument(message);
		case EACCES:
		case EFAULT:
		case EROFS:
		case ETXTBSY:
		case SIGSEGV:
		case SIGBUS:
			return DefaultError::AccessDenied(message);
		case EDQUOT:
		case ELOOP:
		case EMLINK:
		case ENOMEM:
		case ENOSPC:
		case EOVERFLOW:
		case EMFILE:
		case ENFILE:
			return DefaultError::OutOfMemory(message);
		case EEXIST:
			return DefaultError::AlreadyExists(message);
		case EINTR:
			return DefaultError::Interrupted(message);
		default:
			return DefaultError::UnknownError(message);
	}
}
#include "file_utils.hpp"

#include <Windows.h>

#include "hlsdk/public/FileSystem.h"

#include "core/global.hpp"

#define NAMESPACE_BEGIN namespace U::File {
#define NAMESPACE_END }

NAMESPACE_BEGIN

bool FileExists(const char *path)
{
	if (!path || !*path)
		return false;

	return P::filesystem->FileExists(path);
}

NAMESPACE_END
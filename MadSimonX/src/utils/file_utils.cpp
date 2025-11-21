#include "file_utils.hpp"

#include <Windows.h>

#define FILE_NAMESPACE_BEGIN namespace U::File {
#define FILE_NAMESPACE_END }

FILE_NAMESPACE_BEGIN

bool FileExists(const char *path)
{
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

FILE_NAMESPACE_END
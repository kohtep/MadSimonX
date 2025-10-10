#include "app_utils.hpp"

#include <Windows.h>

bool U::App::HasCommandLineParam(const char *param)
{
    const char *cmd = GetCommandLineA();
    if (!cmd || !param)
        return false;

    return strstr(cmd, param) != NULL;
}

bool U::App::HasDebugParam()
{
    return HasCommandLineParam(" -debug");
}
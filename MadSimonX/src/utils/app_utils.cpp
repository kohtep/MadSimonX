#include "app_utils.hpp"

#include <Windows.h>

#include "core/global.hpp"

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

bool U::App::AppendWindowTag(const char *tag)
{
    if (!tag)
        return false;

    HWND hWnd = P::game->GetMainWindow();

    char buffer[512];
    int length = GetWindowTextA(hWnd, buffer, sizeof(buffer));
    if (length <= 0)
        return false;

    const char separator[] = " // ";
    const int sepLen = 4;
    int tagLen = lstrlenA(tag);

    if (length + sepLen + tagLen >= static_cast<int>(sizeof(buffer)))
        return false;

    int pos = length;
    for (int i = 0; i < sepLen; ++i)
        buffer[pos++] = separator[i];

    for (int i = 0; i < tagLen; ++i)
        buffer[pos++] = tag[i];

    buffer[pos] = '\0';

    if (!SetWindowTextA(hWnd, buffer))
        return false;

    return true;
}
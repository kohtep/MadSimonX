@echo off
setlocal

set OUT=%1

set MAJOR=1
set MINOR=0
set PATCH=0

for /f %%i in ('
  powershell -NoProfile -Command ^
    "(Get-Date).Date - (Get-Date '2018-06-29').Date | Select -Expand Days"
') do set BUILD=%%i

> "%OUT%" echo #pragma once
>>"%OUT%" echo #define FILEVER        %MAJOR%,%MINOR%,%PATCH%,%BUILD%
>>"%OUT%" echo #define PRODUCTVER     %MAJOR%,%MINOR%,%PATCH%,%BUILD%
>>"%OUT%" echo #define FILEVER_STR    "%MAJOR%.%MINOR%.%PATCH%.%BUILD%\0"
>>"%OUT%" echo #define PRODUCTVER_STR "%MAJOR%.%MINOR%.%PATCH%.%BUILD%\0"

endlocal
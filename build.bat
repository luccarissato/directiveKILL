@echo off
REM Build script that uses RAYLIB environment variable.
REM It tries common w64devkit locations relative to RAYLIB or a W64DEVKIT env var.

setlocal ENABLEDELAYEDEXPANSION

if "%RAYLIB%"=="" (
    echo ERROR: RAYLIB environment variable is not set.
    echo Please set a user/system environment variable named RAYLIB that points to your raylib root (example: C:\\raylib).
    echo You can run the "setup" task to create a .env.example in the workspace.
    exit /b 1
)

set "RAY=%RAYLIB%"

REM Potential gcc paths to try (no absolute hard-coded paths other than env fallbacks)
set "TRY1=%RAY%\w64devkit\mingw64\bin\gcc.exe"
set "TRY2=%RAY%\w64devkit\bin\gcc.exe"
set "TRY3=%W64DEVKIT%\mingw64\bin\gcc.exe"
set "TRY4=%W64DEVKIT%\bin\gcc.exe"

set "GCC="
if exist "%TRY1%" set "GCC=%TRY1%"
if exist "%TRY2%" set "GCC=%TRY2%"
if "%GCC%"=="" if exist "%TRY3%" set "GCC=%TRY3%"
if "%GCC%"=="" if exist "%TRY4%" set "GCC=%TRY4%"

if "%GCC%"=="" (
    echo ERROR: Could not find gcc. Expected it under RAYLIB\w64devkit or in W64DEVKIT env var.
    echo Checked paths:
    echo  %TRY1%
    echo  %TRY2%
    echo  %TRY3%
    echo  %TRY4%
    echo If you have w64devkit installed in a different location, set W64DEVKIT env var or modify this script.
    exit /b 1
)

echo Using compiler: %GCC%

REM Build command (include raylib headers and link with raylib). Output placed in workspace root.
REM If a w64devkit root exists, prefer it and ensure cc1/cc1plus are locatable by adjusting PATH.
set "W64ROOT="
if exist "%RAY%\w64devkit" set "W64ROOT=%RAY%\w64devkit"
if "%W64DEVKIT%" NEQ "" if exist "%W64DEVKIT%" set "W64ROOT=%W64DEVKIT%"
if exist "C:\\w64devkit" if "%W64ROOT%"=="" set "W64ROOT=C:\\w64devkit"

if not "%W64ROOT%"=="" (
    echo Found w64devkit at %W64ROOT%, preparing environment...
    REM prepare to search for cc1.exe under W64ROOT
    set "CC1DIR="
)

if not "%W64ROOT%"=="" for /r "%W64ROOT%" %%G in (cc1.exe) do (
    set "CC1DIR=%%~dpG"
    goto FOUND_CC1
)

:FOUND_CC1
if defined CC1DIR (
    echo Prepending CC1 dir to PATH: %CC1DIR%
    set "PATH=%CC1DIR%;%W64ROOT%\bin;%PATH%"
    set "FOUND_W64=1"
) else if not "%W64ROOT%"=="" (
    echo cc1 not found under %W64ROOT%, adding %W64ROOT%\bin to PATH anyway
    set "PATH=%W64ROOT%\bin;%PATH%"
    set "FOUND_W64=1"
)

if defined FOUND_W64 (
    if exist "%W64ROOT%\bin\x86_64-w64-mingw32-gcc.exe" (
        set "COMPILER=%W64ROOT%\bin\x86_64-w64-mingw32-gcc.exe"
    ) else if exist "%W64ROOT%\bin\gcc.exe" (
        set "COMPILER=%W64ROOT%\bin\gcc.exe"
    ) else (
        set "COMPILER=%GCC%"
    )
    echo Using compiler: %COMPILER%
    "%COMPILER%" "%~dp0main.c" -o "%~dp0main.exe" -I"%RAY%\raylib\src" -L"%RAY%\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
)

if not defined FOUND_W64 (
    "%GCC%" "%~dp0main.c" -o "%~dp0main.exe" -I"%RAY%\raylib\src" -L"%RAY%\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
)

if errorlevel 1 (
    echo Build FAILED
    exit /b 1
)

echo Build succeeded: %~dp0main.exe
endlocal
exit /b 0

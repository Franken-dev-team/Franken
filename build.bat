@echo off
setlocal enabledelayedexpansion

:: --- FRANKEN ENGINE BUILD SCRIPT ---
echo ========================================
echo       BUILDING FRANKEN ENGINE
echo ========================================

:: Check if build directory exists, if not create it
if not exist build (
    echo [*] Creating build directory...
    mkdir build
)

:: Run CMake Configuration
echo [*] Running CMake configuration...
cmake -B build -S .
if %errorlevel% neq 0 (
    echo [!] CMake configuration failed. This often happens due to stale cache.
    echo [*] Attempting to clear build directory and retry...
    rd /s /q build
    mkdir build
    cmake -B build -S .
    if !errorlevel! neq 0 (
        echo [!] Second attempt failed. Please check if Visual Studio is installed correctly.
        pause
        exit /b !errorlevel!
    )
)

:: Build the project
echo [*] Building project (Release mode)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo [!] Build failed!
    pause
    exit /b %errorlevel%
)

:: Deploy DLLs and Assets
echo [*] Copying dependencies and assets...
if exist "build\Release" (
    copy /Y "lib\SDL3\x64\SDL3.dll" "build\Release\"
    copy /Y "lib\ttf\x64\SDL3_ttf.dll" "build\Release\"
    if exist "assets" (
        xcopy /E /I /Y "assets" "build\Release\assets"
    )
    echo [OK] DLLs and Assets copied to build\Release\
) else if exist "build" (
    :: In case of single-config generators like MinGW
    copy /Y "lib\SDL3\x64\SDL3.dll" "build\"
    copy /Y "lib\ttf\x64\SDL3_ttf.dll" "build\"
    if exist "assets" (
        xcopy /E /I /Y "assets" "build\assets"
    )
    echo [OK] DLLs and Assets copied to build\
)

echo ========================================
echo BUILD COMPLETE!
echo ========================================
echo You can run the engine from build\Release\CluxeEditor.exe
pause

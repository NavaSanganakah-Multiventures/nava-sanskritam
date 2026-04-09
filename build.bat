@echo off
echo 🚩 Nava Sanskritam: Building Native NVC Compiler (SUL v13.0) 🚩
if not exist nvc\build mkdir nvc\build
cd nvc\build
cmake ..
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ❌ Build Failed! Make sure CMake and Visual Studio are in your PATH.
    pause
    exit /b %errorlevel%
)
echo ✅ Build Successful! nvc.exe is ready in nvc\build\Release\
pause

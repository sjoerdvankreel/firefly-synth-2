@echo off
setlocal

if "%1" == "" goto usage
if "%2" == "" goto usage
if "%3" == "" goto usage

cd ..
if not exist build\windows mkdir build\windows
cd build\windows
cmake -D FB_USE_SSE="%2" -D FB_USE_AVX="%3" ..\..
if %errorlevel% neq 0 exit /b !errorlevel!

msbuild /property:Configuration="%1" playground_plug.sln
if %errorlevel% neq 0 exit /b !errorlevel!

exit /b 0

:usage
echo "Usage: Debug|Release FB_USE_SSE=0|1 FB_USE_AVX=0|1. Example: build_windows Debug 1 0 builds debug binaries with SSE support."
exit /b 1
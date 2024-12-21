@echo off
setlocal

if "%1" == "" goto usage

cd ..
if not exist build\windows\sse mkdir build\windows\sse
cd build\windows\sse
cmake -D FB_USE_SSE=1 -D FB_USE_AVX=0 ..\..\..
if %errorlevel% neq 0 exit /b !errorlevel!

msbuild /property:Configuration="%1" playground_plug.sln
if %errorlevel% neq 0 exit /b !errorlevel!

exit /b 0

:usage
echo "Usage: build_windows Debug|Release"
exit /b 1
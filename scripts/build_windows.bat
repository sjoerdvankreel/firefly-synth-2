@echo off
setlocal

if "%1" == "" goto usage

cd ..
if not exist build\windows mkdir build\windows
cd build\windows
cmake -DFB_ARCH_TYPE=1 ..\..
if %errorlevel% neq 0 exit /b !errorlevel!
msbuild /property:Configuration="%1" firefly_synth.sln
if %errorlevel% neq 0 exit /b !errorlevel!

exit /b 0
:usage
echo "Usage: build_windows Debug|Release"
exit /b 1
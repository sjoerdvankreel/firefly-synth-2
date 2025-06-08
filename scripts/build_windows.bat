@echo off
setlocal

if "%1" == "" goto usage

cd ..
if not exist build\windows mkdir build\windows
cd build\windows
cmake ..\..
if %errorlevel% neq 0 exit /b !errorlevel!
msbuild /property:Configuration="%1" firefly_synth_2.sln
if %errorlevel% neq 0 exit /b !errorlevel!

cd ..\..\dist\windows\"%1"
7z a -tzip FireflySynth2-windows-vst3-"%1".zip FireflySynth2.vst3\*
7z a -tzip FireflySynth2-windows-clap-"%1".zip FireflySynth2.clap\*
if %errorlevel% neq 0 exit /b !errorlevel!

cd ..\..\..\..\scripts

exit /b 0
:usage
echo "Usage: build_windows Debug|RelWithDebInfo"
exit /b 1
@echo off
setlocal

cd ..
if not exist build\windows mkdir build\windows
cd build\windows
cmake ..\..
if %errorlevel% neq 0 exit /b !errorlevel!

msbuild /property:Configuration="%1" playground_plug.sln
if %errorlevel% neq 0 exit /b !errorlevel!
@ECHO OFF
setlocal
set startDir=%CD%
cd %~dp0

if "%1" == "" (
    set buildConfig=Release
) else (
    set buildConfig=%1
)

if "%2" == "" (
    set buildPlatform=win-x64
) else (
    set buildPlatform=%2
)

if "%buildPlatform%" == "win-x64" (
    set cmake-arch="x64"
) else (
    set cmake-arch="Win32"
)

call :run_exit_on_error dotnet build managed.sln -c %buildConfig%
call :run_exit_on_error cd unitygc
call :run_exit_on_error cmake . -A %cmake-arch%
call :run_exit_on_error cmake --build . --config %buildConfig%
call :run_exit_on_error cd..
call :run_exit_on_error copy unitygc\%buildConfig%\unitygc.dll ..\artifacts\bin\microsoft.netcore.app.runtime.%buildPlatform%\%buildConfig%\runtimes\%plateform%\native
call :run_exit_on_error copy ..\artifacts\bin\unity-embed-host\%buildConfig%\net6.0\unity-embed-host.dll ..\artifacts\bin\microsoft.netcore.app.runtime.%buildPlatform%\%buildConfig%\runtimes\%buildPlatform%\lib\net7.0 
call :run_exit_on_error copy ..\artifacts\bin\unity-embed-host\%buildConfig%\net6.0\unity-embed-host.pdb ..\artifacts\bin\microsoft.netcore.app.runtime.%buildPlatform%\%buildConfig%\runtimes\%buildPlatform%\lib\net7.0 
call :run_exit_on_error copy ..\LICENSE.md ..\artifacts\bin\microsoft.netcore.app.runtime.%buildPlatform%\%buildConfig%\runtimes\%buildPlatform%

set save_error_level=%errorlevel%
cd %startDir%
set errorlevel=%save_error_level%
exit /b %save_error_level%
GOTO :EOF

:run_exit_on_error
if "%errorlevel%" == "0" call %*
GOTO :EOF



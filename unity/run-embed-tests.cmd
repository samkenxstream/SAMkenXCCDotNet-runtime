@ECHO OFF
setlocal
set startDir=%CD%

if "%1" == "" (
    SET buildConfig=Release
) else (
    SET buildConfig=%1
)

call :run_exit_on_error dotnet build managed.sln -c %buildConfig%
call :run_exit_on_error cd embed_api_tests
call :run_exit_on_error cmake .
call :run_exit_on_error cmake --build . --config %buildConfig%
call :run_exit_on_error .\%buildConfig%\mono_test_app.exe

set save_error_level=%errorlevel%
cd %startDir%
set errorlevel=%save_error_level%
exit /b %save_error_level%
GOTO :EOF

:run_exit_on_error
if "%errorlevel%" == "0" %*
GOTO :EOF



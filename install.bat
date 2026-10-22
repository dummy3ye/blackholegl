@echo off
rem Builds blackholegl on Windows with CMake.
rem
rem   install.bat          build the project
rem   install.bat -Tools   install missing tools with winget first, then build
rem
rem Needs CMake >= 3.21 and the Visual Studio 2022 Build Tools with the C++
rem workload. GLFW is downloaded and built by CMake, no admin rights needed.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build-windows.ps1" %*
set "EXITCODE=%ERRORLEVEL%"
if not defined BH_NO_PAUSE pause
exit /b %EXITCODE%

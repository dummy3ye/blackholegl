@echo off
setlocal EnableExtensions
rem Builds blackholegl on Windows: installs Git, CMake and the VS 2022 Build
rem Tools via winget, builds GLFW 3.4 from source, then builds the project.
rem The plain Makefile is Linux-only (-ldl -lm), hence this CMake path.
rem Requires an Administrator terminal.

set "ROOT=%~dp0"
set "WORK=%ROOT%build-win"
set "DEPS=%WORK%\deps"
set "GLFWSRC=%WORK%\glfw-src"
set "GLFW_TAG=3.4"

net session >nul 2>&1
if not errorlevel 1 goto :admin_ok
fltmc >nul 2>&1
if not errorlevel 1 goto :admin_ok
echo error: administrator rights required, rerun from an elevated terminal
goto :fail
:admin_ok

where winget >nul 2>&1
if not errorlevel 1 goto :winget_ok
echo error: winget not found, install "App Installer" from the Microsoft Store
goto :fail
:winget_ok

winget install --id Git.Git -e --silent --accept-package-agreements --accept-source-agreements
if errorlevel 1 echo warning: git install reported a problem, verified below

winget install --id Kitware.CMake -e --silent --accept-package-agreements --accept-source-agreements
if errorlevel 1 echo warning: cmake install reported a problem, verified below

echo installing VS 2022 Build Tools, large download
winget install --id Microsoft.VisualStudio.2022.BuildTools -e --silent --accept-package-agreements --accept-source-agreements --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
if errorlevel 1 echo warning: build tools install reported a problem, verified below

rem newly installed programs are not always on PATH yet
set "CMAKE="
set "GIT="
set "VSPATH="

if exist "%ProgramFiles%\CMake\bin\cmake.exe" set "CMAKE=%ProgramFiles%\CMake\bin\cmake.exe"
if not defined CMAKE if exist "%LocalAppData%\Programs\CMake\bin\cmake.exe" set "CMAKE=%LocalAppData%\Programs\CMake\bin\cmake.exe"
if not defined CMAKE where cmake >nul 2>&1 && set "CMAKE=cmake"

if exist "%ProgramFiles%\Git\cmd\git.exe" set "GIT=%ProgramFiles%\Git\cmd\git.exe"
if not defined GIT if exist "%LocalAppData%\Programs\Git\cmd\git.exe" set "GIT=%LocalAppData%\Programs\Git\cmd\git.exe"
if not defined GIT where git >nul 2>&1 && set "GIT=git"

if not defined CMAKE (
    echo error: cmake not found after install, open a fresh terminal and rerun
    goto :fail
)
if not defined GIT (
    echo error: git not found after install, open a fresh terminal and rerun
    goto :fail
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo error: VS Build Tools not found, open a fresh terminal and rerun
    goto :fail
)
for /f "usebackq tokens=*" %%i in (`""%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath"`) do set "VSPATH=%%i"
if not defined VSPATH (
    echo error: no Visual Studio C++ compiler found, open a fresh terminal and rerun
    goto :fail
)

if exist "%GLFWSRC%\CMakeLists.txt" goto :glfw_have
"%GIT%" clone --depth 1 --branch %GLFW_TAG% https://github.com/glfw/glfw.git "%GLFWSRC%"
if errorlevel 1 goto :fail
:glfw_have

"%CMAKE%" -S "%GLFWSRC%" -B "%GLFWSRC%\build" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail
"%CMAKE%" --build "%GLFWSRC%\build" --config Release --parallel
if errorlevel 1 goto :fail
"%CMAKE%" --install "%GLFWSRC%\build" --prefix "%DEPS%"
if errorlevel 1 goto :fail

"%CMAKE%" -S "%ROOT%." -B "%WORK%" -DCMAKE_PREFIX_PATH="%DEPS%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail
"%CMAKE%" --build "%WORK%" --config Release --parallel
if errorlevel 1 goto :fail

rem shaders are loaded from .\shaders relative to the working directory
xcopy /E /I /Y "%ROOT%shaders" "%WORK%\Release\shaders" >nul
if errorlevel 1 goto :fail

echo.
echo build ok: %WORK%\Release\blackholegl.exe
echo needs an OpenGL 4.3 Core capable GPU driver
echo run from the repo root:
echo   cd /d "%ROOT%."
echo   "build-win\Release\blackholegl.exe"
goto :end

:fail
echo.
echo build failed, see messages above
pause
exit /b 1

:end
pause
exit /b 0

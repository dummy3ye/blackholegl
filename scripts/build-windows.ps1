# Builds blackholegl on Windows with CMake.
#
# Requirements:
#   - CMake >= 3.21
#   - Visual Studio 2022 Build Tools with the C++ workload
#
# GLFW is downloaded and built automatically by CMake, no admin rights needed.
#
# Parameters:
#   -Tools                    install missing tools with winget first
#   -Preset <name>            CMake preset to build (default windows-msvc-release)
#   -Configuration <name>     build configuration (default Release)
#   -Install                  install into -Prefix after building
#   -Prefix <path>            install prefix

param(
    [switch]$Tools,
    [string]$Preset = 'windows-msvc-release',
    [string]$Configuration = 'Release',
    [switch]$Install,
    [string]$Prefix = ''
)

$ErrorActionPreference = 'Continue'

$root = Split-Path -Parent $PSScriptRoot

function Find-CMake {
    $candidates = @(
        "$env:ProgramFiles\CMake\bin\cmake.exe",
        "$env:LOCALAPPDATA\Programs\CMake\bin\cmake.exe"
    )
    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) { return $candidate }
    }
    $cmd = Get-Command cmake -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    return $null
}

function Find-VisualStudio {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { return $null }
    $path = & $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath
    if (-not $path) { return $null }
    return ($path | Select-Object -First 1).Trim()
}

$cmake = Find-CMake
$vs = Find-VisualStudio

if ($Tools) {
    if (-not $cmake) {
        Write-Host "installing CMake"
        winget install --id Kitware.CMake -e --silent --accept-package-agreements --accept-source-agreements
        $cmake = Find-CMake
    }
    if (-not $vs) {
        Write-Host "installing Visual Studio 2022 Build Tools, this is a large download"
        winget install --id Microsoft.VisualStudio.2022.BuildTools -e --silent --accept-package-agreements --accept-source-agreements --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
        $vs = Find-VisualStudio
    }
}

if (-not $cmake) {
    Write-Host ""
    Write-Host "error: cmake not found"
    Write-Host "  install it with: winget install Kitware.CMake"
    Write-Host "  then reopen the terminal and run install.bat -Tools"
    exit 1
}

if (-not $vs) {
    Write-Host ""
    Write-Host "error: no Visual Studio C++ compiler found"
    Write-Host "  install it with: winget install Microsoft.VisualStudio.2022.BuildTools"
    Write-Host "  then reopen the terminal and run install.bat -Tools"
    exit 1
}

$buildDir = Join-Path $root "build\$Preset"

Push-Location $root
& $cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { Pop-Location; exit 1 }
& $cmake --build --preset $Preset --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) { Pop-Location; exit 1 }
Pop-Location

if ($Install) {
    if (-not $Prefix) { $Prefix = Join-Path $root "dist" }
    Push-Location $root
    & $cmake --install $buildDir --config $Configuration --prefix $Prefix
    if ($LASTEXITCODE -ne 0) { Pop-Location; exit 1 }
    Pop-Location
    Write-Host ""
    Write-Host "installed to $Prefix"
    Write-Host "  $Prefix\bin\blackholegl.exe"
}

$exe = Join-Path $buildDir "$Configuration\blackholegl.exe"
Write-Host ""
Write-Host "build ok: $exe"
Write-Host "needs an OpenGL 4.3 Core capable GPU driver"
Write-Host "run it with:"
Write-Host "  $exe"
exit 0

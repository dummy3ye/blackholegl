# blackholegl

![GitHub last commit](https://img.shields.io/github/last-commit/dummy3ye/blackholegl?style=for-the-badge&labelColor=101418&color=9ccbfb)
![GitHub Repo stars](https://img.shields.io/github/stars/dummy3ye/blackholegl?style=for-the-badge&labelColor=101418&color=b9c8da)
![GitHub repo size](https://img.shields.io/github/repo-size/dummy3ye/blackholegl?style=for-the-badge&labelColor=101418&color=d3bfe6)

Blackholegl is a simulation of The Gargantua Blackhole seen in the movie intersteller written entirely in C++ and OpenGL

> [!IMPORTANT]
> Thisn project's main code is written by human, Not LLM's. only help taken from llm is in the release pipeline because it's too problematic to maintain multi-platform releases for one person.

https://github.com/user-attachments/assets/dc154f8b-f5fd-40ea-9eb0-5cc6d82ed8b2

A huge inspiration for this project was Kavan's video about simulating a blackhole in C++, check out his video on [youtube](https://www.youtube.com/watch?v=8-B6ryuBkCM).

I'm still working on it, so yeah<br>

## installation

**Download a prebuilt binary** from the [releases page](https://github.com/dummy3ye/blackholegl/releases):

| platform | file | how to run |
|----------|------|------------|
| Windows x64 | `blackholegl-windows-x64.zip` | unzip it, then double click `blackholegl.exe` |
| Linux x86_64 | `blackholegl-linux-x86_64.tar.gz` | `tar -xzf blackholegl-linux-x86_64.tar.gz && ./blackholegl` |

Both archives contain the binary and the `shaders/` folder it needs, no build
tools and no install step.

> [!WARNING]
> This project currently is still not able to run on MacOS.

> [!IMPORTANT]
> A GPU driver with **OpenGL 4.3 Core** is required (compute shaders).
> Check with `glxinfo \| grep "OpenGL version"` on Linux, or
> [glview](https://www.gfxbench.com/opengl) on Windows.

```sh
git clone https://github.com/dummy3ye/blackholegl
cd blackholegl

# Linux
cmake --preset linux && cmake --build --preset linux
./build/linux/blackholegl

# Windows
cmake --preset windows && cmake --build --preset windows
.\build\windows\Release\blackholegl.exe
```

GLFW is downloaded and built automatically when it is not installed on the
system, so there is no dependency to install by hand.

## controls

| input | action |
|-------|--------|
| mouse drag | orbit the camera |
| scroll | zoom (3.5M - 60.0M) |
| `0` - `5` | cycle render resolution (240p - 1440p) |
| `q` | cycle quality (very low / low / medium / high) |
| `f` | toggle the FPS overlay |
| `esc` | quit |

## command line

```
blackholegl [OPTIONS]

  --res=<int>, -r <int>      Render height (default 720, range 120-4320)
  --quality=<name|int>       very-low (0), low (1), medium (2), high (3)
  --show-fps                 Show real-time FPS / GPU overlay
  --very-low, -vl            Preset: quality=0, res=360p
  --help, -h                 Show this message and exit
  --version                  Show version and exit
```

## building from source

### requirements

- CMake >= 3.21 (for the presets) and a C++17 compiler
- OpenGL 4.3 Core capable driver

Nothing else, GLFW is fetched and built by CMake when it is missing. On Linux
the system packages are used when they are available:

```sh
# Debian / Ubuntu
sudo apt install cmake clang libglfw3-dev libgl1-mesa-dev

# Arch
sudo pacman -S cmake clang glfw
```

### presets

| preset | what it does |
|--------|--------------|
| `linux` / `linux-debug` | Linux build, debug adds address + UB sanitizers |
| `windows` / `windows-msvc-debug` | Windows build with MSVC |

```sh
cmake --preset linux
cmake --build --preset linux
```

### make

`make` is a thin wrapper around the CMake presets, it works on Linux and on
Windows (on Windows it goes through `scripts/build-windows.ps1`, so CMake does
not have to be on `PATH`):

| target | what it does |
|--------|--------------|
| `make` | build the release binary |
| `make run` | build and run |
| `make debug` | debug build, on Linux with address + UB sanitizers |
| `make install` | install binary + shaders under `$(PREFIX)` (`/usr/local` on Linux, `dist` on Windows) |
| `make clean` | remove build artefacts |

### windows

`install.bat` checks for CMake and the Visual Studio build tools, and builds
the project:

```bat
install.bat          :: build
install.bat -Tools   :: install missing tools with winget first, then build
```

Both tools can also be installed by hand:

```bat
winget install Kitware.CMake
winget install Microsoft.VisualStudio.2022.BuildTools --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

### windows on wsl2

Follow the Linux instructions, then note that a common problem in WSL2 is
getting the OpenGL version right. If anything looks off, run:

```sh
source scripts/wsl2-mesa-version-fix.sh
```

# Troubleshooting

<details>
<summary><b>1. Why am I getting the error "Window creation failed"?</b></summary>

Your GPU driver is likely out of date. The application requires **OpenGL 4.3 Core** support.
* **Fix:** Update your graphics drivers. On Intel or AMD Linux, ensure you are using **Mesa** drivers.
</details>

<details>
<summary><b>2. What does "Failed to open shader file" mean?</b></summary>

The application cannot locate the required shader files on launch.
* **Fix:** Ensure the `shaders/` directory is located directly next to the executable binary. Re-extract the project archive if necessary.
</details>

<details>
<summary><b>3. Why is the window opening to a black screen?</b></summary>

Your GPU may have dropped the compute shader, usually due to high resource demands or hardware limitations.
* **Fix:** Launch the application with reduced settings:
  `--quality=very-low` or a smaller resolution like `--res=360`
</details>

<details>
<summary><b>4. How can I improve performance on integrated graphics?</b></summary>

Integrated GPUs have limited rendering capacity and require lighter graphics options.
* **Fix:** Run the application with lower resolution and quality settings using the flags:
  `--res=360 --quality=very-low`
</details>

<details>
<summary><b>5. Why do I see "GLFW init failed" on startup?</b></summary>

The application cannot detect an active graphical display environment.
* **Fix:** Ensure a display server is available. If you are using **WSL2**, make sure **WSLg** is enabled to support GUI applications.
</details>

<details>
<summary><b>I can't have the solution to my problem above.</b></summary>

create a issue at [Issues](https://github.com/dummy3ye/blackholegl/issues)
</details>

## license

MIT, see [LICENSE](LICENSE).

# blackholegl

Blackholegl is a simulation of The Gargantua Blackhole seen in the movie intersteller

![v3 blackhole](assets/v3.png)

A huge inspiration for this project was Kavan's video about simulating a blackhole in C++, check out his video on [youtube](https://www.youtube.com/watch?v=8-B6ryuBkCM).

I'm still working on it, so yeah<br>

### wanna try it yourself ?

#### requirements
- clang / clang++
- make
- GLFW 3 (glfw3 dev package)
- OpenGL (libGL)
- cmake ≥ 3.15
- pkg-config

### installation

#### For Linux

Dependencies:

```sh
# Debian / Ubuntu
sudo apt install clang make libglfw3-dev libgl1-mesa-dev

# Arch
sudo pacman -S clang make glfw
```

Build and run:

```sh
git clone https://github.com/dummy3ye/blackholegl
cd blackholegl
make
./build/blackholegl
```

Other make targets:

| target                 | what it does                                     |
|------------------------|--------------------------------------------------|
| `make run`             | build and run                                    |
| `make run RES=360`     | run with 360p render resolution                  |
| `make run SHOW_FPS=1`  | run with the FPS overlay                         |
| `make debug`           | debug build with address + UB sanitizers         |
| `make install`         | install binary + shaders under `/usr/local`      |
| `make clean`           | remove build artefacts                           |

`make install` honors `PREFIX` (e.g. `make install PREFIX=~/.local`).
The GPU driver must support OpenGL 4.3 Core (compute shaders) — check with
`glxinfo | grep "OpenGL version"` (`mesa-utils` package). Mesa covers
Intel/AMD; Nvidia needs the proprietary driver.

The Makefile links `-lGL -ldl -lm`, so it targets Linux — on macOS the linker
flags need adjusting (GLFW via Homebrew, OpenGL framework instead of `-lGL`).

#### For Windows

>[!IMPORTANT]
> in wsl2 a common problem is getting the OpenGL version right, if you get into any problem run `source scripts/wsl2-mesa-version-fix.sh`

The Makefile is Linux-only, so on Windows use `install.bat` from an
**Administrator** terminal. It installs Git, CMake and the VS 2022 Build
Tools via winget, builds GLFW 3.4 from source, then builds with CMake:

```bat
git clone https://github.com/dummy3ye/blackholegl
cd blackholegl
install.bat
build-win\Release\blackholegl.exe
```

Run the exe from the repo root so it finds `shaders/`.
The GPU driver must support OpenGL 4.3 Core (compute shaders).

Building inside **WSL 2** also works (WSL 1 has no GPU access) — install the same
packages as Linux (`clang make cmake libglfw3-dev libgl1-mesa-dev pkg-config`)
and `make run`. Check the GL version WSLg exposes with
`glxinfo | grep "OpenGL version"` (`mesa-utils` package).

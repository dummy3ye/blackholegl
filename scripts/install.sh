#!/usr/bin/env bash
# Builds blackholegl on Linux with CMake.
#
# Usage:
#   ./scripts/install.sh [options]
#
# Options:
#   --preset <name>       CMake preset to build (default: linux)
#   --debug               Build with debug preset (linux-debug)
#   --install             Install after building
#   --prefix <path>       Install prefix (default: /usr/local)
#   -h, --help            Show this help message

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

PRESET="linux"
INSTALL=0
PREFIX="/usr/local"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --preset)
      PRESET="$2"
      shift 2
      ;;
    --debug)
      PRESET="linux-debug"
      shift
      ;;
    --install)
      INSTALL=1
      shift
      ;;
    --prefix)
      PREFIX="$2"
      shift 2
      ;;
    -h|--help)
      echo "Usage: $0 [options]"
      echo ""
      echo "Options:"
      echo "  --preset <name>       CMake preset to build (default: linux)"
      echo "  --debug               Build with debug preset (linux-debug)"
      echo "  --install             Install after building"
      echo "  --prefix <path>       Install prefix (default: /usr/local)"
      echo "  -h, --help            Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
  esac
done

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is not installed or not in PATH." >&2
  echo "Install it with your package manager (e.g., sudo apt install cmake or sudo pacman -S cmake)" >&2
  exit 1
fi

cd "$ROOT_DIR"

echo "Configuring preset '$PRESET'..."
cmake --preset "$PRESET"

echo "Building preset '$PRESET'..."
cmake --build --preset "$PRESET"

BUILD_DIR="$ROOT_DIR/build/$PRESET"
EXE="$BUILD_DIR/blackholegl"

if [ "$INSTALL" -eq 1 ]; then
  echo "Installing to $PREFIX..."
  cmake --install "$BUILD_DIR" --prefix "$PREFIX"
  echo ""
  echo "Installed to $PREFIX/bin/blackholegl"
fi

echo ""
echo "Build ok: $EXE"
echo "Needs an OpenGL 4.3 Core capable GPU driver"
echo "Run it with:"
echo "  $EXE"

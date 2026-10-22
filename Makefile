#   make                     build release binary
#   make debug               build with debug symbols + address/UB sanitizers
#   make run                 build and run
#   make run RES=360         run with custom resolution
#   make run SHOW_FPS=1      run with the FPS overlay enabled
#   make clean               remove build artefacts
#   make install             install binary + shaders under $(PREFIX)
#
# This is a thin wrapper around the CMake build, CMake is the single source of
# truth. See CMakePresets.json for the available presets.
#
# On Windows the work is done by scripts/build-windows.ps1, which finds CMake
# and the Visual Studio build tools even when they are not on PATH.

TARGET := blackholegl

ifeq ($(OS),Windows_NT)
  PRESET               ?= windows
  DEBUG_PRESET         ?= windows-msvc-debug
  CONFIGURATION        ?= Release
  DEBUG_CONFIGURATION  ?= Debug
  BUILD_DIR := build/$(PRESET)
  BIN       := $(BUILD_DIR)/$(CONFIGURATION)/$(TARGET).exe
  PREFIX    ?= dist
  PS := powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
else
  PRESET               ?= linux
  DEBUG_PRESET         ?= linux-debug
  CONFIGURATION        ?=
  DEBUG_CONFIGURATION  ?=
  BUILD_DIR := build/$(PRESET)
  BIN       := $(BUILD_DIR)/$(TARGET)
  PREFIX    ?= /usr/local
  PS :=
endif

RES     ?=
SHOW_FPS ?=
ARGS    :=
ifneq ($(strip $(RES)),)
  ARGS += --res=$(RES)
endif
ifneq ($(strip $(SHOW_FPS)),)
  ARGS += --show-fps
endif

.PHONY: all build debug run clean install uninstall help

all: build

build:
ifeq ($(OS),Windows_NT)
	$(PS) -Preset $(PRESET) -Configuration $(CONFIGURATION)
else
	cmake --preset $(PRESET)
	cmake --build --preset $(PRESET)
endif

debug:
ifeq ($(OS),Windows_NT)
	$(PS) -Preset $(DEBUG_PRESET) -Configuration $(DEBUG_CONFIGURATION)
else
	cmake --preset $(DEBUG_PRESET)
	cmake --build --preset $(DEBUG_PRESET)
endif

run: build
	@echo Running: $(BIN) $(ARGS)
	$(BIN) $(ARGS)

clean:
ifeq ($(OS),Windows_NT)
	if exist build rmdir /s /q build
	if exist dist rmdir /s /q dist
else
	rm -rf build
endif

install: build
ifeq ($(OS),Windows_NT)
	$(PS) -Preset $(PRESET) -Configuration $(CONFIGURATION) -Install -Prefix $(PREFIX)
else
	cmake --install $(BUILD_DIR) --prefix $(PREFIX)
	@echo "Installed to $(PREFIX)/bin and $(PREFIX)/share/blackholegl"
endif

uninstall:
ifeq ($(OS),Windows_NT)
	if exist $(PREFIX) rmdir /s /q $(PREFIX)
else
	rm -f $(PREFIX)/bin/$(TARGET)
	rm -rf $(PREFIX)/share/blackholegl
	@echo "Uninstalled from $(PREFIX)"
endif

help:
	@echo "targets: all build debug run clean install uninstall"
	@echo "vars:    PRESET=$(PRESET) RES=$(RES) SHOW_FPS=$(SHOW_FPS) PREFIX=$(PREFIX)"

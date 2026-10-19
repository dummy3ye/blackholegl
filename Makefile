#   make                     build release binary
#   make debug               build with debug symbols + address sanitizer
#   make run                 build and run
#   make run RES=360         run with custom resolution
#   make run SHOW_FPS=1      run with FPS overlay enabled
#   make clean               remove build artefacts

CXX      := clang++
CC       := clang
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
CFLAGS   := -std=c11 -Wall -Wextra -O2
INCLUDES := -Iinclude -Iexternal/glad/include
LDFLAGS  := -lglfw -lGL -ldl -lm

TARGET   := blackholegl
BUILDDIR := build

CXX_SRCS := src/main.cpp src/shader.cpp
C_SRCS   := external/glad/src/gl.c

CXX_OBJS := $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CXX_SRCS))
C_OBJS   := $(patsubst %.c,$(BUILDDIR)/%.o,$(C_SRCS))
OBJS     := $(CXX_OBJS) $(C_OBJS)

RES      ?=
SHOW_FPS ?=

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
SHAREDIR := $(PREFIX)/share/blackholegl

.PHONY: all debug run clean install uninstall

all: $(BUILDDIR)/$(TARGET)

debug: CXXFLAGS := -std=c++17 -Wall -Wextra -g -O0 -fsanitize=address,undefined
debug: CFLAGS   := -std=c11 -Wall -Wextra -g -O0 -fsanitize=address,undefined
debug: LDFLAGS  += -fsanitize=address,undefined
debug: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "  LINK  $@"

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	@echo "  CXX   $<"

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "  CC    $<"

run: $(BUILDDIR)/$(TARGET)
	@ARGS=""; \
	if [ -n "$(RES)" ];      then ARGS="$$ARGS --res=$(RES)"; fi; \
	if [ -n "$(SHOW_FPS)" ]; then ARGS="$$ARGS --show-fps";   fi; \
	echo "Running: ./$(BUILDDIR)/$(TARGET) $$ARGS"; \
	./$(BUILDDIR)/$(TARGET) $$ARGS

clean:
	rm -rf $(BUILDDIR)

install: $(BUILDDIR)/$(TARGET)
	install -d $(BINDIR)
	install -m 755 $(BUILDDIR)/$(TARGET) $(BINDIR)/$(TARGET)
	install -d $(SHAREDIR)
	cp -r shaders $(SHAREDIR)/
	@echo "Installed to $(BINDIR) and $(SHAREDIR)"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(SHAREDIR)
	@echo "Uninstalled from $(BINDIR) and $(SHAREDIR)"

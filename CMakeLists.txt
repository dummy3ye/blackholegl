cmake_minimum_required(VERSION 3.15)
project(blackholegl VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# version reported by `blackholegl --version`, overridable by release CI
set(BLACKHOLEGL_VERSION "${PROJECT_VERSION}" CACHE STRING
    "version string reported by the --version flag")

find_package(OpenGL REQUIRED)

# GLFW: use an installed copy when there is one, otherwise download and build
# it from source so a fresh clone only needs CMake and a C/C++ compiler.
# Both paths provide the `glfw` target, so nothing below has to care.
find_package(glfw3 QUIET)
if(NOT TARGET glfw)
  include(FetchContent)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
  # wayland needs extra dev packages; X11 works everywhere GLFW is packaged
  set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(glfw)
  message(STATUS "GLFW was not found locally, building it from source")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
)

add_executable(blackholegl 
    src/main.cpp
    src/shader.cpp
    external/glad/src/gl.c
)

target_link_libraries(blackholegl 
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

target_compile_definitions(blackholegl PRIVATE BLACKHOLEGL_VERSION="${BLACKHOLEGL_VERSION}")

# older libstdc++ keeps std::filesystem in a separate library
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_link_libraries(blackholegl stdc++fs)
endif()

# shaders are loaded at runtime, keep a copy next to the binary so the
# executable works from any working directory
add_custom_command(TARGET blackholegl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:blackholegl>/shaders")

include(GNUInstallDirs)
install(TARGETS blackholegl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY shaders DESTINATION ${CMAKE_INSTALL_DATADIR}/blackholegl)

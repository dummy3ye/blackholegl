cmake_minimum_required(VERSION 3.15)
project(blackholegl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

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

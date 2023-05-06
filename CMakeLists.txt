cmake_minimum_required(VERSION 3.14)

message("CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")

# Define project name and detail
project("Helicopter"
    VERSION 0.1.0
    LANGUAGES C CXX
    DESCRIPTION "My CG hw2"
)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Define the executable name of this project, and create a CMake binary target
add_executable(MainExe "Main.cpp"   "PublicValue.h")

# Assign source files to the executable.
target_link_libraries(MainExe PRIVATE GLUT::GLUT)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(MainExe PRIVATE OpenGL::OpenGL OpenGL::GLU X11)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(MainExe PRIVATE OpenGL::GL OpenGL::GLU)
endif()
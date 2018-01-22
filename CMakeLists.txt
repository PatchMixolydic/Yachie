cmake_minimum_required(VERSION 3.6)
project(yachie)

set(INCLUDE_DIR "" CACHE FILEPATH "Includes directory")
set(LIBS_DIR "" CACHE FILEPATH "Library directory")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if(WIN32)
    set(RESOURCE_FILE ${PROJECT_SOURCE_DIR}/res/yachie.rc)
else()
    set(RESOURCE_FILE)
endif(WIN32)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT yachie)

include_directories(${INCLUDE_DIR})
link_directories(${LIBS_DIR})

add_executable(yachie ${RESOURCE_FILE} src/main.cpp src/Chip8.cpp src/Chip8.h src/Display.cpp src/Display.h src/tinyfiledialogs.c src/tinyfiledialogs.h)

target_link_libraries (yachie
    sfml-graphics
    sfml-window
    sfml-system
    -static-libgcc
    -static-libstdc++
)

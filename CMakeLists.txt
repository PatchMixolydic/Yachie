cmake_minimum_required(VERSION 3.6)
project(yachie)

set(INCLUDE_DIR "C:/C++ Libs/include" CACHE FILEPATH "Includes directory")
set(LIBS_DIR "C:/C++ Libs/lib" CACHE FILEPATH "Library directory")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# if(WIN32)
#     set(GUI_TYPE WIN32)
# endif(WIN32)
# if(APPLE)
#     set(GUI_TYPE MACOSX_BUNDLE)
# endif(APPLE)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT yachie)

include_directories(${INCLUDE_DIR})
link_directories(${LIBS_DIR})

add_executable(yachie ${GUI_TYPE} src/main.cpp src/Chip8.cpp src/Chip8.h src/Display.cpp src/Display.h)

target_link_libraries (yachie
    sfml-graphics
    sfml-window
    sfml-system
)

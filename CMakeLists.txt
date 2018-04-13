cmake_minimum_required(VERSION 3.9)
project(zombie)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(ext/gl3w)
include_directories(ext/glfw)
include_directories(ext/sdl)
include_directories(ext/stb_image)
include_directories(ext/Box2D)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(SDL)
find_package(SDL_mixer)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(TinyXML2)
find_package(ZLIB)
find_package(GLFW REQUIRED glfw3)
find_library(CoreFoundation corefoundation)
message(INFO ${TINYXML2_LIBRARIES})
message(INFO ${TINYXML2_INCLUDE_DIR})


file(GLOB_RECURSE cmake-build-debug/data ${CMAKE_SOURCE_DIR}/data/*.wav ${CMAKE_SOURCE_DIR}/data/*.png ${CMAKE_SOURCE_DIR}/data/*.mesh ${CMAKE_SOURCE_DIR}/data/*.bmp)
file(GLOB_RECURSE cmake-build-debug/shaders ${CMAKE_SOURCE_DIR}/shaders/*.glsl)
file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.hpp ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.in ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE EXT ${CMAKE_SOURCE_DIR}/ext/*.h ${CMAKE_SOURCE_DIR}/ext/*.cpp ${CMAKE_SOURCE_DIR}/ext/*.c)

add_executable(zombie ${SRC} ${DATA} ${SHADERS} ${EXT} ${HEADERS} ${HPPHEADERS})


target_link_libraries(zombie
        pthread
        ${TINYXML2_LIBRARIES}
        ${COCOA_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${SDL_MAIN_LIBRARY}
        ${SDL_LIBRARY}
        ${ZLIB_LIBRARIES}
        SDLmain
        SDL_mixer
        dl
        "-framework OpenGL"
        "-framework CoreFoundation"
        "-framework Cocoa")

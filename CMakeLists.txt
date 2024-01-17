cmake_minimum_required(VERSION 3.20)

project(engine
  VERSION
      "0.1.0"
  DESCRIPTION
      "Sonic Engine")

add_compile_definitions(-DIMGUI_IMPL_OPENGL_ES2)
    
### CMake modules ###
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


### Glad library ####
add_library(glad ${CMAKE_CURRENT_LIST_DIR}/src/glad.c)
target_include_directories(glad PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>)


### Flecs library ###
file(GLOB FLECS_SRC ${CMAKE_CURRENT_LIST_DIR}/flecs-3.2.7/*.c)
add_library(flecs ${FLECS_SRC})
target_include_directories(flecs PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/flecs-3.2.7/>)




### dear imgui library ###
file(GLOB IMGUI_SRC
  ${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/imgui_impl_opengl3.cpp)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/>)



### GAME ###
# Glob source files
file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_executable(game ${SRC_FILES})

# Dependencies
find_package(TomlPlusPlus REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
# Ogg, Vorbis, Vorbisfile
find_package(OggVorbis REQUIRED)

# Game include directories
target_include_directories(game PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/flecs-3.2.7/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/>
  $<INSTALL_INTERFACE:include/>)

target_link_libraries(game
  glfw
  OpenGL::GL
  glad
  flecs
  imgui
  tomlplusplus
  openal
  ogg
  vorbis
  vorbisfile)

set_property(TARGET glad PROPERTY C_STANDARD 11)
set_property(TARGET flecs PROPERTY C_STANDARD 99)
set_property(TARGET imgui PROPERTY CXX_STANDARD 20)
set_property(TARGET game PROPERTY CXX_STANDARD 20)

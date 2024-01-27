cmake_minimum_required(VERSION 3.27)

project(engine
  VERSION
      "0.1.0"
  DESCRIPTION
      "Sonic Engine")

add_compile_definitions(-DIMGUI_IMPL_OPENGL_ES3)
    
### CMake modules ###
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


### Glad library ####
# CHANGE
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glfw/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/>)
target_link_libraries(imgui
  OpenGL::GLES3
  glad
  glfw)

### GLFW ###
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glfw)

### ENGINE ###
# Glob source files
file(GLOB ENGINE_SRC_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/collision/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/components/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/controls/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/core/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/io/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/player/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/render/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/resources/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/scenes/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/sound/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/sprite/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/engine/tiled/*.cpp)

add_library(engine ${ENGINE_SRC_FILES})

target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/include/>
  
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glfw/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/tomlplusplus-3.4.0/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/rapidxml-1.16/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/flecs-3.2.7/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/>
  $<INSTALL_INTERFACE:include/>)


### GAME ###
# Dependencies that should have their development packages installed system-wide
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS GLES2 GLES3)
find_package(OpenAL REQUIRED)
find_package(OggVorbis REQUIRED)

file(GLOB GAME_SRC_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/screens/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_executable(game ${GAME_SRC_FILES})

target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/>
  $<INSTALL_INTERFACE:include/>)

target_link_libraries(game
  engine
  OpenGL::GLES3
  glad
  glfw
  flecs
  imgui
  openal
  ogg
  vorbis
  vorbisfile)

set_property(TARGET glad PROPERTY C_STANDARD 11)
set_property(TARGET flecs PROPERTY C_STANDARD 99)
set_property(TARGET imgui PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET game PROPERTY CXX_STANDARD 20)

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
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)
add_library(engine ${SRC_FILES})

target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/tomlplusplus-3.4.0/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/flecs-3.2.7/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui-1.89.9/backends/>
  $<INSTALL_INTERFACE:include/>)

# Dependencies
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OggVorbis REQUIRED)

# Game include directories

add_executable(game ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/>
  $<INSTALL_INTERFACE:include/>)

target_link_libraries(game
  engine
  glfw
  OpenGL::GL
  glad
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

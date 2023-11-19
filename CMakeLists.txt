cmake_minimum_required(VERSION 3.10)
project(
  zelda3
  VERSION 0.1.0
  LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# Find SDL2
# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(
  SDL2
  REQUIRED
  CONFIG
  REQUIRED
  COMPONENTS SDL2)

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_subdirectory(snes)
add_subdirectory(third_party)
add_subdirectory(ppu_lib)
add_subdirectory(src)

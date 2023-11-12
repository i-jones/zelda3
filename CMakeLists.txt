cmake_minimum_required(VERSION 3.10)
project(
  zelda3
  VERSION 0.1.0
  LANGUAGES C)

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

add_executable(zelda3)

file(
  GLOB
  SOURCES
  "src/*.c"
  "snes/*.c")
target_sources(zelda3 PRIVATE ${SOURCES} third_party/gl_core/gl_core_3_1.c
                              third_party/opus-1.3.1-stripped/opus_decoder_amalgam.c)

target_include_directories(zelda3 PRIVATE ${PROJECT_SOURCE_DIR})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
  # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
  target_link_libraries(zelda3 PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(zelda3 PRIVATE SDL2::SDL2)

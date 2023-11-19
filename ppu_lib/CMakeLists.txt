include(FindProtobuf)

find_package(flatbuffers REQUIRED)

set(FB_SCHEMA "${CMAKE_CURRENT_SOURCE_DIR}/PPUCommands.fbs")
build_flatbuffers(
  "${FB_SCHEMA}"
  ""
  fbschemas
  ""
  "${CMAKE_CURRENT_SOURCE_DIR}/generated"
  ""
  "")
add_library(PPUFBSchemas INTERFACE)
target_include_directories(PPUFBSchemas INTERFACE ${FLATBUFFERS_INCLUDE_DIR})
add_dependencies(PPUFBSchemas fbschemas)

add_library(
  ppu_lib STATIC
  ppu_interface.h
  ppu_interface.cpp
  PPUBase.hpp
  PPUBase.cpp
  PPURecorder.hpp
  PPURecorder.cpp
  SNESPPU.hpp
  SNESPPU.cpp)

target_compile_features(ppu_lib PUBLIC cxx_std_20)

target_link_libraries(ppu_lib PUBLIC zelda3::snes)
target_link_libraries(ppu_lib PRIVATE PPUFBSchemas)

add_library(zelda3::ppu_lib ALIAS ppu_lib)

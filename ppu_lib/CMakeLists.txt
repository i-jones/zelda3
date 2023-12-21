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
  DebugPPU.hpp
  ppu_interface.h
  ppu_interface.cpp
  PPMWriter.hpp
  PPMWriter.cpp
  PPUBase.hpp
  PPUBase.cpp
  PPURecorder.hpp
  PPURecorder.cpp
  SNESPPU.hpp
  SNESPPU.cpp
  PPUFixedFrameRecorder.hpp
  PPUFixedFrameRecorder.cpp
  utils.hpp
  utils.cpp
  RenderBuffer.hpp
  RenderBuffer.cpp
  reference/ppu.hpp
  reference/ppu.cpp
  reference/register.hpp
  reference/register.cpp
  vec.hpp
  color.hpp
  reference/CharData.hpp
  reference/Objects.cpp
  reference/Objects.hpp
  reference/CGRam.hpp
  reference/VRAM.hpp)

target_compile_features(ppu_lib PUBLIC cxx_std_20)

target_link_libraries(ppu_lib PUBLIC zelda3::snes)
target_link_libraries(ppu_lib PRIVATE PPUFBSchemas)

add_library(zelda3::ppu_lib ALIAS ppu_lib)

add_executable(ppu_lib_tests tests/register_tests.cpp tests/chardata_tests.cpp tests/objects_tests.cpp)
target_link_libraries(ppu_lib_tests GTest::gtest_main ppu_lib)

include(GoogleTest)
gtest_discover_tests(ppu_lib_tests)

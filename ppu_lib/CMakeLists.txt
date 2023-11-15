add_library(
  ppu_lib STATIC
  ppu_interface.h
  ppu_interface.cpp
  PPUBase.hpp
  PPUBase.cpp
  SNESPPU.hpp
  SNESPPU.cpp)

target_compile_features(ppu_lib PUBLIC cxx_std_20)

target_link_libraries(ppu_lib PUBLIC zelda3::snes)

add_library(zelda3::ppu_lib ALIAS ppu_lib)

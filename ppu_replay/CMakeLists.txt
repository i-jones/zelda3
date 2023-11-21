find_package(CLI11 CONFIG REQUIRED)

add_executable(ppu-replay main.cpp)

target_link_libraries(
  ppu-replay
  zelda3::ppu_lib
  CLI11::CLI11
  PPUFBSchemas)

find_package(CLI11 CONFIG REQUIRED)

add_executable(ppu-replay main.cpp)

target_link_libraries(
  ppu-replay
  zelda3::ppu_lib
  CLI11::CLI11
  PPUFBSchemas)

set(ReplayTests ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame1.ppu)

foreach(test ${ReplayTests})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME replay-${TName} COMMAND $<TARGET_FILE:ppu-replay> -f ${test})
endforeach()

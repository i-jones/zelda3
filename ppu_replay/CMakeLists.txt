find_package(CLI11 CONFIG REQUIRED)

add_executable(ppu-replay main.cpp)

target_link_libraries(
  ppu-replay
  zelda3::ppu_lib
  CLI11::CLI11
  PPUFBSchemas
  GTest::gtest)

set(TestPpuTypes snes reference)
set(ReplayTests
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame1.ppu
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame1b.ppu
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame300.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame600.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame900.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame1500.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame1500_sparse.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/frame5500_sparse.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/diffFrame673.ppu.zstd
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/diffFrame5448.ppu.zstd)

foreach(testppu ${TestPpuTypes})
  foreach(test ${ReplayTests})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME replay-${testppu}-${TName} COMMAND $<TARGET_FILE:ppu-replay> -f ${test} -p ${testppu})
  endforeach()
endforeach()

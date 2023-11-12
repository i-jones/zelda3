add_library(
  snes STATIC
  apu.c
  cart.c
  cpu.c
  dma.c
  dsp.c
  input.c
  ppu.c
  snes_other.c
  snes.c
  spc.c
  tracing.c)

add_library(zelda3::snes ALIAS snes)

target_include_directories(snes PUBLIC ${PROJECT_SOURCE_DIR})

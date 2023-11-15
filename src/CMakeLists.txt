add_executable(
  zelda3
  ancilla.c
  attract.c
  audio.c
  config.c
  dungeon.c
  ending.c
  glsl_shader.c
  hud.c
  load_gfx.c
  main.c
  messaging.c
  misc.c
  nmi.c
  opengl.c
  overlord.c
  overworld.c
  player_oam.c
  player.c
  poly.c
  select_file.c
  spc_player.c
  sprite_main.c
  sprite.c
  tagalong.c
  tile_detect.c
  util.c
  zelda_cpu_infra.c
  zelda_rtl.c)

target_include_directories(zelda3 PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(
  zelda3
  PRIVATE zelda3::snes
          zelda3::gl_core
          zelda3::opus
          zelda3::ppu_lib)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
  # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
  target_link_libraries(zelda3 PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(zelda3 PRIVATE SDL2::SDL2)

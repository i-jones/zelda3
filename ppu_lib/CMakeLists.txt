add_library(ppu_lib STATIC ppu_interface.c ppu_interface.h)

target_link_libraries(ppu_lib PUBLIC zelda3::snes)

add_library(zelda3::ppu_lib ALIAS ppu_lib)

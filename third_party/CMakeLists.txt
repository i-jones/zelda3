add_library(gl_core STATIC gl_core/gl_core_3_1.c)

add_library(zelda3::gl_core ALIAS gl_core)

add_library(opus STATIC opus-1.3.1-stripped/opus_decoder_amalgam.c)

add_library(zelda3::opus ALIAS opus)
